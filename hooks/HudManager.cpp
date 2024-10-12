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
	VersionShower_Start(__this, method);
	const auto& versionText = !State.PanicMode && !State.HideWatermark ? std::format("<size=75%>{}{} ~ <#0f0>Sicko</color><#f00>Menu</color> <#fb0>{}</color> by <#39f>goaty</color></color></size>",
		State.DarkMode ? "<#666>" : "<#fff>", convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr)), State.SickoVersion) :
		convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
	app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(versionText), nullptr);
}

void dPingTracker_Update(PingTracker* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPingTracker_Update executed");
	app::PingTracker_Update(__this, method);
	app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, app::TextAlignmentOptions__Enum::Top, nullptr);
	//center the ping text when panic is enabled
	try {
		if (!IsStreamerMode() && !State.PanicMode) {
			std::string ping = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
			int fps = GetFps();
			std::string fpsText = "";
			if (State.ShowFps) {
				if (fps <= 20) fpsText = std::format(" ~ FPS: <#f00>{}</color>", fps);
				else if (fps <= 40) fpsText = std::format(" ~ <#ff0>FPS: {}</color>", fps);
				else fpsText = std::format(" ~ <#0f0>FPS: {}</color>", fps);
			}
			std::string autoKill = State.AutoKill ? " ~ <#f00>Autokill</color>" : "";
			std::string noClip = State.NoClip ? " ~ NoClip" : "";
			std::string freeCam = State.FreeCam ? " ~ Freecam" : "";
			std::string spectating = "";
			if (State.playerToFollow.has_value()) {
				app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(GetPlayerData(GetPlayerControlById(State.playerToFollow.get_PlayerId())));
				Color32 playerColor = GetPlayerColor(outfit->fields.ColorId);
				std::string colorCode = std::format("<#{:02x}{:02x}{:02x}{:02x}>",
					playerColor.r, playerColor.g, playerColor.b, playerColor.a);
				spectating = " ~ Now Spectating: " + colorCode + RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(GetPlayerControlById(State.playerToFollow.get_PlayerId())), nullptr))) + "</color>";
			}
			else spectating = "";
			std::string hostText = State.ShowHost && IsInGame() ?
				(IsHost() ? " ~ You are Host" : std::format(" ~ Host: {}", GetHostUsername(true))) : "";
			std::string voteKicksText = (State.ShowVoteKicks && State.VoteKicks > 0) ? std::format(" Vote Kicks: {}", State.VoteKicks) : "";
			std::string watermarkText = std::format("<size={}%><#0f0>Sicko</color><#f00>Menu</color> <#fb0>{}</color> by <#39f>goaty</color> ~ ", spectating == "" ? 100 : 50, State.SickoVersion);
			std::string pingText = std::format("{}{}{}{}{}{}{}{}{}</color></size>", State.DarkMode ? "<#666>" : "<#fff>",
				State.HideWatermark ? "" : watermarkText, ping, fpsText, hostText, voteKicksText, autoKill, noClip, freeCam, spectating);
			app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, app::TextAlignmentOptions__Enum::Top, nullptr);
			app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(pingText), nullptr);
		}
		else {
			std::string ping = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
			app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(ping), nullptr);
		}
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