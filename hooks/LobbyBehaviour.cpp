#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <format>

//std::unordered_set<std::string> glitchEndings = { "IJPG", "YTHG", "WYWG", "KHQG", "FUGG", "UFLG", "KJQG", "ZQCG", "GEWG", "NPPG", "SZAF", "PATG", "PJDG", "TPYG", "JTFG", "VDXG", "DHSG", "TQQG", "ALGG", "UMPG", "GFXG", "RGGG", "HQXG", "LDQG", "ZLHG", "WMPG", "TAGG", "FBGG", "EJYG", "AOTG", "LCAF", "DORG", "ZCQG" };

void dLobbyBehaviour_Start(LobbyBehaviour* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLobbyBehaviour_Start executed");
	State.LobbyTimer = 600;
	LobbyBehaviour_Start(__this, method);
	if (IsHost()) State.JoinedAsHost = true;
}

void dLobbyBehaviour_Update(LobbyBehaviour* __this, MethodInfo* method)
{
	static bool hasStarted = true;
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLobbyBehaviour_Update executed");
	LobbyBehaviour_Update(__this, method);
	if (State.DisableLobbyMusic) {
		hasStarted = false;
		SoundManager_StopSound(SoundManager__TypeInfo->static_fields->instance, (AudioClip*)__this->fields.MapTheme, NULL);
	}
	else if (!hasStarted) {
		hasStarted = true;
		LobbyBehaviour_Start(__this, method); //restart lobby music
	}
	if (GameOptions().GetByte(app::ByteOptionNames__Enum::MapId) == 3) {
		GameOptions().SetByte(app::ByteOptionNames__Enum::MapId, 0);
		auto gameOptionsManager = GameOptionsManager_get_Instance(NULL);
		GameManager* gameManager = GameManager_get_Instance(NULL);
		GameOptionsManager_set_GameHostOptions(gameOptionsManager, GameOptionsManager_get_CurrentGameOptions(gameOptionsManager, NULL), NULL);
		LogicOptions_SyncOptions(GameManager_get_LogicOptions(gameManager, NULL), NULL);
	}
	State.LobbyTimer -= Time_get_deltaTime(NULL);
}

void dMatchMakerGameButton_SetGame(MatchMakerGameButton* __this, GameListing gameListing, MethodInfo* method) {
	if (State.PanicMode || !State.ShowLobbyInfo) return MatchMakerGameButton_SetGame(__this, gameListing, method);
	auto platform = gameListing.Platform;
	std::string platformId = "Unknown";
	switch (platform) {
	case Platforms__Enum::StandaloneEpicPC:
		platformId = "Epic Games";
		break;
	case Platforms__Enum::StandaloneSteamPC:
		platformId = "Steam";
		break;
	case Platforms__Enum::StandaloneMac:
		platformId = "Mac";
		break;
	case Platforms__Enum::StandaloneWin10:
		platformId = "Microsoft Store";
		break;
	case Platforms__Enum::StandaloneItch:
		platformId = "itch.io";
		break;
	case Platforms__Enum::IPhone:
		platformId = "iOS/iPadOS";
		break;
	case Platforms__Enum::Android:
		platformId = "Android";
		break;
	case Platforms__Enum::Switch:
		platformId = "Nintendo Switch";
		break;
	case Platforms__Enum::Xbox:
		platformId = "Xbox";
		break;
	case Platforms__Enum::Playstation:
		platformId = "Playstation";
		break;
	default:
		platformId = "Unknown";
		break;
	}
	std::string lobbyCode = IsStreamerMode() ? "" : convert_from_string(InnerNet_GameCode_IntToGameName(gameListing.GameId, NULL));

	/*	std::string glitchDisplay = "";
		if (!State.PanicMode && State.ShowLobbyInfo) {
			std::string codeEnding = lobbyCode.substr(lobbyCode.length() - 4);
			if (glitchEndings.find(codeEnding) != glitchEndings.end()) glitchDisplay = " *";
		}

		lobbyCode += glitchDisplay;
	*/
	int LobbyTime = (std::max)(0, int(gameListing.Age));
	std::string lobbyTimeDisplay = "";
	if (State.ShowLobbyTimer) {
		lobbyTimeDisplay = std::format(" ~ <#0f0>Age: {}:{}{}</color>", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
	}
	std::string hostName = convert_from_string(gameListing.HostName);
	gameListing.HostName = convert_to_string(std::format("<size=50%>{} <#fb0>{}</color>\n<#b0f>{}</color>{}</size>", hostName, lobbyCode, platformId, lobbyTimeDisplay/*, ServerMode*/));
	MatchMakerGameButton_SetGame(__this, gameListing, method);
}

void dGameStartManager_Update(GameStartManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dGameStartManager_Update executed");
	try {
		std::string LobbyCode = convert_from_string(InnerNet_GameCode_IntToGameName((*Game::pAmongUsClient)->fields._.GameId, NULL));
		int LobbyTime = (int)State.LobbyTimer;
		std::string lobbyTimeDisplay = "";
		if (!State.PanicMode && State.ShowLobbyTimer && IsHost()) {
			if (LobbyTime < 0)
				lobbyTimeDisplay = std::format(" <#0f0>({}0:00)</color>", State.JoinedAsHost ? "" : "~");
			else if (LobbyTime <= 60)
				lobbyTimeDisplay = std::format(" <#f00>({}{}:{}{})</color>", State.JoinedAsHost ? "" : "~", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
			else if (LobbyTime <= 180)
				lobbyTimeDisplay = std::format(" <#ff0>({}{}:{}{})</color>", State.JoinedAsHost ? "" : "~", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
			else
				lobbyTimeDisplay = std::format(" ({}{}:{}{})", State.JoinedAsHost ? "" : "~", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
		}
		/*std::string glitchDisplay = "";
		if (!State.PanicMode && State.ShowLobbyInfo) {
			std::string codeEnding = LobbyCode.substr(LobbyCode.length() - 4);
			if (glitchEndings.find(codeEnding) != glitchEndings.end()) glitchDisplay = " * ";
		}*/

		if (State.HideCode && IsStreamerMode() && !State.PanicMode && LobbyCode != "") {
			std::string customCode = State.HideCode && IsStreamerMode() ? State.customCode : "******";
			if (State.RgbLobbyCode)
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(State.rgbCode + /*glitchDisplay +*/ customCode + lobbyTimeDisplay), NULL);
			else
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(/*glitchDisplay +*/ customCode + lobbyTimeDisplay), NULL);
		}
		else {
			if (State.RgbLobbyCode && !State.PanicMode)
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(State.rgbCode + /*glitchDisplay +*/ LobbyCode + lobbyTimeDisplay), NULL);
			else
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(LobbyCode + /*glitchDisplay +*/ lobbyTimeDisplay), NULL);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in GameStartManager_Update (LobbyBehaviour)");
	}
	GameStartManager_Update(__this, method);
}