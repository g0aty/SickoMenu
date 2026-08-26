#include "pch-il2cpp.h"
#include "DirectX.h"
#include "Renderer.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "imgui/imstb_truetype.h"
#include "keybinds.h"
#include "menu.hpp"
#include "radar.hpp"
#include "replay.hpp"
#include "esp.hpp"
#include "state.hpp"
#include "theme.hpp"
#include <mutex>
#include "logger.h"
#include "resource_data.h"
#include "game.h"
#include "console.hpp"
#include "profiler.h"

#include <future>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND DirectX::window;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* pRenderTargetView = NULL;
D3D_PRESENT_FUNCTION oPresent = nullptr;
WNDPROC oWndProc;

HANDLE DirectX::hRenderSemaphore;
constexpr DWORD MAX_RENDER_THREAD_COUNT = 5; //Should be overkill for our purposes

std::vector<MapTexture> maps = std::vector<MapTexture>();
std::unordered_map<ICON_TYPES, IconTexture> icons;

typedef struct Cache
{
    ImGuiWindow* Window = nullptr;  //Window instance
    ImVec2       Winsize; //Size of the window
} cache_t;

static cache_t s_Cache;

ImVec2 DirectX::GetWindowSize()
{
    if (Screen_get_fullScreen(nullptr))
    {
        RECT rect;
        GetWindowRect(window, &rect);

        return { (float)(rect.right - rect.left),  (float)(rect.bottom - rect.top) };
    }

    return { (float)Screen_get_width(nullptr), (float)Screen_get_height(nullptr) };

}

static bool CanDrawEsp()
{
    return (!State.PanicMode && IsInGame() || IsInLobby()) && State.ShowEsp && (!State.InMeeting || !State.HideEsp_During_Meetings);
}

static bool CanDrawRadar()
{
    return !State.PanicMode && IsInGame() && State.ShowRadar && (!State.InMeeting || !State.HideRadar_During_Meetings);
}

static bool CanDrawReplay()
{
    return !State.PanicMode && IsInGame() && State.ShowReplay;
}

LRESULT __stdcall dWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (!State.ImGuiInitialized)
        return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);

    if (uMsg == WM_DPICHANGED && State.AdjustByDPI) {
        float dpi = HIWORD(wParam);
        State.dpiScale = dpi / 96.0f;
        State.dpiChanged = true;
        STREAM_DEBUG("DPI Scale: " << State.dpiScale);
    }

    if (uMsg == WM_SIZE) {
        // RenderTarget needs to be released because the resolution has changed 
        WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);
        if (pRenderTargetView) {
            pRenderTargetView->Release();
            pRenderTargetView = nullptr;
        }
        ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
    }

    if (!ImGui::GetIO().WantTextInput) {
        KeyBinds::WndProc(uMsg, wParam, lParam);
    }
    else {
        // let the menu get toggled while in any menu text field but suppress every other keybind like zoom etc
        switch (uMsg) {
        case WM_KEYDOWN: case WM_KEYUP: case WM_SYSKEYDOWN: case WM_SYSKEYUP:
            if ((uint8_t)wParam == State.KeyBinds.Toggle_Sicko || (uint8_t)wParam == State.KeyBinds.Toggle_Menu) // panic mode or show/hide menu
                KeyBinds::WndProc(uMsg, wParam, lParam);
            break;
        default:
            KeyBinds::WndProc(uMsg, wParam, lParam);
            break;
        }
    }

    bool shouldKeybindsActivate = !State.PanicMode && !State.KeybindsBeingEdited && (!State.ChatFocused || State.KeybindsWhileChatting) /*disable keybinds when chatting*/;

    if (shouldKeybindsActivate) {
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Menu)) State.ShowMenu = !State.ShowMenu;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Radar)) State.ShowRadar = !State.ShowRadar;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Console)) State.ShowConsole = !State.ShowConsole;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Repair_Sabotage) && IsInGame()) RepairSabotage(*Game::pLocalPlayer);
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Noclip) && (IsInGame() || IsInLobby())) { State.NoClip = !State.NoClip; State.HotkeyNoClip = true; }
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Autokill) && (IsInGame() || IsInLobby())) State.AutoKill = !State.AutoKill;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Close_All_Doors) && IsInGame()) State.CloseAllDoors = true;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Zoom) && (IsInGame() || IsInLobby())) {
            State.EnableZoom = !State.EnableZoom;
            if (!State.EnableZoom && Game::HudManager.IsInstanceExists()) {
                auto hud = Game::HudManager.GetInstance();
                bool isKillOverlayActive = hud->fields.KillOverlay != NULL &&
                    KillOverlay_get_IsOpen((KillOverlay*)hud->fields.KillOverlay, NULL);
                if (isKillOverlayActive) State.EnableZoom = true;
                // the ProgressTracker disappears if you disable zoom during the kill animation
            }
            State.HasRefreshedUI = false;
        }
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Freecam) && (IsInGame() || IsInLobby())) {
            State.FreeCam = !State.FreeCam;
            State.playerToFollow = {};
        }
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Close_Current_Room_Door) && IsInGame()) State.rpcQueue.push(new RpcCloseDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer)), false));
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Replay)) State.ShowReplay = !State.ShowReplay;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_ChatAlwaysActive) && (IsInGame() || IsInLobby())) State.ChatAlwaysActive = !State.ChatAlwaysActive;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_ReadGhostMessages) && (IsInGame() || IsInLobby())) State.ReadGhostMessages = !State.ReadGhostMessages;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Hud) && IsInGame()) State.DisableHud = !State.DisableHud;
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Reset_Appearance) && (IsInGame() || IsInLobby())) ControlAppearance(false);
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Randomize_Appearance)) ControlAppearance(true);
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Complete_Tasks) && IsInGame()) CompleteAllTasks();
        if (KeyBinds::IsKeyPressed(State.KeyBinds.Leave_Game) && (IsInGame() || IsInLobby()) && !State.PanicMode)
            app::AmongUsClient_ExitGame((*Game::pAmongUsClient), DisconnectReasons__Enum::ExitGame, NULL);
    }
    if (KeyBinds::IsKeyPressed(State.KeyBinds.Toggle_Sicko)) {
        State.PanicMode = !State.PanicMode;
        State.MIG_ThemeChanged = true;
        ReloadCurrentSceneIfNeeded();
    }

    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    if ((IsInGame() || IsInLobby()) && Game::HudManager.GetInstance()->fields.Chat != NULL && shouldKeybindsActivate) {
        auto chatState = Game::HudManager.GetInstance()->fields.Chat->fields.state;
        bool chatOpen = chatState == ChatControllerState__Enum::Open || chatState == ChatControllerState__Enum::Opening || chatState == ChatControllerState__Enum::Closing;
        bool isScrollModifierAllowed = !chatOpen && !State.InMeeting && State.EnableZoom_ScrollZoom;
        bool isShifted = ImGui::IsKeyDown(VK_SHIFT) || ImGui::IsKeyDown(VK_LSHIFT) || ImGui::IsKeyDown(VK_RSHIFT);

        if (!isShifted && isScrollModifierAllowed && State.EnableZoom && (IsInGame() || IsInLobby())) {
            if (ImGui::GetIO().MouseWheel < 0.f)  State.CameraHeight += 0.5f;
            if (ImGui::GetIO().MouseWheel > 0.f) {
                State.CameraHeight -= 0.5f;
                if (State.CameraHeight < 1.f) State.CameraHeight = 1.f;
            }
        }
        if (isShifted &&
            isScrollModifierAllowed && State.FreeCam && (IsInGame() || IsInLobby())) {
            if (ImGui::GetIO().MouseWheel < 0.f) State.FreeCamSpeed += 0.1f;
            if (ImGui::GetIO().MouseWheel > 0.f) {
                State.FreeCamSpeed -= 0.1f;
                if (State.FreeCamSpeed <= 0.f) State.FreeCamSpeed = 0.1f;
            }
        }
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool ImGuiInitialization(IDXGISwapChain* pSwapChain) {
    if ((pDevice != NULL) || (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))) {
        pDevice->GetImmediateContext(&pContext);
        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        DirectX::window = sd.OutputWindow;
        ID3D11Texture2D* pBackBuffer = NULL;
        pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        if (!pBackBuffer)
            return false;
        pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
        pBackBuffer->Release();
        oWndProc = (WNDPROC)SetWindowLongPtr(DirectX::window, GWLP_WNDPROC, (LONG_PTR)dWndProc);
        /*if (State.AdjustByDPI) {
            State.dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(DirectX::window);
        }
        else {
            State.dpiScale = 1.0f;
        }
        State.dpiChanged = true;*/

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        ImGui_ImplWin32_Init(DirectX::window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        maps.push_back({ D3D11Image(Resource(IDB_PNG1), pDevice), 277.F, 77.F, 11.5F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG2), pDevice), 115.F, 240.F, 9.25F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG3), pDevice), 8.F, 21.F, 10.F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG4), pDevice), 162.F, 107.F, 6.F });
        maps.push_back({ D3D11Image(Resource(IDB_PNG15), pDevice), 237.F, 140.F, 8.5F });

        icons.insert({ ICON_TYPES::VENT_IN, { D3D11Image(Resource(IDB_PNG5), pDevice), 0.02f }});
        icons.insert({ ICON_TYPES::VENT_OUT, { D3D11Image(Resource(IDB_PNG6), pDevice), 0.02f }});
        icons.insert({ ICON_TYPES::KILL, { D3D11Image(Resource(IDB_PNG7), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::REPORT, { D3D11Image(Resource(IDB_PNG8), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::TASK, { D3D11Image(Resource(IDB_PNG9), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::PLAYER, { D3D11Image(Resource(IDB_PNG10), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::CROSS, { D3D11Image(Resource(IDB_PNG11), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::DEAD, { D3D11Image(Resource(IDB_PNG12), pDevice), 0.02f } });
        icons.insert({ ICON_TYPES::PLAY, { D3D11Image(Resource(IDB_PNG13), pDevice), 0.55f } });
        icons.insert({ ICON_TYPES::PAUSE, { D3D11Image(Resource(IDB_PNG14), pDevice), 0.55f } });
        icons.insert({ ICON_TYPES::PLAYERVISOR, { D3D11Image(Resource(IDB_PNG16), pDevice), 0.02f } });

        DirectX::hRenderSemaphore = CreateSemaphore(
            NULL,                                 // default security attributes
            MAX_RENDER_THREAD_COUNT,              // initial count
            MAX_RENDER_THREAD_COUNT,              // maximum count
            NULL);                                // unnamed semaphore);
        return true;
    }
    
    return false;
}

static const ImWchar* GetAllGlyphRanges(ImGuiIO& io) {
    static ImVector<ImWchar> ranges;

    if (ranges.empty()) {
        auto add = [&](const ImWchar* r)
            {
                while (r[0] != 0 || r[1] != 0)
                {
                    ranges.push_back(r[0]);
                    ranges.push_back(r[1]);
                    r += 2;
                }
            };

        static const ImWchar allCharsRanges[] =
        {
            0x0020, 0xFFFF,
            0,
        };

        add(&allCharsRanges[0]);
        ranges.push_back(0);
        ranges.push_back(0);
    }

    return ranges.Data;
}

static void RebuildFont() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14 * State.dpiScale, nullptr,
        GetAllGlyphRanges(io));

    io.Fonts->Build();
}

std::once_flag init_d3d;
HRESULT __stdcall dPresent(IDXGISwapChain* __this, UINT SyncInterval, UINT Flags) {
    std::call_once(init_d3d, [&] {
        if (SUCCEEDED(__this->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
        }
    });
    if (!State.ImGuiInitialized) {
        if (ImGuiInitialization(__this)) {
            ImVec2 size = DirectX::GetWindowSize();
            State.ImGuiInitialized = true;
            STREAM_DEBUG("ImGui Initialized successfully!");
            STREAM_DEBUG("Fullscreen: " << Screen_get_fullScreen(nullptr));
            STREAM_DEBUG("Unity Window Resolution: " << +Screen_get_width(nullptr) << "x" << +Screen_get_height(nullptr));
            STREAM_DEBUG("DirectX Window Size: " << +size.x << "x" << +size.y);
        } else {
            ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);
            return oPresent(__this, SyncInterval, Flags);
        }
    }

    if (!Profiler::HasInitialized)
    {
        Profiler::InitProfiling();
    }

    WaitForSingleObject(DirectX::hRenderSemaphore, INFINITE);

    // resolution changed
    if (!pRenderTargetView) {
        ID3D11Texture2D* pBackBuffer = nullptr;
        __this->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        assert(pBackBuffer);
        pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
        pBackBuffer->Release();

        ImVec2 size = DirectX::GetWindowSize();
        STREAM_DEBUG("Unity Window Resolution: " << +Screen_get_width(nullptr) << "x" << +Screen_get_height(nullptr));
        STREAM_DEBUG("DirectX Window Size: " << +size.x << "x" << +size.y);
    }

    if (State.dpiChanged) {
        State.dpiChanged = false;
        ImGui_ImplDX11_InvalidateDeviceObjects();
        RebuildFont();
    }

    il2cpp_gc_disable();

    ApplyTheme();
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (State.PanicMode && State.TempPanicMode && *Game::pAmongUsClient == nullptr) {
        State.PanicMode = false;
        State.TempPanicMode = false;
    }

    if (!State.PanicMode && State.ShowMenu)
    {
        ImGuiRenderer::Submit([]() { Menu::Render(); });
    }

    if (!State.PanicMode && State.ShowConsole)
    {
        ImGuiRenderer::Submit([]() { ConsoleGui::Render(); });
    }

    if (CanDrawEsp()) {
        ImGuiRenderer::Submit([&]()
        {
            //Push ImGui flags
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f * State.dpiScale);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });

            //Setup BackBuffer
            ImGui::Begin("BackBuffer", reinterpret_cast<bool*>(true),
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);

            s_Cache.Winsize = DirectX::GetWindowSize();
            s_Cache.Window = ImGui::GetCurrentWindow();

            //Set window properties
            ImGui::SetWindowPos({ 0, 0 }, ImGuiCond_Always);
            ImGui::SetWindowSize(s_Cache.Winsize, ImGuiCond_Always);

            Esp::Render();

            s_Cache.Window->DrawList->PushClipRectFullScreen();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::End();
        });
    }

    if (CanDrawRadar())
    {
        ImGuiRenderer::Submit([]() { Radar::Render(); });
    }
    else State.HoveringOverAnyWindowButRadar = ImGui::GetIO().WantCaptureMouse;

    if (CanDrawReplay())
    {
        ImGuiRenderer::Submit([]() { Replay::Render(); });
    }

    // Render in a separate thread
    std::async(std::launch::async, ImGuiRenderer::ExecuteQueue).wait();

    ImGui::EndFrame();
    ImGui::Render();

    pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    il2cpp_gc_enable();

    HRESULT result = oPresent(__this, SyncInterval, Flags);

    ReleaseSemaphore(DirectX::hRenderSemaphore, 1, NULL);

    return result;
}

void DirectX::Shutdown() {
    assert(hRenderSemaphore != NULL); //Initialization is now in a hook, so we might as well guard against this
    for (uint8_t i = 0; i < MAX_RENDER_THREAD_COUNT; i++) //This ugly little hack means we use up all the render queues so we can end everything
    {
        assert(WaitForSingleObject(hRenderSemaphore, INFINITE) == WAIT_OBJECT_0); //Since this is only used on debug builds, we'll leave this for now
    }
    oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CloseHandle(hRenderSemaphore);
}
