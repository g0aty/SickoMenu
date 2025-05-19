#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <format>

//std::unordered_set<std::string> glitchEndings = { "IJPG", "YTHG", "WYWG", "KHQG", "FUGG", "UFLG", "KJQG", "ZQCG", "GEWG", "NPPG", "SZAF", "PATG", "PJDG", "TPYG", "JTFG", "VDXG", "DHSG", "TQQG", "ALGG", "UMPG", "GFXG", "RGGG", "HQXG", "LDQG", "ZLHG", "WMPG", "TAGG", "FBGG", "EJYG", "AOTG", "LCAF", "DORG", "ZCQG" };

extern bool editingAutoStartPlayerCount;

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

	
	if (IsHost() && State.AutoStartGamePlayers && IsInLobby() && !editingAutoStartPlayerCount) {  //this makes sure they dont start the game by mistake, if they are typing a 2 digit number eg 12
		int playerCount = (int)GetAllPlayerData().size();
		if (playerCount >= State.AutoStartPlayerCount) {
			app::InnerNetClient_SendStartGame((InnerNetClient*)(*Game::pAmongUsClient), NULL);
			State.AutoStartGamePlayers = false;
			State.Save();
		}
	}
	
}

void dMatchMakerGameButton_SetGame(MatchMakerGameButton* __this, GameListing gameListing, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dMatchMakerGameButton_SetGame executed");
	/*if (State.PanicMode || !State.ShowLobbyInfo) return MatchMakerGameButton_SetGame(__this, gameListing, method);
	MatchMakerGameButton_SetGame(__this, gameListing, method);
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
	int LobbyTime = (std::max)(0, int(gameListing.Age));
	std::string lobbyTimeDisplay = "";
	if (State.ShowLobbyTimer) {
		lobbyTimeDisplay = std::format(" ~ <#0f0>Age: {}:{}{}</color>", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
	}
	std::string hostName = std::format("<size=50%>{} <#fb0>{}</color>\n<#b0f>{}</color>{}</size>", convert_from_string(gameListing.TrueHostName), lobbyCode, platformId, lobbyTimeDisplay, ServerMode);
	TMP_Text_set_text((TMP_Text*)__this->fields.NameText, convert_to_string(hostName), NULL);
	TMP_Text_set_text((TMP_Text*)__this->fields.SmallNameText, convert_to_string(hostName), NULL);
	*/ // Deprecated
}

void dGameContainer_SetupGameInfo(GameContainer* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dGameContainer_SetupGameInfo executed");
	if (State.PanicMode || !State.ShowLobbyInfo) return GameContainer_SetupGameInfo(__this, method);
	GameContainer_SetupGameInfo(__this, method);
	auto gameListing = __this->fields.gameListing;
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
	std::string lobbyCode = IsStreamerMode() ? "******" : convert_from_string(InnerNet_GameCode_IntToGameName(gameListing.GameId, NULL));
	int LobbyTime = (std::max)(0, int(gameListing.Age));
	std::string lobbyTimeDisplay = "";
	if (State.ShowLobbyTimer) {
		lobbyTimeDisplay = std::format("\n<#0f0>Age: {}:{}{}</color>", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
	}
	std::string playerCountCol = "<#0f0>";
	if (gameListing.PlayerCount == 4) playerCountCol = "<#ff0>";
	if (gameListing.PlayerCount < 4) playerCountCol = "<#f00>";
	std::string playerCount = playerCountCol + convert_from_string(TMP_Text_get_text((TMP_Text*)__this->fields.capacity, NULL)) + "</color>";
	std::string trueHostName = convert_from_string(gameListing.TrueHostName);
	std::string separator = "<#0000>000000000000000</color>"; // The crewmate icon gets aligned properly with this
	std::string playerCountDisplay = std::format("<size=40%>{}\n{}\n{}\n<#fb0>{}</color>\n<#b0f>{}</color>{}\n{}</size>", separator, trueHostName, playerCount, lobbyCode, platformId, lobbyTimeDisplay, separator);
	TMP_Text_set_text((TMP_Text*)__this->fields.capacity, convert_to_string(playerCountDisplay), NULL);
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

static int findGameOffset = 0;

void dFindAGameManager_Update(FindAGameManager* __this, MethodInfo* method) {
	FindAGameManager_Update(__this, method);
	// Useful for later
}