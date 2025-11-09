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
//#include "gitparams.h"
// right here
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
    ss << "\n\tSickoMenu - " << __DATE__ << " - " << __TIME__ << std::endl;
    ss << "\tVersion: " << State.SickoVersion << std::endl;
    ss << "\tAmong Us Version: " << getGameVersion() << std::endl;
    LOG_INFO(ss.str());

#if _DEBUG
    SetConsoleTitleA(std::format("Debug Console - SickoMenu {} (Among Us v{})", State.SickoVersion, getGameVersion()).c_str());
#endif

    hUnloadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Initialize game pointers
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
        // Attempt normal deserialization
        GAME_STATIC_POINTER(Game::pNormalGameManager, app::NormalGameManager, Instance);
        auto mode = "Normal";
        int version = 10; // or dynamically detect if needed
        try {
            GameOptionsFactory::ReadIGameOptionsFromInternalMessage(mode, version);
        } catch (...) {
            LOG_WARNING("Unknown game options version " + std::to_string(version) + " - using default options");
            GameOptionsManager::set_CurrentGameOptions(DefaultGameOptions());
        }
    } catch (...) {
        LOG_WARNING("Failed to initialize NormalGameManager - continuing without crashing.");
    }

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
//remove this if it doesnt work
// test autoRelease main ver increment

HMODULE hModule;
HANDLE hUnloadEvent;

std::string GetCRC32(std::filesystem::path filePath) {
	CRC32 crc32;
	char buffer[4096] = { 0 };

	std::ifstream fin(filePath, std::ifstream::binary);

	while (!fin.eof()) {
		fin.read(&buffer[0], 4096);
		auto readSize = fin.gcount();
		crc32.add(&buffer[0], (size_t)readSize);
	}
	//LOG_DEBUG("CRC32 of \"" + filePath.string() + "\" is " + crc32.getHash());
	return crc32.getHash();
}

bool GameVersionCheck() {
	auto modulePath = getModulePath(NULL);
	auto gameAssembly = modulePath.parent_path() / "GameAssembly.dll";
	auto steamApi = modulePath.parent_path() / "Among Us_Data" / "Plugins" / "x86" / "steam_api.dll";

	/*if (!IsWindows10OrGreater()) {
		Log.Error("Version of windows not supported exiting!");
		MessageBox(NULL, L"This version of Windows is not supported!", L"SickoMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}*/

	if (!std::filesystem::exists(gameAssembly)) {
		Log.Error("GameAssembly.dll was not found");
		MessageBox(NULL, L"Unable to locate GameAssembly.dll", L"SickoMenu", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}

	std::string gameAssemblyCRC = GetCRC32(gameAssembly); //We won't use this, but it will log it

	return true;
}

#define ToString(s) stringify(s)
#define stringify(s) #s

#define GAME_STATIC_POINTER(f,c,m) \
	do \
	{ \
		if (!cctor_finished(c##__TypeInfo->_0.klass)) { \
			if (MessageBox(NULL, \
				L"SickoMenu does not support Among Us versions past v16.0.5 as of now!\n\nMake sure you downgrade your Among Us instance to v16.0.0 / v16.0.2 and use SickoMenu. If you're wondering about playing with players on the latest version, your game's version will automatically be spoofed so you can play with them!\n\nClick OK to exit the game. Your browser will then open a downgrading guide.", \
				L"SickoMenu", MB_ICONINFORMATION)) { \
				OpenLink("https://textbin.net/rruqqrlgaw"); \
				ExitProcess(0); \
			} \
		}; \
		f = &(c##__TypeInfo->static_fields->m); \
		std::ostringstream ss; \
		ss << std::internal << std::setfill('0') << std::hex << std::setw(8) \
		 << stringify(f) << " is 0x" << f << " -> 0x" << *f; \
		LOG_DEBUG(ss.str()); \
	} while (0);

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
		ss << "\n\tSickoMenu - " << __DATE__ << " - " << __TIME__ << std::endl; // Log SickoMenu info
		/*ss << "\tBuild: " << _CONFIGURATION_NAME << std::endl;
		ss << "\tCommit: " << GetGitCommit() << " - " << GetGitBranch() << std::endl; // Log git info*/
		ss << "\tVersion: " << State.SickoVersion << std::endl;
		ss << "\tAmong Us Version: " << getGameVersion() << std::endl; // Log among us info
		LOG_INFO(ss.str());
#if _DEBUG
		SetConsoleTitleA(std::format("Debug Console - SickoMenu {} (Among Us v{})", State.SickoVersion, getGameVersion()).c_str());
#endif
	}
#if _DEBUG
	hUnloadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

#define DO_APP_CLASS(n, s) if(!n ## __TypeInfo) LOG_ERROR("Unable to locate " #n "__TypeInfo")
#include "il2cpp-classes.h"
#undef DO_APP_CLASS

#define DO_APP_FUNC(r, n, p, s) if(!n) LOG_ERROR("Unable to locate " #n)
#include "il2cpp-functions.h"
#undef DO_APP_FUNC
	
	auto domain = il2cpp_domain_get();
	auto assembly = il2cpp_domain_assembly_open(domain, "Assembly-CSharp");
	//auto klass = il2cpp_class_from_name(assembly->image, "", "MovingPlatformBehaviour");
	//output_class_methods(klass);
	//output_assembly_methods(assembly);
	
#endif
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
