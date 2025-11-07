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

HMODULE hModule;
HANDLE hUnloadEvent;

// Compute CRC32 of file. Streams safely.
static std::string GetCRC32(const std::filesystem::path& filePath) {
    CRC32 crc32;
    std::array<char, 4096> buffer{};
    std::ifstream fin(filePath, std::ifstream::binary);
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

// Basic checks for game files and integrity. Does not bypass anything.
// Returns true if basic sanity checks pass.
static bool GameVersionCheck() {
    try {
        auto modulePath = getModulePath(NULL);
        auto gameAssembly = modulePath.parent_path() / "GameAssembly.dll";
        auto steamApi = modulePath.parent_path() / "Among Us_Data" / "Plugins" / "x86" / "steam_api.dll";

        if (!std::filesystem::exists(gameAssembly)) {
            Log.Error("GameAssembly.dll was not found");
            MessageBox(NULL, L"Unable to locate GameAssembly.dll", L"SickoMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
            return false;
        }

        // Compute CRC for diagnostics only
        std::string gameAssemblyCRC = GetCRC32(gameAssembly);
        LOG_INFO(std::format("Detected GameAssembly.dll CRC32: {}", gameAssemblyCRC));

        // Place for additional platform or version checks if desired
        return true;
    }
    catch (const std::exception& ex) {
        LOG_ERROR(std::string("GameVersionCheck exception: ") + ex.what());
        return false;
    }
}

// Feature flags the rest of the code can check.
struct FeatureFlags {
    bool PlayerControlsEnabled = true;
    bool GameDataEnabled = true;
    bool RoleManagerEnabled = true;
} g_Features;

// Check presence of the il2cpp symbols your mod expects. Does not modify the game.
// This function relies on the pattern in your project where DO_APP_CLASS / DO_APP_FUNC
// populate TypeInfo pointers like SomeClass__TypeInfo.
static bool CheckRequiredIl2CppSymbols() {
    bool ok = true;

    // Example checks. Replace/add checks for the specific TypeInfo pointers you use.
    // These identifiers should be provided by your generated il2cpp-classes.h
    #ifdef _DEBUG
    // If your build generates these symbols when DO_APP_CLASS is included, they exist in debug.
    #endif

    if (!app::PlayerControl__TypeInfo) {
        LOG_ERROR("PlayerControl__TypeInfo missing. Disabling player-related features.");
        g_Features.PlayerControlsEnabled = false;
        ok = false;
    }
    if (!app::GameData__TypeInfo) {
        LOG_ERROR("GameData__TypeInfo missing. Disabling game-data features.");
        g_Features.GameDataEnabled = false;
        ok = false;
    }
    if (!app::RoleManager__TypeInfo) {
        LOG_WARN("RoleManager__TypeInfo missing. RoleManager-related functionality will be disabled.");
        g_Features.RoleManagerEnabled = false;
        // Not fatal; depends on how critical RoleManager is
    }

    return ok;
}

// Initialize pointers to game static instances in a safe, defensive way.
// If required symbols are missing the pointers will be set to nullptr and features
// will be disabled. This avoids calling into missing memory.
static void InitStaticPointersSafely() {
    // Clear first
    Game::pAmongUsClient = nullptr;
    Game::pGameData = nullptr;
    Game::pAllPlayerControls = nullptr;
    Game::pLocalPlayer = nullptr;
    Game::pShipStatus = nullptr;
    Game::pLobbyBehaviour = nullptr;

    // Only assign if TypeInfo exists and static field layout is as expected.
    if (g_Features.GameDataEnabled && app::GameData__TypeInfo && app::GameData__TypeInfo->static_fields) {
        Game::pGameData = &(app::GameData__TypeInfo->static_fields->Instance);
    } else {
        LOG_WARN("GameData instance not available. GameData-dependent features disabled.");
        g_Features.GameDataEnabled = false;
    }

    if (g_Features.PlayerControlsEnabled && app::PlayerControl__TypeInfo && app::PlayerControl__TypeInfo->static_fields) {
        Game::pAllPlayerControls = &(app::PlayerControl__TypeInfo->static_fields->AllPlayerControls);
        Game::pLocalPlayer = &(app::PlayerControl__TypeInfo->static_fields->LocalPlayer);
    } else {
        LOG_WARN("PlayerControl statics not available. PlayerControl features disabled.");
        g_Features.PlayerControlsEnabled = false;
    }

    if (app::AmongUsClient__TypeInfo && app::AmongUsClient__TypeInfo->static_fields) {
        Game::pAmongUsClient = &(app::AmongUsClient__TypeInfo->static_fields->Instance);
    } else {
        LOG_WARN("AmongUsClient instance not available.");
        // Not necessarily fatal.
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

// Initialize features after il2cpp is up. Gate usage of unsafe features.
static void InitFeatures() {
    // Do symbol presence checks
    bool base_ok = CheckRequiredIl2CppSymbols();

    // Initialize static pointers defensively
    InitStaticPointersSafely();

    // If critical things are missing, notify user and continue in degraded mode
    if (!base_ok) {
        std::ostringstream ss;
        ss << "SickoMenu detected incompatibilities with this Among Us build.\n\n"
           << "Some features will be disabled to avoid crashes.\n"
           << "See the log for details.\n\n"
           << "If you believe this is a false positive, rebuild the mod for the current game version.";
        std::string msg = ss.str();
        MessageBoxA(NULL, msg.c_str(), "SickoMenu - Compatibility", MB_OK | MB_ICONINFORMATION);
    }

    // Only scan and detour if the core symbols we need are present.
    if (g_Features.GameDataEnabled || g_Features.PlayerControlsEnabled) {
        Game::scanGameFunctions();
        DetourInitilization();
    } else {
        LOG_WARN("Core features disabled. Detours and game function scanning skipped.");
    }
}

// Main worker thread entry.
void Run(LPVOID lpParam) {
#if _DEBUG
    new_console();
#endif
    Log.Create();

    if (!GameVersionCheck()) {
        // Cleanup and exit thread if basic checks fail.
        fclose(stdout);
        FreeConsole();
        FreeLibraryAndExitThread((HMODULE)lpParam, 0);
        return;
    }

    hModule = (HMODULE)lpParam;
    State.lol = getModulePath(hModule).filename().string();

    // Initialize il2cpp runtime for symbol resolution
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

    // Populate DO_APP_CLASS / DO_APP_FUNC symbols if you rely on these generated headers.
    #if _DEBUG
    hUnloadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    #define DO_APP_CLASS(n, s) if(!n ## __TypeInfo) LOG_ERROR("Unable to locate " #n "__TypeInfo")
    #include "il2cpp-classes.h"
    #undef DO_APP_CLASS

    #define DO_APP_FUNC(r, n, p, s) if(!n) LOG_ERROR("Unable to locate " #n)
    #include "il2cpp-functions.h"
    #undef DO_APP_FUNC
    #endif

    // Set up feature gates and static pointers in a safe way.
    InitFeatures();

    // Set user/name and remaining initialization as before.
    State.userName = GetPlayerName();

    // Log role manager pointer; may be null in degraded mode.
    LOG_DEBUG(std::format("Game::RoleManager is {}", static_cast<void*>(Game::RoleManager.GetInstance())));

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
