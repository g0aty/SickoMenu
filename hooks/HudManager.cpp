#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "utility.h"
#include "game.h"

void dHudManager_Update(HudManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dHudManager_Update executed");
	try {
		static bool bChatAlwaysActivePrevious = false;
		if (bChatAlwaysActivePrevious != State.ChatAlwaysActive)
		{
			if (State.ChatAlwaysActive && !State.PanicMode)
				ChatController_SetVisible(__this->fields.Chat, true, NULL);
			else if (!State.InMeeting && !IsInLobby()) //You will lose chat ability in meeting otherwise
				ChatController_SetVisible(__this->fields.Chat, State.ChatActiveOriginalState, NULL);
			bChatAlwaysActivePrevious = State.ChatAlwaysActive;
		}
		__this->fields.PlayerCam->fields.Locked = State.FreeCam && !State.PanicMode;

		if (__this->fields.Chat && __this->fields.Chat->fields.freeChatField) {
			__this->fields.Chat->fields.freeChatField->fields.textArea->fields.AllowPaste = State.ChatPaste && !State.PanicMode;
		}


		static bool DisableActivation = false; //so a ghost seek button doesn't show up

		if (State.InMeeting)
			HudManager_SetHudActive(__this, false, NULL);
		else {
			if (State.DisableHud && !State.PanicMode) {
				HudManager_SetHudActive(__this, false, NULL);
				DisableActivation = false;
			}
			else if (!DisableActivation) {
				HudManager_SetHudActive(__this, true, NULL);
				DisableActivation = true;
			}
		}

		if ((IsInGame() || IsInLobby())) {
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			if (!localData) {
				// oops: game bug
				return;
			}
			GameObject* shadowLayerObject = Component_get_gameObject((Component_1*)__this->fields.ShadowQuad, NULL);
			if (IsInGame() || IsInLobby()) {
				if (shadowLayerObject != NULL)
					GameObject_SetActive(shadowLayerObject,
						(State.PanicMode || (!(State.IsRevived || State.FreeCam || State.EnableZoom || State.playerToFollow.has_value() || State.Wallhack || (State.MaxVision && IsInLobby()))))
						&& !localData->fields.IsDead,
						NULL);

				if (State.OutfitCooldown == 0) {
					if (!State.CanChangeOutfit && IsInLobby() && !State.PanicMode && State.confuser && State.confuseOnJoin)
						ControlAppearance(true);
					State.CanChangeOutfit = true;
				}
				else if (State.OutfitCooldown == 25 && State.PanicMode) {
					if (State.TempPanicMode) {
						State.PanicMode = false;
						State.TempPanicMode = false;
					}
					State.OutfitCooldown--;
					ChatController_SetVisible(__this->fields.Chat, true, NULL);
				}
				else State.OutfitCooldown--;
			}

			if (!State.InMeeting && !State.DisableHud)
			{
				app::RoleBehaviour* playerRole = localData->fields.Role; // Nullable
				app::RoleTypes__Enum role = playerRole != nullptr ? playerRole->fields.Role : app::RoleTypes__Enum::Crewmate;
				GameObject* ImpostorVentButton = app::Component_get_gameObject((Component_1*)__this->fields.ImpostorVentButton, NULL);

				if (ImpostorVentButton != NULL) {
					if (role == RoleTypes__Enum::Engineer && State.UnlockVents && !State.PanicMode)
					{
						app::EngineerRole* engineerRole = (app::EngineerRole*)playerRole;
						if (engineerRole->fields.cooldownSecondsRemaining > 0.0f)
							engineerRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
						engineerRole->fields.inVentTimeRemaining = 30.0f; //Can be anything as it will always be written
					}
					else if ((GetPlayerData(*Game::pLocalPlayer)->fields.IsDead || IsInLobby()))
					{
						app::GameObject_SetActive(ImpostorVentButton, false, nullptr);
					}
					else
					{
						app::GameObject_SetActive(ImpostorVentButton, (State.UnlockVents && !State.PanicMode) || (((*Game::pLocalPlayer)->fields.inVent && role != RoleTypes__Enum::Engineer)) || (PlayerIsImpostor(localData) && GameOptions().GetGameMode() == GameModes__Enum::Normal), nullptr);
					}
				}

				if ((IsInGame() || (IsInLobby() && State.KillInLobbies))) {
					bool amImpostor = false;
					try {
						amImpostor = PlayerIsImpostor(localData);
					}
					catch (...) {
						LOG_ERROR("Exception occured while fetching whether player is impostor or not.");
					}

					for (auto player : GetAllPlayerControl())
					{
						auto playerInfo = GetPlayerData(player);
						if (!playerInfo) break; //This happens sometimes during loading


						if ((!IsInLobby()) && !State.PanicMode && State.KillImpostors && !playerInfo->fields.IsDead && amImpostor)
							playerInfo->fields.Role->fields.CanBeKilled = true;
						else if (PlayerIsImpostor(playerInfo))
							playerInfo->fields.Role->fields.CanBeKilled = false;
					}
					GameObject* KillButton = app::Component_get_gameObject((Component_1*)__this->fields.KillButton, NULL);
					if (KillButton != NULL && (IsInGame())) {
						if ((!State.PanicMode && State.UnlockKillButton && (IsHost() || !State.SafeMode) && !localData->fields.IsDead) || amImpostor) {
							app::GameObject_SetActive(KillButton, true, nullptr);
							playerRole->fields.CanUseKillButton = true;
						}
						else {
							app::GameObject_SetActive(KillButton, false, nullptr);
							playerRole->fields.CanUseKillButton = false;
						}
					}
					else if (KillButton != NULL && IsInLobby()) {
						app::GameObject_SetActive(KillButton, false, nullptr);
					}
				}
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in HudManager_Update (HudManager)");
	}
	HudManager_Update(__this, method);
}

void dVersionShower_Start(VersionShower* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dVersionShower_Start executed");
	State.versionShower = __this;
	VersionShower_Start(__this, method);
	State.versionShowerDefaultText = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));

	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm = {};
	localtime_s(&tm, &t);  // Safe version of localtime
	std::ostringstream oss;
	oss << std::put_time(&tm, "%m-%d");
	if (oss.str() == "04-01") State.AprilFoolsMode = true;
	else LOG_DEBUG(oss.str());

	/*if (State.PanicMode) return;
	std::string watermarkText = std::format(" ~ <#0f0>Sicko</color><#f00>Menu</color> <#fb0>{}</color> by <#39f>g0aty</color>", State.SickoVersion);
	const auto& versionText = std::format("<font=\"Barlow-Regular SDF\"><size={}%>{}{}{}</color></size></font>",
		State.HideWatermark ? 100 : 60, State.DarkMode ? "<#666>" : "<#fff>", convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr)),
		State.HideWatermark ? "" : watermarkText);
	app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(versionText), nullptr);*/
}

void dPingTracker_Update(PingTracker* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPingTracker_Update executed");
	__this->fields.gamePos.x = 0.f, __this->fields.lobbyPos.x = -0.09f; // Make the PingTracker actually look centered
	bool isFreeplay = ((InnerNetClient*)(*Game::pAmongUsClient))->fields.NetworkMode == NetworkModes__Enum::FreePlay;
	app::PingTracker_Update(__this, method);
	if (!State.PanicMode && State.EnableZoom) __this->fields.aspectPosition->fields.DistanceFromEdge.y += 3 * (State.CameraHeight - 1);
	if (isFreeplay) {
		GameObject_SetActive(Component_get_gameObject((Component_1*)__this, NULL), true, NULL);
		__this->fields.aspectPosition->fields.DistanceFromEdge = __this->fields.gamePos;
		if (!State.PanicMode && State.EnableZoom) __this->fields.aspectPosition->fields.DistanceFromEdge.y += 3 * (State.CameraHeight - 1);
		if (State.PanicMode) return app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(""), nullptr);
	}
	app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, app::TextAlignmentOptions__Enum::Top, nullptr);
	try {
		if (!State.PanicMode || State.TempPanicMode) {
			std::string ping = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
			static int fps = GetFps();
			static int fpsDelay = 0;
			if (fpsDelay <= 0) {
				fps = GetFps();
				fpsDelay = int(0.5 * GetFps()); // 0.5 sec delay
			}
			else fpsDelay--;
			std::string fpsText = "";
			if (State.ShowFps) {
				if (fps <= 20) fpsText = std::format(" ~ FPS: <#f00>{}</color>", fps);
				else if (fps <= 40) fpsText = std::format(" ~ <#ff0>FPS: {}</color>", fps);
				else fpsText = std::format(" ~ <#0f0>FPS: {}</color>", fps);
			}
			int LobbyTime = (int)State.LobbyTimer;
			std::string lobbyTimeDisplay = "";
			/*if (IsHost() && IsInLobby() && State.ShowLobbyTimer) {
				if (LobbyTime <= 60) lobbyTimeDisplay = std::format(" ~ (<#f00>{}:{}{}</color>)", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
				else if (LobbyTime <= 180) lobbyTimeDisplay = std::format(" ~ (<#ff0>{}:{}{}</color>)", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
				else if (LobbyTime >= 0) lobbyTimeDisplay = std::format(" ~ ({}<#0f0>{}:{}{}</color>)", State.JoinedAsHost ? "" : "~", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
				else lobbyTimeDisplay = std::format(" ~ ({}<#0f0>0:00</color>)", State.JoinedAsHost ? "" : "~");
			}*/
			std::string autoKill = State.AutoKill ? " ~ <#f00>Autokill</color>" : "";
			std::string noClip = State.NoClip ? " ~ NoClip" : "";
			std::string freeCam = State.FreeCam ? " ~ Freecam" : "";
			std::string spectating = "";
			if (auto playerToFollow = State.playerToFollow.validate(); playerToFollow.has_value()) {
				app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(playerToFollow.get_PlayerData());
				Color32 playerColor = GetPlayerColor(outfit->fields.ColorId);
				std::string colorCode = std::format("<#{:02x}{:02x}{:02x}{:02x}>",
					playerColor.r, playerColor.g, playerColor.b, playerColor.a);
				auto name = RemoveHtmlTags(convert_from_string(outfit->fields.PlayerName));
				if (name == "") spectating = " ~ Now Spectating";
				else spectating = " ~ Now Spectating: " + colorCode + name + "</color>";
			}
			uint8_t pingSize = 100;
			if (!State.HideWatermark || spectating != "") pingSize = 75;
			if (!State.HideWatermark && spectating != "") pingSize = 50;
			std::string hostText = State.ShowHost && IsInGame() ?
				(IsHost() ? " ~ You are Host" : std::format(" ~ Host: {}", GetHostUsername(true))) : "";
			std::string voteKicksText = (State.ShowVoteKicks && State.VoteKicks > 0) ? std::format(" Vote Kicks: {}", State.VoteKicks) : "";
			std::string watermarkText = State.AprilFoolsMode ? std::format("<size={}%><#0f0>Sicko</color><#f00>Menu</color> <#fb0>{}</color> <#ca08ff>[F{}son Mode]</color> by <#39f>g0aty</color> ~ ",
				IsInGame() ? pingSize : 100, State.SickoVersion, IsChatCensored() || IsStreamerMode() ? "***" : "uck") :
				std::format("<size={}%><#0f0>Sicko</color><#f00>Menu</color> <#fb0>{}</color> by <#39f>g0aty</color> ~ ", IsInGame() ? pingSize : 100, State.SickoVersion);
			std::string pingText = (isFreeplay ? "<size=150%><#0000>0</color></size>\n" : "") +
				std::format("{}{}{}{}{}{}{}{}{}{}{}</color></size>", State.DarkMode ? "<#666>" : "<#fff>",
					State.HideWatermark ? "" : watermarkText, ping, lobbyTimeDisplay, fpsText, hostText, voteKicksText, autoKill, noClip, freeCam, spectating);
			app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, app::TextAlignmentOptions__Enum::Top, nullptr);
			app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(pingText), nullptr);
		}
		else {
			std::string ping = (isFreeplay ? "<size=150%><#0000>0</color></size>\n" : "") +
				convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
			app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(ping), nullptr);
		}
		//"<#0000>00 00</color>" has been added to center the ping text
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PingTracker_Update (HudManager)");
	}
}

bool dLogicGameFlowNormal_IsGameOverDueToDeath(LogicGameFlowNormal* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLogicGameFlowNormal_IsGameOverDueToDeath executed");
	return false; //fix black screen when you set fake role
}
bool dLogicGameFlowHnS_IsGameOverDueToDeath(LogicGameFlowHnS* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLogicGameFlowHnS_IsGameOverDueToDeath executed");
	return false; //fix black screen when you set fake role
}

void dModManager_LateUpdate(ModManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dModManager_LateUpdate executed");
	ModManager_LateUpdate(__this, method);
}

void dEndGameNavigation_ShowDefaultNavigation(EndGameNavigation* __this, MethodInfo* method) {
	EndGameNavigation_ShowDefaultNavigation(__this, method);
	if (!State.PanicMode && State.AutoRejoin) EndGameNavigation_CoJoinGame(__this, NULL);
}