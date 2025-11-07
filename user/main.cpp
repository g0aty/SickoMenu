// main.cpp
#include "pch-il2cpp.h"
#include "main.h"
#include "il2cpp-init.h"
#include <VersionHelpers.h>
#include "crc32.h"
#include <shellapi.h>
#include <iostream>
#include "game.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"
#include "version.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <array>

HMODULE hModule = nullptr;
HANDLE hUnloadEvent = nullptr;

static std::string GetCRC32(const std::filesystem::path& filePath) {
    CRC32 crc32;
    std::array<char, 4096> buffer{};
    std::ifstream fin(filePath, std::ios::binary);
    if (!fin.is_open()) {
        LOG_ERROR(std::format("GetCRC32: failed to open {}", filePath.string()));
        return std::string();
    }

    while (fin.good()) {
        fin.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        std::streamsize r = fin.gcount();
        if (r > 0) crc32.add(buffer.data(), static_cast<size_t>(r));
    }
    return crc32.getHash();
}

static bool GameVersionCheck() {
    try {
        auto modulePath = getModulePath(NULL);
        auto gameAssembly = modulePath.parent_path() / "GameAssembly.dll";

        if (!std::filesystem::exists(gameAssembly)) {
            Log.Error("GameAssembly.dll was not found");
            MessageBox(NULL, L"Unable to locate GameAssembly.dll", L"SickoMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
            return false;
        }

        std::string gameAssemblyCRC = GetCRC32(gameAssembly);
        LOG_INFO(std::format("Detected GameAssembly.dll CRC32: {}", gameAssemblyCRC));
        return true;
    }
    catch (const std::exception& ex) {
        LOG_ERROR(std::string("GameVersionCheck exception: ") + ex.what());
        return false;
    }
}

// Feature flags
struct FeatureFlags {
    bool PlayerControlsEnabled = true;
    bool GameDataEnabled = true;
} g_Features;

static bool CheckRequiredIl2CppSymbols() {
    bool ok = true;

    // These identifiers are expected to come from your il2cpp-classes.h
    // If any are missing at runtime this code will disable the features.
    // Note: do not reference symbols that don't exist at compile time.
    // We assume PlayerControl__TypeInfo and GameData__TypeInfo are generated normally.
#if defined(_DEBUG)
    // In debug builds we already include diagnostic checks elsewhere.
#endif

    // Use plain pointer-check style. If symbols are missing at compile time
    // this code should be adjusted to match your generated headers.
    if (!app::PlayerControl__TypeInfo) {
        LOG_DEBUG("PlayerControl__TypeInfo missing. Disabling player-related features.");
        g_Features.PlayerControlsEnabled = false;
        ok = false;
    }
    if (!app::GameData__TypeInfo) {
        LOG_DEBUG("GameData__TypeInfo missing. Disabling game-data features.");
        g_Features.GameDataEnabled = false;
        ok = false;
    }

    return ok;
}

static void InitStaticPointersSafely() {
    Game::pAmongUsClient = nullptr;
    Game::pGameData = nullptr;
    Game::pAllPlayerControls = nullptr;
    Game::pLocalPlayer = nullptr;
    Game::pShipStatus = nullptr;
    Game::pLobbyBehaviour = nullptr;

    if (g_Features.GameDataEnabled && app::GameData__TypeInfo && app::GameData__TypeInfo->static_fields) {
        Game::pGameData = &(app::GameData__TypeInfo->static_fields->Instance);
    } else {
        LOG_DEBUG("GameData instance not available. GameData-dependent features disabled.");
        g_Features.GameDataEnabled = false;
    }

    if (g_Features.PlayerControlsEnabled && app::PlayerControl__TypeInfo && app::PlayerControl__TypeInfo->static_fields) {
        Game::pAllPlayerControls = &(app::PlayerControl__TypeInfo->static_fields->AllPlayerControls);
        Game::pLocalPlayer = &(app::PlayerControl__TypeInfo->static_fields->LocalPlayer);
    } else {
        LOG_DEBUG("PlayerControl statics not available. PlayerControl features disabled.");
        g_Features.PlayerControlsEnabled = false;
    }

    if (app::AmongUsClient__TypeInfo && app::AmongUsClient__TypeInfo->static_fields) {
        Game::pAmongUsClient = &(app::AmongUsClient__TypeInfo->static_fields->Instance);
    } else {
        LOG_DEBUG("AmongUsClient instance not available.");
    }

    if (app::ShipStatus__TypeInfo && app::ShipStatus__TypeInfo->static_fields) {
        Game::pShipStatus = &(app::ShipStatus__TypeInfo->static_fields->Instance);
    }

    if (app::LobbyBehaviour__TypeInfo && app::LobbyBehaviour__TypeInfo->static_fields) {
        Game::pLobbyBehaviour = &(app::LobbyBehaviour__TypeInfo->static_fields->Instance);
    }

    LOG_DEBUG(std::format("InitStaticPointersSafely: pLocalPlayer {} pGameData {} pAmongUsClient {}",
        static_cast<void*>(Game::pLocalPlayer),
        static_cast<void*>(Game::pGameData),
        static_cast<void*>(Game::pAmongUsClient)));
}

static void InitFeatures() {
    bool base_ok = CheckRequiredIl2CppSymbols();
    InitStaticPointersSafely();

    if (!base_ok) {
        std::ostringstream ss;
        ss << "SickoMenu detected incompatibilities with this Among Us build.\n\n"
           << "Some features will be disabled to avoid crashes.\n"
           << "See the log for details.\n\n";
        std::string msg = ss.str();
        MessageBoxA(NULL, msg.c_str(), "SickoMenu - Compatibility", MB_OK | MB_ICONINFORMATION);
    }

    if (g_Features.GameDataEnabled || g_Features.PlayerControlsEnabled) {
        Game::scanGameFunctions();
        DetourInitilization();
    } else {
        LOG_DEBUG("Core features disabled. Detours and game function scanning skipped.");
    }
}

void Run(LPVOID lpParam) {
#if _DEBUG
    new_console();
#endif

    Log.Create();

    if (!GameVersionCheck()) {
        fclose(stdout);
        FreeConsole();
        FreeLibraryAndExitThread((HMODULE)lpParam, 0);
        return;
    }

    hModule = (HMODULE)lpParam;
    State.lol = getModulePath(hModule).filename().string();

    init_il2cpp();
    State.Load();

    ScopedThreadAttacher managedThreadAttached;

    {
        std::ostringstream ss;
        ss << "\n\tSickoMenu - " << __DATE__ << " - " << __TIME__ << std::endl;
        ss << "\tVersion: " << State.SickoVersion << std::endl;
        ss << "\tAmong Us Version: " << getGameVersion() << std::endl;
        LOG_INFO(ss.str());
#if _DEBUG
        SetConsoleTitleA(std::format("Debug Console - SickoMenu {} (Among Us v{})", State.SickoVersion, getGameVersion()).c_str());
#endif
    }

#if _DEBUG
    hUnloadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    #define DO_APP_CLASS(n, s) if(!n ## __TypeInfo) LOG_DEBUG("Unable to locate " #n "__TypeInfo")
    #include "il2cpp-classes.h"
    #undef DO_APP_CLASS

    #define DO_APP_FUNC(r, n, p, s) if(!n) LOG_DEBUG("Unable to locate " #n)
    #include "il2cpp-functions.h"
    #undef DO_APP_FUNC
#endif

    InitFeatures();

    State.userName = GetPlayerName();

    // Removed RoleManager reference to avoid compile errors when symbol is not present.
    LOG_DEBUG(std::format("Game::pLocalPlayer is {}", static_cast<void*>(Game::pLocalPlayer)));
    LOG_DEBUG(std::format("Game::pGameData is {}", static_cast<void*>(Game::pGameData)));

#if _DEBUG
    managedThreadAttached.detach();
    DWORD dwWaitResult = WaitForSingleObject(hUnloadEvent, INFINITE);
    if (dwWaitResult != WAIT_OBJECT_0) {
        STREAM_ERROR("Failed to watch unload signal! dwWaitResult = " << dwWaitResult << " Error " << GetLastError());
        return;
    }

    DetourUninitialization();
    fclose(stdout);
    FreeConsole();
    CloseHandle(hUnloadEvent);
    FreeLibraryAndExitThread(hModule, 0);
#endif
}
