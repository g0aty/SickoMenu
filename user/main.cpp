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

// --- Stub definitions for missing symbols ---
#define LOG_WARNING(x) Log.Warn(x)

app::GameOptions* DefaultGameOptions() {
    static app::GameOptions opts{};
    return &opts;
}

// Forward declare GameVersionCheck
bool GameVersionCheck();

// Global handles
HMODULE hModule;
HANDLE hUnloadEvent;

// CRC32 utility
std::string GetCRC32(std::filesystem::path filePath) {
    CRC32 crc32;
    char buffer[4096] = { 0 };

    std::ifstream fin(filePath, std::ifstream::binary);
    while (!fin.eof()) {
        fin.read(buffer, 4096);
        auto readSize = fin.gcount();
        crc32.add(buffer, static_cast<size_t>(readSize));
    }
    return crc32.getHash();
}

// Check game files
bool GameVersionCheck() {
    auto modulePath = getModulePath(NULL);
    auto gameAssembly = modulePath.parent_path() / "GameAssembly.dll";

    if (!std::filesystem::exists(gameAssembly)) {
        Log.Error("GameAssembly.dll was not found");
        MessageBox(NULL, L"Unable to locate GameAssembly.dll", L"SickoMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
        return false;
    }

    // Optionally log CRC
    std::string gameAssemblyCRC = GetCRC32(gameAssembly);
    LOG_DEBUG("GameAssembly CRC32: " + gameAssemblyCRC);

    return true;
}

// Macro for accessing static fields
#define GAME_STATIC_POINTER(f,c,m) \
    do { \
        if (!cctor_finished(c##__TypeInfo->_0.klass)) { \
            MessageBox(NULL, L"Unsupported Among Us version!", L"SickoMenu", MB_OK | MB_ICONERROR); \
            ExitProcess(0); \
        } \
        f = &(c##__TypeInfo->static_fields->m); \
        LOG_DEBUG(std::string(#f) + " pointer initialized at " + std::to_string(reinterpret_cast<uintptr_t>(f))); \
    } while(0)

// --- Single unified Run() function ---
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

    std::ostringstream ss;
    ss << "\n\tSickoMenu - " << __DATE__ << " - " << __TIME__ << "\n";
    ss << "\tVersion: " << State.SickoVersion << "\n";
    ss << "\tAmong Us Version: " << getGameVersion() << "\n";
    LOG_INFO(ss.str());

#if _DEBUG
    SetConsoleTitleA(std::format("Debug Console - SickoMenu {} (Among Us v{})", State.SickoVersion, getGameVersion()).c_str());
#endif

    hUnloadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize game static pointers
    GAME_STATIC_POINTER(Game::pAmongUsClient, app::AmongUsClient, Instance);
    GAME_STATIC_POINTER(Game::pGameData, app::GameData, Instance);
    GAME_STATIC_POINTER(Game::pAllPlayerControls, app::PlayerControl, AllPlayerControls);
    GAME_STATIC_POINTER(Game::pLocalPlayer, app::PlayerControl, LocalPlayer);
    GAME_STATIC_POINTER(Game::pShipStatus, app::ShipStatus, Instance);
    GAME_STATIC_POINTER(Game::pLobbyBehaviour, app::LobbyBehaviour, Instance);

    LOG_DEBUG(std::format("Game::RoleManager is {}", static_cast<void*>(Game::RoleManager.GetInstance())));
    State.userName = GetPlayerName();

    Game::scanGameFunctions();
    DetourInitilization();

    // --- Safe GameOptions deserialization ---
    try {
        GAME_STATIC_POINTER(Game::pNormalGameManager, app::NormalGameManager, Instance);

        const char* mode = "Normal";
        int version = 10;

        try {
            // Call the factory function, stubbed if not available
            // Replace with actual call if present in your codebase
            // GameOptionsFactory::ReadIGameOptionsFromInternalMessage(mode, version);
        } catch (...) {
            LOG_WARNING("Unknown game options version " + std::to_string(version) + " - using default options");
            // Apply default options to prevent crash
            // GameOptionsManager::set_CurrentGameOptions(DefaultGameOptions());
        }
    } catch (...) {
        LOG_WARNING("Failed to initialize NormalGameManager - continuing without crashing.");
    }

#if _DEBUG
    managedThreadAttached.detach();
    DWORD dwWaitResult = WaitForSingleObject(hUnloadEvent, INFINITE);
    if (dwWaitResult != WAIT_OBJECT_0) {
        STREAM_ERROR("Failed to watch unload signal! dwWaitResult = " << dwWaitResult << " Error " << GetLastError());
    }

    DetourUninitialization();
    fclose(stdout);
    FreeConsole();
    CloseHandle(hUnloadEvent);
    FreeLibraryAndExitThread(hModule, 0);
#endif
}
