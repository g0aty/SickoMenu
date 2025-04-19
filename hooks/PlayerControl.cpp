#include "pch-il2cpp.h"
#include "_hooks.h"
#include "game.h"
#include "state.hpp"
#include "esp.hpp"
#include "_rpc.h"
#include "replay.hpp"
#include "profiler.h"
#include <iostream>
#include <optional>
#include "logger.h"

using namespace std::chrono;

struct PlayerActivityInfo {
	Vector2 lastPosition;
	time_point<steady_clock> lastMoveTime;
	bool hasEnteredGame = false;
	bool hasReceivedWarning = false;
	bool hasReceivedExtraTimeInMeeting = false;
};

std::unordered_map<int, PlayerActivityInfo> playerActivityMap;

void dPlayerControl_CompleteTask(PlayerControl* __this, uint32_t idx, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CompleteTask executed");
	try {
		std::optional<TaskTypes__Enum> taskType = std::nullopt;

		auto normalPlayerTasks = GetNormalPlayerTasks(__this);
		for (auto normalPlayerTask : normalPlayerTasks)
			if (normalPlayerTask->fields._._Id_k__BackingField == idx) taskType = normalPlayerTask->fields._.TaskType;

		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<TaskCompletedEvent>(GetEventPlayerControl(__this).value(), taskType, PlayerControl_GetTruePosition(__this, NULL)));
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_CompleteTask (PlayerControl)");
	}
	PlayerControl_CompleteTask(__this, idx, method);
}

static Color32 GetKillCooldownColor(float killTimer) {
	if (killTimer < 2.0) {
		return app::Color32_op_Implicit(Palette__TypeInfo->static_fields->ImpostorRed, NULL);
	}
	else if (killTimer < 5.0) {
		return app::Color32_op_Implicit(Palette__TypeInfo->static_fields->Orange, NULL);
	}
	else {
		return app::Color32_op_Implicit(Palette__TypeInfo->static_fields->White, NULL);
	}
}

static std::string DisplayScore(float f) {
	return std::format("{}", f == (int)f ? (int)f : f);
}

float dPlayerControl_fixedUpdateTimer = 50;
float dPlayerControl_fixedUpdateCount = 0;
void dPlayerControl_FixedUpdate(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_FixedUpdate executed");
	try {
		dPlayerControl_fixedUpdateTimer = round(1.f / Time_get_fixedDeltaTime(nullptr));
		if ((IsInGame() || IsInLobby())) {
			if (!__this || !(*Game::pLocalPlayer)) return;
			auto playerData = GetPlayerData(__this);
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			if (!playerData || !localData) return;

			if (Object_1_IsNull((Object_1*)__this->fields.cosmetics)) return;

			auto nameTextTMP = __this->fields.cosmetics->fields.nameText;

			if (!State.PanicMode && State.ModDetection && __this == *Game::pLocalPlayer && (IsInLobby() || State.BroadcastedMod == 1)) {
				uint8_t rpcCall = (uint8_t)420;
				switch (State.BroadcastedMod) {
				case 1:
					rpcCall = (uint8_t)42069;
					break;
				case 2:
					rpcCall = (uint8_t)250;
					break;
				}
				if (State.rpcCooldown <= 0) {
					//SickoMenu users can detect this rpc
					MessageWriter* writer = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId, rpcCall, (SendOption__Enum)1, NULL);
					MessageWriter_EndMessage(writer, NULL);
					State.rpcCooldown = int(0.5 * GetFps());
				}
				else {
					State.rpcCooldown--;
				}
			}

			if (!State.PanicMode && IsInGame() && State.DisableVents && __this->fields.inVent) {
				if (State.rpcCooldown == 0) {
					//copy rpc code so that we don't spam the rpc queue
					il2cpp::Array<Vent__Array> allVents = (*Game::pShipStatus)->fields._AllVents_k__BackingField;
					for (auto vent : allVents) {
						VentilationSystem_Update(VentilationSystem_Operation__Enum::BootImpostors, vent->fields.Id, NULL);
					}
					State.rpcCooldown = 15;
				}
				else {
					State.rpcCooldown--;
				}
			}

			auto outfit = GetPlayerOutfit(playerData, true);
			std::string playerName = "<Unknown>";
			if (outfit != NULL)
				playerName = convert_from_string(outfit->fields.PlayerName);

			static int nameDelay = 0;
			static uint8_t offset = 0; // This should be unsigned or modulo will give unexpected negative results

			if (!State.PanicMode && State.CustomName) {
				if ((IsHost() || !State.SafeMode) && State.ServerSideCustomName) {
					if (nameDelay <= 0) {
						auto customName = GetCustomName(convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(__this), nullptr)), false, 0, offset);
						if (State.ForceNameForEveryone) customName = GetCustomName(State.hostUserName, true, __this->fields.PlayerId);
						if ((__this == *Game::pLocalPlayer || State.CustomNameForEveryone) &&
							customName != convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(__this), nullptr))) {
							if (IsInGame()) State.rpcQueue.push(new RpcForceName(__this, customName));
							if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceName(__this, customName));
						}
						nameDelay = int(0.2 * GetFps());
						offset++;
					}
					else nameDelay--;
				}
				else {
					if (__this == *Game::pLocalPlayer || State.CustomNameForEveryone) {
						if (nameDelay <= 0) {
							offset++;
							nameDelay = int(0.2 * GetFps());
						}
						else nameDelay--;
						playerName = GetCustomName(playerName, false, 0, offset);
					}
				}
			}

			bool hideName = GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && !(GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames));

			bool shouldSeeName = ((!State.PanicMode && (State.RevealRoles || State.ShowKillCD || State.PlayerColoredDots)) || !hideName) && PlayerControl_get_Visible(__this, NULL);

			if (State.PlayerColoredDots && shouldSeeName && !State.PanicMode)
			{
				auto realOutfit = GetPlayerOutfit(playerData);
				Color32&& nameColor = GetPlayerColor(realOutfit->fields.ColorId);
				std::string dot = std::format("<#{:02x}{:02x}{:02x}{:02x}> ●</color>",
					nameColor.r, nameColor.g, nameColor.b,
					nameColor.a);

				playerName = "<#0000>● </color>" + playerName + dot;
			}

			if (IsInLobby() && State.RevealRoles) {
				Color32&& roleColor = app::Color32_op_Implicit(GetRoleColor(playerData->fields.Role), NULL);
				playerName = std::format("<#{:02x}{:02x}{:02x}{:02x}>{}</color>",
					roleColor.r, roleColor.g, roleColor.b,
					roleColor.a, playerName);
			}

			/*if (IsHost() && State.TournamentMode &&
				std::find(State.tournamentFriendCodes.begin(), State.tournamentFriendCodes.end(), convert_from_string(playerData->fields.FriendCode)) != State.tournamentFriendCodes.end()) {
				auto fc = convert_from_string(playerData->fields.FriendCode);
				float points = State.tournamentPoints[fc], win = State.tournamentWinPoints[fc],
					callout = State.tournamentCalloutPoints[fc], death = State.tournamentEarlyDeathPoints[fc];
				std::string pointsHeader = std::format("Your Points: <#0f0>{} Normal</color>, <#9ef>{} +W, {} +C, {} +D</color>", DisplayScore(points),
					DisplayScore(win), DisplayScore(callout), DisplayScore(death)).c_str();
				playerName = std::format("<size=50%>{}</size>\n{}\n<size=50%><#0000>0</color></size>", pointsHeader, playerName);
			}*/ //removed on request

			if (State.ShowPlayerInfo && IsInLobby() && !State.PanicMode)
			{
				uint32_t playerLevel = playerData->fields.PlayerLevel + 1;
				uint8_t playerId = GetPlayerControlById(playerData->fields.PlayerId)->fields._.OwnerId;
				ClientData* host = InnerNetClient_GetHost((InnerNetClient*)(*Game::pAmongUsClient), NULL);
				ClientData* client = InnerNetClient_GetClientFromCharacter((InnerNetClient*)(*Game::pAmongUsClient), __this, NULL);
				std::string platformId = "Unknown";
				if (client != NULL) {
					auto platform = client->fields.PlatformData->fields.Platform;
					if (client->fields.Character == *Game::pLocalPlayer && State.SpoofPlatform) platform = Platforms__Enum(State.FakePlatform + 1); //fix incorrect platform showing for yourself
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
				}
				std::string localPlayerMod = "<#0f0>Sicko</color><#f00>Menu</color>";
				if (State.ModDetection) {
					switch (State.BroadcastedMod) {
					case 1:
						localPlayerMod = "<#f55>AmongUsMenu</color>";
						break;
					case 2:
						localPlayerMod = "<#f00>KillNetwork</color>";
						break;
					}
				}

				std::string modUsage = (__this == *Game::pLocalPlayer && State.ModDetection) || State.modUsers.find(playerData->fields.PlayerId) != State.modUsers.end() ?
					std::format(" <#fb0>[{} User]</color>",
						__this == *Game::pLocalPlayer ? localPlayerMod : State.modUsers.at(playerData->fields.PlayerId)) : "";
				std::string friendCode = convert_from_string(playerData->fields.FriendCode);
				std::string listed = "";
				if (std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), friendCode) != State.WhitelistFriendCodes.end())
					listed = "<#0ff>[+]</color> ";
				else if (std::find(State.BlacklistFriendCodes.begin(), State.BlacklistFriendCodes.end(), friendCode) != State.BlacklistFriendCodes.end())
					listed = "<#f00>[-]</color> ";
				std::string levelText = std::format("{}<#f00>ID {}</color> <#0f0>Level {}</color> <#b0f>({})</color>{}</color>", listed, playerData->fields.PlayerId, playerLevel, platformId, modUsage);
				if (IsStreamerMode())
					friendCode = "Friend Code Hidden";
				std::string hostFriendCode = convert_from_string(InnerNetClient_GetHost((InnerNetClient*)(*Game::pAmongUsClient), NULL)->fields.FriendCode);
				if (client != NULL && client == host) {
					if (friendCode == "" && !IsStreamerMode())
						playerName = "<size=1.4><#fb0>[HOST]</color> " + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#39f>No Friend Code</color><#0000>0</color>";
					else
						playerName = "<size=1.4><#fb0>[HOST]</color> " + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#39f>" + friendCode + "</color><#0000>0</color>";
				}
				else {
					if (friendCode == "" && !IsStreamerMode())
						playerName = "<size=1.4>" + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#39f>No Friend Code</color><#0000>0</color>";
					else
						playerName = "<size=1.4>" + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#39f>" + friendCode + "</color><#0000>0</color>";
				}
			}

			if (IsInGame() && (State.RevealRoles || (IsHost() && (State.TournamentMode || State.TaskSpeedrun))) && shouldSeeName && !State.PanicMode)
			{
				std::string roleName = GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames);
				int completedTasks = 0;
				int totalTasks = 0;
				auto tasks = GetNormalPlayerTasks(__this);
				for (auto task : tasks)
				{
					if (task == nullptr) continue;
					if (task->fields.taskStep == task->fields.MaxStep) {
						completedTasks++;
						totalTasks++;
					}
					else
						totalTasks++;
				}

				if (totalTasks != 0 && completedTasks == totalTasks) {
					if (IsHost() && State.TournamentMode &&
						std::find(State.tournamentAllTasksCompleted.begin(), State.tournamentAllTasksCompleted.end(), playerData->fields.PlayerId) == State.tournamentAllTasksCompleted.end()) {
						UpdatePoints(playerData, 1);
						LOG_DEBUG(std::format("Added 1 point to {} for completing tasks", ToString(playerData)).c_str());
						State.tournamentAllTasksCompleted.push_back(playerData->fields.PlayerId);
					}
					if (IsHost() && State.TaskSpeedrun && !State.SpeedrunOver) {
						int speedrunTimer = int(State.SpeedrunTimer);
						std::string timerDisplay = std::format("<#fff>{} <#0f0>({}:{}{})</color></color>", playerName, int(speedrunTimer / 60), speedrunTimer % 60 < 10 ? "0" : "", speedrunTimer % 60);
						PlayerControl_RpcSetName(__this, convert_to_string(timerDisplay), NULL);
						for (auto receiver : GetAllPlayerControl()) {
							auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId,
								uint8_t(RpcCalls__Enum::SetRole), SendOption__Enum::None, receiver->fields._.OwnerId, NULL);
							MessageWriter_WriteUShort(writer, uint16_t(RoleTypes__Enum::ImpostorGhost), NULL);
							MessageWriter_WriteBoolean(writer, false, NULL);
							InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
						}
						std::string playerName = convert_from_string(GetPlayerOutfit(playerData)->fields.PlayerName);
						State.SpeedrunOver = true; //prevent duplicate timer
						GameManager_RpcEndGame(GameManager__TypeInfo->static_fields->_Instance_k__BackingField, GameOverReason__Enum::ImpostorsByKill, false, NULL);
					}
				}

				if (State.RevealRoles) {
					Color32&& roleColor = app::Color32_op_Implicit(GetRoleColor(playerData->fields.Role), NULL);
					if (totalTasks == 0 || (PlayerIsImpostor(playerData) && completedTasks == 0)) {
						playerName = "<size=1.4>" + roleName + "\n</size>" + playerName + "\n<size=1.4><#0000>0</color>";
						playerName = std::format("<#{:02x}{:02x}{:02x}{:02x}>{}",
							roleColor.r, roleColor.g, roleColor.b,
							roleColor.a, playerName);
					}
					else {
						playerName = "\n</size>" + playerName + "\n<size=1.4><#0000>0</color>";
						playerName = std::format("<#{:02x}{:02x}{:02x}{:02x}><size=1.4>{} ({:d}/{:d}) {}",
							roleColor.r, roleColor.g, roleColor.b,
							roleColor.a, roleName, completedTasks, totalTasks, playerName);
					}
				}
			}
			if (IsInGame() && State.ShowKillCD
				&& !playerData->fields.IsDead
				&& playerData->fields.Role
				&& playerData->fields.Role->fields.CanUseKillButton
				&& shouldSeeName
				&& !State.PanicMode) {
				if (State.RevealRoles) {
					float killTimer = __this->fields.killTimer;
					Color32&& color = GetKillCooldownColor(killTimer);
					playerName += std::format("<size=1.4><#{:02x}{:02x}{:02x}{:02x}>Kill Cooldown: {:.2f}s<#0000>0",
						color.r, color.g, color.b, color.a,
						killTimer);
				}
				else {
					float killTimer = __this->fields.killTimer;
					Color32&& color = GetKillCooldownColor(killTimer);
					playerName = "<size=1.4><#0000>0\n</color></size>" + playerName;
					playerName += std::format("\n<size=1.4><#{:02x}{:02x}{:02x}{:02x}>Kill Cooldown: {:.2f}s",
						color.r, color.g, color.b, color.a,
						killTimer);
				}
			}

			if (IsInGame() && !shouldSeeName) {
				playerName = "<#0000>" + RemoveHtmlTags(playerName) + "</color>";
			}

			if ((IsHost() || !State.SafeMode) && State.TeleportEveryone && (IsInGame() && !State.InMeeting)
				&& State.ShiftRightClickTP && (ImGui::IsKeyPressed(VK_SHIFT) || ImGui::IsKeyDown(VK_SHIFT))
				&& (ImGui::IsKeyPressed(0x12) || ImGui::IsKeyDown(0x12)) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)
				&& !State.PanicMode) {
				ImVec2 mouse = ImGui::GetMousePos();
				Vector2 target = {
					(mouse.x - DirectX::GetWindowSize().x / 2) + DirectX::GetWindowSize().x / 2,
					((mouse.y - DirectX::GetWindowSize().y / 2) - DirectX::GetWindowSize().y / 2) * -1.0f
				};
				for (auto player : GetAllPlayerControl())
					State.rpcQueue.push(new RpcForceSnapTo(player, ScreenToWorld(target)));
			}

			else if ((IsHost() || !State.SafeMode) && State.TeleportEveryone && IsInLobby() && State.ShiftRightClickTP
				&& (ImGui::IsKeyPressed(VK_SHIFT) || ImGui::IsKeyDown(VK_SHIFT))
				&& (ImGui::IsKeyPressed(0x12) || ImGui::IsKeyDown(0x12)) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)
				&& !State.PanicMode) {
				ImVec2 mouse = ImGui::GetMousePos();
				Vector2 target = {
					(mouse.x - DirectX::GetWindowSize().x / 2) + DirectX::GetWindowSize().x / 2,
					((mouse.y - DirectX::GetWindowSize().y / 2) - DirectX::GetWindowSize().y / 2) * -1.0f
				};
				for (auto player : GetAllPlayerControl())
					State.lobbyRpcQueue.push(new RpcForceSnapTo(player, ScreenToWorld(target)));
			}
			if (!State.PanicMode) {
				if (State.RotateEveryone) {
					static float f = 0.f;
					static float rotateDelay = 0;
					if (!State.SafeMode && State.RotateServerSide) {
						if (rotateDelay <= 0) {
							Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
							float num = (State.RotateRadius * cos(f)) + position.x;
							float num2 = (State.RotateRadius * sin(f)) + position.y;
							Vector2 target = { num, num2 };
							if (IsInGame()) {
								for (auto player : GetAllPlayerControl()) {
									if (player != *Game::pLocalPlayer)
										State.rpcQueue.push(new RpcForceSnapTo(player, target));
								}
							}
							else if (IsInLobby()) {
								for (auto player : GetAllPlayerControl()) {
									if (player != *Game::pLocalPlayer)
										State.lobbyRpcQueue.push(new RpcForceSnapTo(player, target));
								}
							}
							rotateDelay = 25 * float(GetAllPlayerControl().size());
							f += 36 * 0.017453292f;
						}
						else {
							rotateDelay--;
						}
					}
					else {
						Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
						float num = (State.RotateRadius * cos(f)) + position.x;
						float num2 = (State.RotateRadius * sin(f)) + position.y;
						Vector2 target = { num, num2 };
						if (rotateDelay <= 0) {
							for (auto player : GetAllPlayerControl()) {
								if (player == *Game::pLocalPlayer) continue;
								CustomNetworkTransform_SnapTo(player->fields.NetTransform, target, player->fields.NetTransform->fields.lastSequenceId + 1, NULL);
								rotateDelay = 0;//25 * float(GetAllPlayerControl().size());
								f += 360 * 0.017453292f / float(GetAllPlayerControl().size());
							}
						}
						else {
							rotateDelay--;
						}
					}
				}
				if (State.ChatSpam && (IsInGame() || IsInLobby())) {
					static float spamDelay = 15;
					auto player = !State.SafeMode && State.playerToChatAs.has_value() ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
					for (auto p : GetAllPlayerControl()) {
						if (p == player || State.ChatSpamEveryone) {
							if (!State.SafeMode && (State.ChatSpamMode == 0 || State.ChatSpamMode == 2)) {
								auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), player->fields._.NetId,
									uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
								MessageWriter_WriteString(writer, convert_to_string(State.chatMessage), NULL);
								InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
								ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, player, convert_to_string(State.chatMessage), false, NULL);
							}
							else if (State.ChatSpamMode == 1 || (State.ChatSpamMode == 2 && ((IsHost() && IsInGame()) || !State.SafeMode))) {
								PlayerControl_RpcSendChatNote(player, p->fields.PlayerId, (ChatNoteTypes__Enum)1, NULL);
							}
						}
					}
				}

				if (State.CrashChatSpam && (IsInGame() || IsInLobby())) {
					static float spamDelay = 0;
					auto player = !State.SafeMode && State.playerToChatAs.has_value() ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
					for (auto p : GetAllPlayerControl()) {
						if (p == player || State.CrashChatSpam) {
							if (!State.SafeMode && State.CrashChatSpamMode == 1) {
								auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), player->fields._.NetId,
									uint8_t(RpcCalls__Enum::SendChat), SendOption__Enum::None, -1, NULL);
								MessageWriter_WriteString(writer, convert_to_string(State.chatMessage), NULL);
								InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
								ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, player, convert_to_string(State.chatMessage), false, NULL);
							}
							else if (State.CrashChatSpamMode == 1) {
								PlayerControl_RpcSendChatNote(player, p->fields.PlayerId, (ChatNoteTypes__Enum)2, NULL);
							}
						}
					}
				}
			}

			String* playerNameStr = convert_to_string(playerName);
			app::TMP_Text_set_text((app::TMP_Text*)nameTextTMP, playerNameStr, NULL);

			if (IsColorBlindMode()) {
				auto colorBlindText = __this->fields.cosmetics->fields.colorBlindText;
				String* text = TMP_Text_get_text((TMP_Text*)colorBlindText, NULL);
				if (!State.PanicMode && ((State.ShowPlayerInfo && IsInLobby()) || (State.ShowKillCD && IsInGame() &&
					playerData->fields.Role && playerData->fields.Role->fields.CanUseKillButton)))
					text = convert_from_string(text).find("\n\n") == std::string::npos ? convert_to_string("\n\n" + convert_from_string(text)) : text;
				else
					text = convert_from_string(text).find("\n\n") != std::string::npos ? convert_to_string(convert_from_string(text).substr(2)) : text;
				TMP_Text_set_text((app::TMP_Text*)colorBlindText, text, NULL);
			}

			// SeeProtect
			do {
				if (__this->fields.protectedByGuardianId < 0)
					break;
				if (localData->fields.IsDead)
					break;
				GameOptions options;
				if (PlayerIsImpostor(localData)
					&& options.GetBool(app::BoolOptionNames__Enum::ImpostorsCanSeeProtect))
					break;
				bool isPlaying = false;
				for (auto anim : il2cpp::List(__this->fields.currentRoleAnimations))
					if (anim->fields.effectType == RoleEffectAnimation_EffectType__Enum::ProtectLoop) {
						isPlaying = true;
						break;
					}
				if (isPlaying == (State.ShowProtections && !State.PanicMode))
					break;
				if (!State.ShowProtections || State.PanicMode)
					app::PlayerControl_RemoveProtection(__this, nullptr);
				std::pair<int32_t/*ColorId*/, float/*Time*/> pair;
				synchronized(State.protectMutex) {
					pair = State.protectMonitor[__this->fields.PlayerId];
				}
				const float ProtectionDurationSeconds = options.GetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, 1.0F);
				float _Duration = ProtectionDurationSeconds - (app::Time_get_time(nullptr) - pair.second);
				options.SetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, _Duration);
				if (_Duration > 0.f)
					app::PlayerControl_TurnOnProtection(__this, State.ShowProtections, pair.first, __this->fields.protectedByGuardianId, nullptr);
				options.SetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, ProtectionDurationSeconds);
			} while (0);

			/*if ((State.Wallhack || State.IsRevived) && __this == *Game::pLocalPlayer && !State.FreeCam
				&& !State.playerToFollow.has_value() && !State.PanicMode) {
				auto mainCamera = Camera_get_main(NULL);

				Transform* cameraTransform = Component_get_transform((Component_1*)mainCamera, NULL);
				Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);
				Transform_set_position(cameraTransform, { cameraVector3.x, cameraVector3.y, 1000 }, NULL);
			}*/

			if (__this == *Game::pLocalPlayer) {
				if (State.FollowerCam == nullptr) {
					for (auto cam : GetAllCameras()) {
						if (Camera_get_orthographicSize(cam, NULL) == 3.0f) {
							State.FollowerCam = cam;
							break;
						}
					};
				}
				if (State.FollowerCam != nullptr) {
					auto chatState = Game::HudManager.GetInstance()->fields.Chat->fields.state;
					bool chatOpen = chatState == ChatControllerState__Enum::Open || chatState == ChatControllerState__Enum::Opening || chatState == ChatControllerState__Enum::Closing;
					if (State.EnableZoom && !State.InMeeting && !chatOpen && (IsInGame() || IsInLobby()) && !State.PanicMode) //chat button disappears after meeting
						Camera_set_orthographicSize(State.FollowerCam, State.CameraHeight * 3, NULL);
					else
						Camera_set_orthographicSize(State.FollowerCam, 3.0f, NULL);

					Transform* cameraTransform = Component_get_transform((Component_1*)State.FollowerCam, NULL);
					Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);
					if (State.EnableZoom && !State.InMeeting && State.CameraHeight > 3.0f)
						Transform_set_position(cameraTransform, { cameraVector3.x, cameraVector3.y, 100 }, NULL);
				}
			}
			else if (auto role = playerData->fields.Role) {
				// ESP: Calculate Kill Cooldown
				if (role->fields.CanUseKillButton && !playerData->fields.IsDead) {
					if (__this->fields.ForceKillTimerContinue
						|| app::PlayerControl_get_IsKillTimerEnabled(__this, nullptr)) {
						__this->fields.killTimer = (std::max)(__this->fields.killTimer - app::Time_get_fixedDeltaTime(nullptr), 0.f);
					}
				}
			}

			/*bool shouldSeeGhost = (State.ShowGhosts && !State.PanicMode) || localData->fields.IsDead;
			if (playerData->fields.IsDead && __this->fields.cosmetics != NULL) {
				auto nameObject = Component_get_gameObject((Component_1*)__this->fields.cosmetics->fields.nameText, NULL);
				GameObject_SetActive(nameObject, true, NULL);
			}*/

			bool shouldSeePhantom = __this == *Game::pLocalPlayer || PlayerIsImpostor(localData) || localData->fields.IsDead;
			auto roleType = playerData->fields.RoleType;

			if (roleType == RoleTypes__Enum::Phantom && !shouldSeePhantom) {
				auto phantomRole = (PhantomRole*)(playerData->fields.Role);
				bool isFullyVanished = phantomRole->fields.isInvisible;
				if (isFullyVanished && __this->fields.invisibilityAlpha < 0.5f && State.ShowPhantoms) {
					PlayerControl_SetInvisibility(__this, false, NULL);
					bool wasDead = false;
					auto local = GetPlayerData(*Game::pLocalPlayer);
					if (__this != NULL && local != NULL && !local->fields.IsDead) {
						local->fields.IsDead = true;
						wasDead = true;
					}
					PlayerControl_SetInvisibility(__this, true, NULL);
					if (wasDead) {
						local->fields.IsDead = false;
					}
				}
				if (isFullyVanished && __this->fields.invisibilityAlpha == 0.5f && !State.ShowPhantoms) {
					PlayerControl_SetInvisibility(__this, true, NULL);
				}
			}

			if (!State.FreeCam && __this == *Game::pLocalPlayer && State.prevCamPos.x != NULL) {
				auto mainCamera = Camera_get_main(NULL);

				Transform* cameraTransform = Component_get_transform((Component_1*)mainCamera, NULL);
				Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);
				Transform_set_position(cameraTransform, State.prevCamPos, NULL);

				State.camPos = { NULL, NULL, NULL };
				State.prevCamPos = { NULL, NULL, NULL };
			}

			if (State.FreeCam && __this == *Game::pLocalPlayer && !State.PanicMode) {
				auto mainCamera = Camera_get_main(NULL);

				Transform* cameraTransform = Component_get_transform((Component_1*)mainCamera, NULL);
				Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);

				if (State.camPos.x == NULL) {
					State.camPos = cameraVector3;
				}
				if (State.prevCamPos.x == NULL) {
					State.prevCamPos = cameraVector3;
				}

				BYTE arr[256];
				if (GetKeyboardState(arr) && !State.ChatFocused)
				{
					float xOffset = 0, yOffset = 0;
					if ((arr[0x57] & 0x80) != 0) {
						yOffset = 1;
					}
					if ((arr[0x41] & 0x80) != 0) {
						xOffset = -1;
					}
					if ((arr[0x53] & 0x80) != 0) {
						yOffset = -1;
					}
					if ((arr[0x44] & 0x80) != 0)
					{
						xOffset = 1;
					}
					float magnitude = (xOffset == 0 && yOffset == 0) ? 1 : sqrt(xOffset * xOffset + yOffset * yOffset);
					//check for zero and prevent you from moving ~1.414 times faster diagonally
					State.camPos.x += float(0.1f * State.FreeCamSpeed * xOffset / magnitude);
					State.camPos.y += float(0.1f * State.FreeCamSpeed * yOffset / magnitude);
				}

				Transform_set_position(cameraTransform, { State.camPos.x, State.camPos.y }, NULL);
			}
		}
		auto playerData = GetPlayerData(__this);
		// We should have this in a scope so that the lock guard only locks the right things
		{
			Vector2 localPos = PlayerControl_GetTruePosition(*Game::pLocalPlayer, nullptr);
			ImVec2 localScreenPosition = WorldToScreen(localPos);

			Vector2 playerPos = PlayerControl_GetTruePosition(__this, nullptr);

			Vector2 prevPlayerPos;
			synchronized(Replay::replayEventMutex) {
				auto& lastPos = State.lastWalkEventPosPerPlayer[__this->fields.PlayerId];
				prevPlayerPos = { lastPos.x, lastPos.y };
				lastPos.x = playerPos.x;
				lastPos.y = playerPos.y;
			}

			// only update our counter if fixedUpdate is executed on local player
			if (__this == *Game::pLocalPlayer)
				dPlayerControl_fixedUpdateCount++;

			if (State.Replay_IsPlaying
				&& !State.Replay_IsLive
				&& dPlayerControl_fixedUpdateCount >= dPlayerControl_fixedUpdateTimer)
			{
				dPlayerControl_fixedUpdateCount = 0;
				State.MatchCurrent += std::chrono::seconds(1);
			}

			if (IsInGame() && !State.InMeeting)
			{
				Profiler::BeginSample("WalkEventCreation");
				float dist = GetDistanceBetweenPoints_Unity(playerPos, prevPlayerPos);
				// NOTE:
				// the localplayer moves even while standing still, by the tiniest amount.
				// hopefully 0.01 will be big enough to filter that out but small enough to catch every real movement
				if (dist > 0.01f)
				{
					synchronized(Replay::replayEventMutex) {
						// NOTE:
						// we do not add walkevents to liveReplayEvents. linedata contains everything we need for live visualization.
						const auto outfit = GetPlayerOutfit(playerData);
						const auto& map = maps[(size_t)State.mapType];
						ImVec2 mapPos_pre = { map.x_offset + (playerPos.x * map.scale), map.y_offset - (playerPos.y * map.scale) };
						if (State.replayWalkPolylineByPlayer.find(__this->fields.PlayerId) == State.replayWalkPolylineByPlayer.end())
						{
							// first-time init
							State.replayWalkPolylineByPlayer[__this->fields.PlayerId] = {};
							State.replayWalkPolylineByPlayer[__this->fields.PlayerId].pendingPoints.reserve(100);
							State.replayWalkPolylineByPlayer[__this->fields.PlayerId].pendingTimeStamps.reserve(100);
						}
						auto& plrLineData = State.replayWalkPolylineByPlayer[__this->fields.PlayerId];
						plrLineData.playerId = __this->fields.PlayerId;
						plrLineData.colorId = outfit ? outfit->fields.ColorId : Game::NoColorId;
						plrLineData.pendingPoints.push_back(mapPos_pre);
						plrLineData.pendingTimeStamps.emplace_back(std::chrono::system_clock::now());
						if (plrLineData.pendingPoints.size() >= 100) {
							DoPolylineSimplification(plrLineData.pendingPoints, plrLineData.pendingTimeStamps,
								plrLineData.simplifiedPoints, plrLineData.simplifiedTimeStamps, 50.f, true);
						}
					}
				}
				Profiler::EndSample("WalkEventCreation");
			}
			auto outfit = GetPlayerOutfit(playerData);
			EspPlayerData espPlayerData;
			espPlayerData.Position = WorldToScreen(playerPos);
			if (outfit != NULL)
			{
				espPlayerData.Color = ImVec4(0.f, 0.f, 0.f, 0.f);
				if (State.ShowEsp_RoleBased) {
					if (State.ShowEsp_Crew && !PlayerIsImpostor(playerData) && (State.ShowEsp_Ghosts || !playerData->fields.IsDead))
						espPlayerData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
					if (State.ShowEsp_Imp && PlayerIsImpostor(playerData) && (State.ShowEsp_Ghosts || !playerData->fields.IsDead))
						espPlayerData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
					if (State.ShowEsp_Ghosts && playerData->fields.IsDead)
						espPlayerData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
				}
				else {
					espPlayerData.Color = AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId));
				}

				espPlayerData.Name = convert_from_string(NetworkedPlayerInfo_get_PlayerName(playerData, nullptr));
			}
			else
			{
				espPlayerData.Color = ImVec4(0.f, 0.f, 0.f, 0.f);
				if (State.ShowEsp_RoleBased) {
					if (State.ShowEsp_Crew && !PlayerIsImpostor(playerData) && (State.ShowEsp_Ghosts || !playerData->fields.IsDead))
						espPlayerData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
					if (State.ShowEsp_Imp && PlayerIsImpostor(playerData) && (State.ShowEsp_Ghosts || !playerData->fields.IsDead))
						espPlayerData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
					if (State.ShowEsp_Ghosts && (playerData->fields.RoleType == RoleTypes__Enum::CrewmateGhost || playerData->fields.RoleType == RoleTypes__Enum::GuardianAngel || playerData->fields.RoleType == RoleTypes__Enum::ImpostorGhost))
						espPlayerData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
				}
				else {
					espPlayerData.Color = ImVec4(0.f, 0.f, 0.f, 0.f);
				}

				espPlayerData.Name = "<Unknown>";
			}
			espPlayerData.OnScreen = IsWithinScreenBounds(playerPos);
			espPlayerData.Distance = Vector2_Distance(localPos, playerPos, nullptr);
			espPlayerData.playerData = PlayerSelection(__this);

			drawing_t& instance = Esp::GetDrawing();
			synchronized(instance.m_DrawingMutex) {
				instance.LocalPosition = localScreenPosition;
				instance.m_Players[playerData->fields.PlayerId] = espPlayerData;
			}
		}
		
		if (!State.KickAFK) return;

		static bool wasInMeeting = false;
		wasInMeeting = State.InMeeting;

		if (wasInMeeting && !State.InMeeting) {
			for (auto& [id, info] : playerActivityMap) {
				info.hasReceivedExtraTimeInMeeting = false;
			}
		}

		for (auto player : GetAllPlayerControl()) {
			if (player == *Game::pLocalPlayer) continue;

			auto* playerData = GetPlayerData(player);
			if (!playerData) continue;

			if (State.Ban_IgnoreWhitelist &&
				std::ranges::find(State.WhitelistFriendCodes, convert_from_string(playerData->fields.FriendCode)) != State.WhitelistFriendCodes.end()) {
				continue;
			}

			int playerId = player->fields._.OwnerId;
			Vector2 currentPosition = GetTrueAdjustedPosition(player);
			auto& info = playerActivityMap[playerId];

			if (!info.hasEnteredGame) {
				info = { currentPosition, steady_clock::now(), true, false, false };
				continue;
			}

			auto now = steady_clock::now();
			if (currentPosition.x != info.lastPosition.x || currentPosition.y != info.lastPosition.y) {
				info.lastPosition = currentPosition;
				info.lastMoveTime = now;
				info.hasReceivedWarning = false;
				continue;
			}

			float elapsed = duration_cast<duration<float>>(now - info.lastMoveTime).count();
			float remainingTime = State.TimerAFK - elapsed;

			if (State.InMeeting) {
				if (remainingTime < State.ExtraTimeThreshold && !info.hasReceivedExtraTimeInMeeting) {
					info.lastMoveTime -= duration_cast<steady_clock::duration>(duration<float>(State.AddExtraTime));
					info.hasReceivedExtraTimeInMeeting = true;
				}
				continue;
			}

			if (State.NotificationsAFK && remainingTime <= State.NotificationTimeWarn && !info.hasReceivedWarning) {
				info.hasReceivedWarning = true;

				std::string nickname = RemoveHtmlTags(convert_from_string(GetPlayerOutfit(playerData)->fields.PlayerName));
				int secondsLeft = static_cast<int>(std::clamp(remainingTime, 0.0f, State.NotificationTimeWarn)) + 1; // <== Time fix on notifications
				std::string warning = std::format("<#FFF>{}</color> <#ff033e>will be kicked in {} seconds due to inactivity!</color>", nickname, secondsLeft);

				auto* notifier = (NotificationPopper*)Game::HudManager.GetInstance()->fields.Notifier;
				if (notifier) {
					auto* spriteBackup = new Sprite(*notifier->fields.playerDisconnectSprite);
					auto colorBackup = notifier->fields.disconnectColor;

					notifier->fields.playerDisconnectSprite = notifier->fields.settingsChangeSprite;
					notifier->fields.disconnectColor = Color(1.0f, 0.0118f, 0.2431f, 1.0f);

					NotificationPopper_AddDisconnectMessage(notifier, convert_to_string(warning), NULL);

					notifier->fields.playerDisconnectSprite = spriteBackup;
					notifier->fields.disconnectColor = colorBackup;
				}
			}

			if (elapsed > State.TimerAFK) {
				InnerNetClient_SendLateRejection((InnerNetClient*)(*Game::pAmongUsClient), playerId, DisconnectReasons__Enum::MatchmakerInactivity, NULL);
				playerActivityMap.erase(playerId);
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_FixedUpdate (PlayerControl)");
	}
	PlayerControl_FixedUpdate(__this, method);
}

void dPlayerControl_RpcSyncSettings(PlayerControl* __this, Byte__Array* optionsByteArray, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_RpcSyncSettings executed");
	try {
		SaveGameOptions();
	}
	catch (...) {
		LOG_ERROR("Exception occurred in RpcSyncSettings (PlayerControl)");
	}
	//PlayerControl_RpcSyncSettings(__this, optionsByteArray, method);
}

bool dPlayerControl_get_CanMove(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_get_CanMove executed");
	try {
		if (__this == NULL || GetPlayerData(__this) == NULL) return false;
		if (!State.PanicMode && __this == *Game::pLocalPlayer) {
			if (((State.AlwaysMove) || (State.MoveInVentAndShapeshift && (((*Game::pLocalPlayer)->fields.inVent) || ((*Game::pLocalPlayer)->fields.shapeshifting)))) && !State.ChatFocused && !((*Game::pLocalPlayer)->fields.petting)) {
				return true;
			}
		}
	}
	catch (...) {
		Log.Debug("Exception occurred in PlayerControl_get_CanMove (PlayerControl)");
	}
	return PlayerControl_get_CanMove(__this, method);
}

void dPlayerControl_OnGameStart(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_OnGameStart executed");
	try {
		State.GameLoaded = true;
		if (IsHost() && State.BattleRoyale) {
			for (auto p : GetAllPlayerControl()) {
				if (p != *Game::pLocalPlayer) RoleManager_SetRole(Game::RoleManager.GetInstance(), p, RoleTypes__Enum::Crewmate, NULL);
			}
		}

		if (IsHost() && State.SpectatorMode && (GetNormalPlayerTasks(*Game::pLocalPlayer).size() != 0 || GetPlayerData(*Game::pLocalPlayer)->fields.RoleType != RoleTypes__Enum::CrewmateGhost)) {
			PlayerControl_RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::ImpostorGhost, false, NULL);
			PlayerControl_RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::CrewmateGhost, false, NULL);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_OnGameStart (PlayerControl)");
	}
	PlayerControl_OnGameStart(__this, method);
}

void dPlayerControl_MurderPlayer(PlayerControl* __this, PlayerControl* target, MurderResultFlags__Enum resultFlags, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_MurderPlayer executed");
	try {
		if (IsInLobby() && target == *Game::pLocalPlayer) return; //for some reason this kicks you from the lobby
		if (static_cast<int32_t>(resultFlags) & static_cast<int32_t>(MurderResultFlags__Enum::FailedError)) {
			app::PlayerControl_MurderPlayer(__this, target, resultFlags, method);
			return;
		}
		auto killer = GetEventPlayerControl(__this);
		auto victim = GetEventPlayerControl(target);
		auto killerData = GetPlayerData(__this);
		auto victimData = GetPlayerData(target);
		if (target == *Game::pLocalPlayer && (victimData->fields.IsDead || State.IsRevived) && (IsInMultiplayerGame() || IsInLobby())) return; //prevent ban exploit
		if (victimData->fields.IsDead && State.LevelFarm) return; //prevent lag caused by multiple bodies
		if (killer && victim) {
			if (!PlayerIsImpostor(killerData) || (PlayerIsImpostor(killerData) && (killerData->fields.IsDead || (victimData->fields.IsDead || PlayerIsImpostor(victimData))))) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<CheatDetectedEvent>(killer.value(), CHEAT_ACTIONS::CHEAT_KILL_IMPOSTOR));
				}
				if (State.SafeMode && State.Enable_SMAC && State.SMAC_CheckMurder)
					SMAC_OnCheatDetected(__this, "Abnormal Murder Player");
			}
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<KillEvent>(killer.value(), victim.value(), PlayerControl_GetTruePosition(__this, NULL), PlayerControl_GetTruePosition(target, NULL)));
				State.replayDeathTimePerPlayer[target->fields.PlayerId] = std::chrono::system_clock::now();
			}
		}

		if (IsHost() && State.TournamentMode) {
			auto killerFc = convert_from_string(GetPlayerData(__this)->fields.FriendCode);
			auto targetData = GetPlayerData(target);

			if (std::find(State.tournamentAliveImpostors.begin(), State.tournamentAliveImpostors.end(), killerFc) != State.tournamentAliveImpostors.end()) {
				if (State.tournamentKillCaps[killerFc] < 3.f) {
					UpdatePoints(killerData, 1.f);
					LOG_DEBUG(std::format("Added 1 point to {} for killing", ToString(killerData)).c_str());
					State.tournamentKillCaps[killerFc] += 1.f;
				}
			}
			if (!State.tournamentFirstMeetingOver) {
				State.tournamentEarlyDeathPoints[convert_from_string(target->fields.FriendCode)] += 1.f;
			}
			/*auto targetFc = convert_from_string(targetData->fields.FriendCode);
			for (auto i : State.tournamentCorrectCallers) {
				if (i.first == targetFc && !GetPlayerDataById(i.second)->fields.IsDead) {
					UpdatePoints(targetData, 1.f); //callout is correct, and the player that called them out dies before the impostor is voted
					break;
				}
			}*/ // Removed on request
		}

		// ESP: Reset Kill Cooldown
		if (__this->fields._.OwnerId != (*Game::pAmongUsClient)->fields._.ClientId) {
			if (!target || target->fields.protectedByGuardianId < 0)
				__this->fields.killTimer = (std::max)(GameOptions().GetKillCooldown(), 0.f);
			else
				__this->fields.killTimer = (std::max)(GameOptions().GetKillCooldown() * 0.5f, 0.f);
			//STREAM_DEBUG("Player " << ToString(__this) << " KillTimer " << __this->fields.killTimer);
		}

		do {
			if (!State.ShowProtections) break;
			if (!target || target->fields.protectedByGuardianId < 0)
				break;
			if (__this->fields._.OwnerId == (*Game::pAmongUsClient)->fields._.ClientId)
				break; // AmKiller
			if (auto localData = GetPlayerData(*Game::pLocalPlayer);
				!localData || !localData->fields.Role
				|| localData->fields.Role->fields.Role == RoleTypes__Enum::GuardianAngel)
				break; // AmAngel
			int prev = target->fields.protectedByGuardianId;
			PlayerControl_ShowFailedMurder(target, nullptr);
			target->fields.protectedByGuardianId = prev;
		} while (false);
		if (__this == *Game::pLocalPlayer && State.confuser && State.confuseOnKill)
			ControlAppearance(true);
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_MurderPlayer (PlayerControl)");
	}
	PlayerControl_MurderPlayer(__this, target, resultFlags, method);

	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	if (!State.PanicMode && State.ReportOnMurder && (!(__this == *Game::pLocalPlayer || PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))) || !State.PreventSelfReport)) {
		PlayerControl_CmdReportDeadBody(*Game::pLocalPlayer, GetPlayerData(target), nullptr);
	}
}

void dPlayerControl_CmdCheckMurder(PlayerControl* __this, PlayerControl* target, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdCheckMurder executed");
	if (!State.PanicMode) {
		if (State.DisableKills || (IsHost() && State.GodMode && target == *Game::pLocalPlayer)) return;

		if (IsHost() || !State.SafeMode) {
			PlayerControl_RpcMurderPlayer(*Game::pLocalPlayer, target, target->fields.protectedByGuardianId < 0 || State.BypassAngelProt, NULL);
			//yay no more complicated checks, enough of me yapping here
		}
		else {
			PlayerControl_CmdCheckMurder(__this, target, NULL);
		}
	}
	else PlayerControl_CmdCheckMurder(__this, target, method);
}

void dPlayerControl_CheckMurder(PlayerControl* __this, PlayerControl* target, MethodInfo* method) {
	// NOTE: This is unused in official Innersloth servers under normal protocol (+0)
	if (!IsHost() || IsInLobby()) {
		if (State.SafeMode && State.Enable_SMAC && State.SMAC_CheckMurder) SMAC_OnCheatDetected(__this, "Abnormal Murder Player");
		return; // Don't send direct murder as non-host or in lobby
	}
	if (IsHost() && State.BattleRoyale && State.DisableHostAnticheat)
		return PlayerControl_RpcMurderPlayer(__this, target, true, NULL); // Force succeed murder in battle royale
	auto pData = GetPlayerData(__this), tData = GetPlayerData(target);
	if (!PlayerIsImpostor(pData)) {
		if (State.SafeMode && State.Enable_SMAC && State.SMAC_CheckMurder) SMAC_OnCheatDetected(__this, "Abnormal Murder Player");
		return; // Don't send direct murder from crewmate
	}
	else {
		if (pData->fields.IsDead || tData->fields.IsDead || PlayerIsImpostor(tData)) {
			if (State.SafeMode && State.Enable_SMAC && State.SMAC_CheckMurder) SMAC_OnCheatDetected(__this, "Abnormal Murder Player");
			return; // Don't send direct murder from dead impostor or to dead target or to an impostor
		}
		if (target->fields.protectedByGuardianId > 0) {
			return PlayerControl_RpcMurderPlayer(__this, target, false, NULL);
			// Show failed murder
		}
	}
	PlayerControl_CheckMurder(__this, target, method);
}

void dPlayerControl_RpcShapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_RpcShapeshift executed");
	if (!State.PanicMode && __this == *Game::pLocalPlayer) PlayerControl_RpcShapeshift(__this, target, (State.PanicMode ? animate : (!State.AnimationlessShapeshift && animate)), method);
	else PlayerControl_RpcShapeshift(__this, target, animate, method);
}

void dPlayerControl_CmdCheckShapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdCheckShapeshift executed");
	if (!State.PanicMode && !State.SafeMode && __this == *Game::pLocalPlayer) PlayerControl_RpcShapeshift(__this, target, (!State.AnimationlessShapeshift && animate), method);
	else PlayerControl_CmdCheckShapeshift(__this, target, (State.PanicMode ? animate : (!State.AnimationlessShapeshift && animate)), method);
}

void dPlayerControl_CmdCheckRevertShapeshift(PlayerControl* __this, bool animate, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdCheckRevertShapeshift executed");
	if (!State.PanicMode && !State.SafeMode && __this == *Game::pLocalPlayer) PlayerControl_RpcShapeshift(__this, __this, (!State.AnimationlessShapeshift && animate), method);
	else PlayerControl_CmdCheckRevertShapeshift(__this, (State.PanicMode ? animate : (!State.AnimationlessShapeshift && animate)), method);
}

/*void dPlayerControl_RpcRevertShapeshift(PlayerControl* __this, bool animate, MethodInfo* method)
{
	try {
		if (__this == *Game::pLocalPlayer && GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer)) == GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer), true))
			return;
		if (!animate)
			PlayerControl_RpcShapeshift(__this, __this, false, method);
		if (animate)
			PlayerControl_RpcShapeshift(__this, __this, !State.AnimationlessShapeshift, method); //cuz game kicks u if u shapeshift in the lobby
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_RpcRevertShapeshift (PlayerControl)");
	}
}*/

void dPlayerControl_StartMeeting(PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_StartMeeting executed");
	State.BlinkPlayersTab = true;
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	try {
		if (!State.PanicMode && IsHost() && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) {
			return;
		}
		else {
			if (State.Enable_SMAC && State.SMAC_CheckReport) {
				if (IsInLobby()) {
					SMAC_OnCheatDetected(__this, "Abnormal Meeting");
				}
				if (IsInGame() && ((target != NULL && !target->fields.IsDead) || GameOptions().GetGameMode() == GameModes__Enum::HideNSeek)) {
					SMAC_OnCheatDetected(__this, "Abnormal Meeting");
				}
			}
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<ReportDeadBodyEvent>(GetEventPlayerControl(__this).value(), GetEventPlayer(target), PlayerControl_GetTruePosition(__this, NULL), GetTargetPosition(target)));
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_StartMeeting (PlayerControl)");
	}
	PlayerControl_StartMeeting(__this, target, method);
}

void dPlayerControl_HandleRpc(PlayerControl* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_HandleRpc executed");
	int32_t pos = reader->fields._position, head = reader->fields.readHead;
	try {
		HandleRpc(__this, callId, reader);
		SMAC_HandleRpc(__this, callId, reader);
		if (!State.PanicMode && IsHost() && State.TournamentMode && !(State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun)) && callId == (uint8_t)RpcCalls__Enum::ReportDeadBody) {
			for (auto p : GetAllPlayerControl()) {
				if (p != *Game::pLocalPlayer) {
					auto playerData = GetPlayerData(p);
					std::string pointsName = std::format("<size=50%><#0f0>Player ID {}</color></size>\n{}\n<size=50%><#0000>0</color></size>", p->fields.PlayerId,
						convert_from_string(GetPlayerOutfit(playerData)->fields.PlayerName));
					auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId,
						uint8_t(RpcCalls__Enum::SetName), SendOption__Enum::None, __this->fields._.OwnerId, NULL);
					MessageWriter_WriteString(writer, convert_to_string(pointsName), NULL);
					InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
				}
			}
		}
		if (Object_1_IsNull((Object_1*)*Game::pShipStatus) && (callId == (uint8_t)RpcCalls__Enum::ReportDeadBody || callId == (uint8_t)RpcCalls__Enum::StartMeeting ||
				callId == (uint8_t)RpcCalls__Enum::CloseDoorsOfType || callId == (uint8_t)RpcCalls__Enum::UpdateSystem))
			return;
		if (IsHost() && ((((!State.PanicMode && State.DisableMeetings) || (State.BattleRoyale || State.TaskSpeedrun)) &&
			(callId == (uint8_t)RpcCalls__Enum::ReportDeadBody || callId == (uint8_t)RpcCalls__Enum::StartMeeting)) ||
			((State.DisableSabotages || (State.BattleRoyale || State.TaskSpeedrun)) &&
				((callId == (uint8_t)RpcCalls__Enum::CloseDoorsOfType && State.mapType != Settings::MapType::Hq) || callId == (uint8_t)RpcCalls__Enum::UpdateSystem))))
			//we cannot prevent murderplayer because the player will force it
			return;
		if (!State.PanicMode && State.DisableKills && callId == (uint8_t)RpcCalls__Enum::CheckMurder) {
			//PlayerControl* target = MessageExtensions_ReadNetObject_1(reader, NULL);
			//PlayerControl_RpcProtectPlayer(*Game::pLocalPlayer, target, GetPlayerOutfit(GetPlayerData(target))->fields.ColorId, NULL);
		}
		int crew = 0, imp = 0;
		for (auto p : GetAllPlayerData()) {
			if (p->fields.IsDead) continue;
			PlayerIsImpostor(p) ? imp++ : crew++;
		}
		bool shouldCheckMeeting = (imp >= crew) || imp == 0;
		if (State.NoGameEnd && shouldCheckMeeting && IsHost() &&
			(callId == (uint8_t)RpcCalls__Enum::ReportDeadBody || callId == (uint8_t)RpcCalls__Enum::StartMeeting))
			return;
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_HandleRpc (PlayerControl)");
	}
	reader->fields._position = pos;
	reader->fields.readHead = head;
	PlayerControl_HandleRpc(__this, callId, reader, NULL);
}

void dRenderer_set_enabled(Renderer* __this, bool value, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dRenderer_set_enabled executed");
	try {//If we're already rendering it, lets skip checking if we should
		if (!State.PanicMode) {
			if ((IsInGame() || IsInLobby()) && !value && State.ShowGhosts)
			{
				Transform* rendererTrans = app::Component_get_transform(reinterpret_cast<app::Component_1*>(__this), NULL);
				if (rendererTrans != NULL)
				{
					Transform* root = app::Transform_GetRoot(rendererTrans, NULL); // docs say GetRoot never returns NULL, so no need to check it
					for (auto player : GetAllPlayerControl())
					{
						auto playerInfo = GetPlayerData(player);
						if (!playerInfo) break; //This happens sometimes during loading

						if (playerInfo->fields.IsDead)
						{
							// TO-DO:
							// figure out if a reference to the Renderer component can be gotten, otherwise just use UnityEngine's GetComponentInChildren<T> function
							// was: player->fields.MyPhysics->fields.rend
							Transform* playerTrans = app::Component_get_transform(reinterpret_cast<app::Component_1*>(player), NULL);
							if (playerTrans == NULL) continue;
							Transform* playerRoot = app::Transform_GetRoot(playerTrans, NULL); // docs say GetRoot never returns NULL, so no need to check it
							if (root == playerRoot)
							{
								value = true;
							}
						}
					}
				}
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in Renderer_set_enabled (PlayerControl)");
	}
	Renderer_set_enabled(__this, value, method);
}

void dGameObject_SetActive(GameObject* __this, bool value, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dGameObject_SetActive executed");
	try {
		if (!State.PanicMode) {
			if ((IsInGame() || IsInLobby()) && !value) { //If we're already rendering it, lets skip checking if we should
				for (auto player : GetAllPlayerControl()) {
					auto playerInfo = GetPlayerData(player);
					if (!playerInfo || !player->fields.cosmetics || !player->fields.visibilityItems) break; //This happens sometimes during loading
					if (((playerInfo->fields.IsDead && State.ShowGhosts) || (!playerInfo->fields.IsDead && player->fields.shouldAppearInvisible && State.ShowPhantoms)) ||
						(!playerInfo->fields.IsDead && State.RevealRoles))
					{
						auto nameObject = Component_get_gameObject((Component_1*)player->fields.cosmetics->fields.nameText, NULL);
						if (nameObject == __this) {
							value = true;
							break;
						}
					}
					else if ((playerInfo->fields.IsDead || (!playerInfo->fields.IsDead && player->fields.shouldAppearInvisible))
						&& !State.RevealRoles) {
						auto nameObject = Component_get_gameObject((Component_1*)player->fields.cosmetics->fields.nameText, NULL);
						if (nameObject == __this) {
							value = false;
							break;
						}
					}
				}
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in GameObject_SetActive (PlayerControl)");
	}
	GameObject_SetActive(__this, value, method);
}

void dPlayerControl_CmdReportDeadBody(PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdReportDeadBody executed");
	try {
		if (!State.PanicMode && IsHost() && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) {
			return;
		}
	}
	catch (...) {
		Log.Debug("Exception occurred in CmdReportDeadBody (PlayerControl)");
	}
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	PlayerControl_CmdReportDeadBody(__this, target, method);
}

void dPlayerControl_RpcStartMeeting(PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_RpcStartMeeting executed");
	try {
		if (!State.PanicMode && IsHost() && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) {
			return;
		}
	}
	catch (...) {
		Log.Debug("Exception occurred in PlayerControl_RpcStartMeeting (PlayerControl)");
	}
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	PlayerControl_RpcStartMeeting(__this, target, method);
}

void dPlayerControl_Shapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_Shapeshift executed");
	try {
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<ShapeShiftEvent>(GetEventPlayerControl(__this).value(), GetEventPlayerControl(target).value()));
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_Shapeshift (PlayerControl)");
	}
	PlayerControl_Shapeshift(__this, target, animate, method);
}

void dPlayerControl_ProtectPlayer(PlayerControl* __this, PlayerControl* target, int32_t colorId, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_ProtectPlayer executed");
	try {
		if (SYNCHRONIZED(Replay::replayEventMutex); target != nullptr) {
			State.liveReplayEvents.emplace_back(std::make_unique<ProtectPlayerEvent>(GetEventPlayerControl(__this).value(), GetEventPlayerControl(target).value()));
		}
		else {
			SMAC_OnCheatDetected(__this, "Overloading");
			return;
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_ProtectPlayer (PlayerControl)");
	}
	PlayerControl_ProtectPlayer(__this, target, colorId, method);
}

void dPlayerControl_TurnOnProtection(PlayerControl* __this, bool visible, int32_t colorId, int32_t guardianPlayerId, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_TurnOnProtection executed");
	try {
		app::PlayerControl_TurnOnProtection(__this, visible || State.ShowProtections, colorId, guardianPlayerId, method);
		std::pair pair{ colorId, app::Time_get_time(nullptr) };
		synchronized(State.protectMutex) {
			State.protectMonitor[__this->fields.PlayerId] = pair;
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in PlayerControl_TurnOnProtection (PlayerControl)");
		app::PlayerControl_TurnOnProtection(__this, visible, colorId, guardianPlayerId, method);
	}
}

void dPlayerControl_RemoveProtection(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_RemoveProtection executed");
	try {
		State.protectMonitor.erase(__this->fields.PlayerId);
	}
	catch (...) {
		Log.Debug("Exception occurred in PlayerControl_RemoveProtection (PlayerControl)");
	}
	PlayerControl_RemoveProtection(__this, method);
}

void dKillButton_SetTarget(KillButton* __this, PlayerControl* target, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dKillButton_SetTarget executed");
	if (!State.PanicMode && IsInGame()) {
		try {
			auto result = target;
			bool amImpostor = PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer));
			if (State.UnlockKillButton && (IsHost() || !State.SafeMode) && (!amImpostor)) {
				if (!State.PanicMode && result == nullptr) {
					PlayerControl* new_result = nullptr;
					float defaultKillDist = 2.5f;
					auto killDistSetting = GameOptions().GetInt(Int32OptionNames__Enum::KillDistance);
					switch (killDistSetting) {
					case 0:
						defaultKillDist = 1.f;
						break; //short
					case 1:
						defaultKillDist = 1.8f;
						break; //medium
					case 2:
						defaultKillDist = 2.5f;
						break; //long
					}
					float max_dist = State.InfiniteKillRange ? FLT_MAX : defaultKillDist; //medium kill distance is 1.8
					auto localPos = GetTrueAdjustedPosition(*Game::pLocalPlayer);
					for (auto p : GetAllPlayerControl()) {
						if (p == *Game::pLocalPlayer || p == NULL) continue; //we don't want to kill ourselves
						auto pData = GetPlayerData(p);
						if (PlayerIsImpostor(pData) && !(State.KillImpostors || (IsHost() && State.BattleRoyale))) continue; //neither impostors
						if (pData->fields.IsDead) continue; //nor ghosts
						float currentDist = GetDistanceBetweenPoints_Unity(GetTrueAdjustedPosition(p), localPos);
						if (currentDist < max_dist) {
							new_result = p;
							max_dist = currentDist;
						}
					}
					result = new_result;
				}

				if (!State.PanicMode && result != nullptr && (State.AutoKill && (IsInLobby() ? State.KillInLobbies : true)) && (State.AlwaysMove || PlayerControl_get_CanMove(*Game::pLocalPlayer, NULL)) && (*Game::pLocalPlayer)->fields.killTimer <= 0.f) {
					std::queue<RPCInterface*>* queue = nullptr;
					if (IsInGame())
						queue = &State.rpcQueue;
					else if (IsInLobby())
						queue = &State.lobbyRpcQueue;

					if (IsHost() || !State.SafeMode) queue->push(new RpcMurderPlayer(*Game::pLocalPlayer, result));
					else queue->push(new CmdCheckMurder(PlayerSelection(result)));
				}
				KillButton_SetTarget(__this, result, NULL);
				return;
			}
			if (IsInLobby()) result = NULL;
			else if (amImpostor) KillButton_SetTarget(__this, result, NULL);
			else KillButton_SetTarget(__this, NULL, NULL);
		}
		catch (...) {
			LOG_ERROR("Exception occurred in KillButton_SetTarget (PlayerControl)");
		}
	}
	KillButton_SetTarget(__this, target, method);
}

PlayerControl* dImpostorRole_FindClosestTarget(ImpostorRole* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dImpostorRole_FindClosestTarget executed");
	if (IsInLobby()) return nullptr;
	auto result = ImpostorRole_FindClosestTarget(__this, method);
	if (!State.PanicMode && result == nullptr && (State.InfiniteKillRange || (State.KillInVanish && IsHost() || !State.SafeMode))) {
		PlayerControl* new_result = nullptr;
		float defaultKillDist = 2.5f;
		auto killDistSetting = GameOptions().GetInt(Int32OptionNames__Enum::KillDistance);
		switch (killDistSetting) {
		case 0:
			defaultKillDist = 1.f;
			break; //short
		case 1:
			defaultKillDist = 1.8f;
			break; //medium
		case 2:
			defaultKillDist = 2.5f;
			break; //long
		}
		float max_dist = State.InfiniteKillRange ? FLT_MAX : defaultKillDist; //medium kill distance is 1.8
		auto localPos = GetTrueAdjustedPosition(*Game::pLocalPlayer);
		for (auto p : GetAllPlayerControl()) {
			if (p == *Game::pLocalPlayer || p == NULL) continue; //we don't want to kill ourselves
			auto pData = GetPlayerData(p);
			if (PlayerIsImpostor(pData) && !(State.KillImpostors || (IsHost() && State.BattleRoyale))) continue; //neither impostors
			if (pData->fields.IsDead) continue; //nor ghosts
			if (pData == NULL) continue; //nor null
			float currentDist = GetDistanceBetweenPoints_Unity(GetTrueAdjustedPosition(p), localPos);
			if (currentDist < max_dist) {
				new_result = p;
				max_dist = currentDist;
			}
		}
		result = new_result;
	}

	if (!State.PanicMode && result != nullptr && (State.AutoKill && (IsInLobby() ? State.KillInLobbies : true)) && (State.AlwaysMove || PlayerControl_get_CanMove(*Game::pLocalPlayer, NULL)) && (*Game::pLocalPlayer)->fields.killTimer <= 0.f) {
		std::queue<RPCInterface*>* queue = nullptr;
		if (IsInGame())
			queue = &State.rpcQueue;
		else if (IsInLobby())
			queue = &State.lobbyRpcQueue;

		if (IsHost() || !State.SafeMode) queue->push(new RpcMurderPlayer(*Game::pLocalPlayer, result));
		else queue->push(new CmdCheckMurder(PlayerSelection(result)));
	}
	return result;
}

float dConsole_1_CanUse(Console_1* __this, NetworkedPlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dConsole_1_CanUse executed");
	try {
		if (!State.PanicMode) {
			std::vector<int> sabotageTaskIds = { 0, 1, 2 }; //don't prevent impostor from fixing sabotages
			if (State.DoTasksAsImpostor || !PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer)))
				__this->fields.AllowImpostor = true;
			else if (std::find(sabotageTaskIds.begin(), sabotageTaskIds.end(), __this->fields.ConsoleId) == sabotageTaskIds.end())
				__this->fields.AllowImpostor = false;
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in Console_1_CanUse (PlayerControl)");
	}
	return Console_1_CanUse(__this, pc, canUse, couldUse, method);
}

void dPlayerControl_CoSetRole(PlayerControl* __this, RoleTypes__Enum role, bool canOverride, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CoSetRole executed");
	if (__this == *Game::pLocalPlayer) State.RealRole = role;
	if (!IsInMultiplayerGame() || __this != *Game::pLocalPlayer || !State.AutoFakeRole) {
		PlayerControl_CoSetRole(__this, role, canOverride, method);
		return;
	}
	bool hasAlreadySetRole = role == RoleTypes__Enum::GuardianAngel || role == RoleTypes__Enum::CrewmateGhost || role == RoleTypes__Enum::ImpostorGhost;
	bool roleAllowed = false;
	switch (State.FakeRole) {
	case (int)RoleTypes__Enum::Crewmate:
	case (int)RoleTypes__Enum::Engineer:
	case (int)RoleTypes__Enum::Scientist:
	case (int)RoleTypes__Enum::Noisemaker:
	case (int)RoleTypes__Enum::Tracker:
	case (int)RoleTypes__Enum::CrewmateGhost:
	case (int)RoleTypes__Enum::ImpostorGhost:
	case (int)RoleTypes__Enum::GuardianAngel:
		roleAllowed = true;
		break;
	case (int)RoleTypes__Enum::Impostor:
		if ((!IsHost() && State.SafeMode) || State.RealRole != RoleTypes__Enum::Impostor || State.RealRole != RoleTypes__Enum::Shapeshifter || State.RealRole != RoleTypes__Enum::Phantom) {
			roleAllowed = false;
			break;
		}
		roleAllowed = true;
		break;
	case (int)RoleTypes__Enum::Shapeshifter:
		if (State.RealRole != RoleTypes__Enum::Shapeshifter) {
			roleAllowed = false;
			break;
		}
		roleAllowed = true;
		break;
	case (int)RoleTypes__Enum::Phantom:
		if (State.RealRole != RoleTypes__Enum::Phantom) {
			roleAllowed = false;
			break;
		}
		roleAllowed = true;
		break;
	default:
		roleAllowed = false;
		break;
	}
	if (State.AutoFakeRole && !hasAlreadySetRole && roleAllowed) {
		role = (RoleTypes__Enum)State.FakeRole;
	}
	PlayerControl_CoSetRole(__this, role, canOverride, method);
}

void dNetworkedPlayerInfo_Serialize(NetworkedPlayerInfo* __this, MessageWriter* writer, bool initialState, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dNetworkedPlayerInfo_Serialize executed");
	if (GetPlayerData(*Game::pLocalPlayer) == __this) {
		if (State.SpoofFriendCode) __this->fields.FriendCode = convert_to_string(State.FakeFriendCode);
	}
	NetworkedPlayerInfo_Serialize(__this, writer, initialState, NULL);
}

void dNetworkedPlayerInfo_Deserialize(NetworkedPlayerInfo* __this, MessageReader* reader, bool initialState, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dNetworkedPlayerInfo_Deserialize executed");
	std::string friendCode = convert_from_string(__this->fields.FriendCode);
	uint8_t id = __this->fields.PlayerId;
	if (std::find(State.BlacklistFriendCodes.begin(), State.BlacklistFriendCodes.end(), friendCode) != State.BlacklistFriendCodes.end()) {
		if (State.Enable_SMAC) {
			std::string name = RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(__this, NULL)));
			switch (IsHost() ? State.SMAC_HostPunishment : State.SMAC_Punishment) {
			case 0:
				break;
			case 1: {
				std::string message = std::format("Blacklisted player {} has joined the game", name);
				ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, GetPlayerControlById(id), convert_to_string(message), false, NULL);
				break;
			}
			case 2:
			{
				String* newName = convert_to_string(name + " has been kicked by <#0f0>Sicko</color><#f00>Menu</color> <#9ef>Anticheat</color>! Reason: Blacklisted<size=0>");
				if (IsHost()) {
					PlayerControl_CmdCheckName(GetPlayerControlById(id), newName, NULL);
					InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), GetPlayerControlById(id)->fields._.OwnerId, false, NULL);
				}
				break;
			}
			case 3:
			{
				String* newName = convert_to_string(name + " has been banned by <#0f0>Sicko</color><#f00>Menu</color> <#9ef>Anticheat</color>! Reason: Blacklisted<size=0>");
				if (IsHost()) {
					PlayerControl_CmdCheckName(GetPlayerControlById(id), newName, NULL);
					InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), GetPlayerControlById(id)->fields._.OwnerId, true, NULL);
				}
				break;
			}
			}
		}
	}
	NetworkedPlayerInfo_Deserialize(__this, reader, initialState, NULL);
}

void dPlayerControl_CmdCheckVanish(PlayerControl* __this, float maxDuration, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdCheckVanish executed");
	if (!State.PanicMode && State.AnimationlessShapeshift) maxDuration = 0.f;
	PlayerControl_CmdCheckVanish(__this, maxDuration, method);
}

void dPlayerControl_CmdCheckAppear(PlayerControl* __this, bool shouldAnimate, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdCheckAppear executed");
	if (!State.PanicMode && State.AnimationlessShapeshift) shouldAnimate = false;
	PlayerControl_CmdCheckAppear(__this, shouldAnimate, method);
}

/*void dPlayerControl_SetInvisibility(PlayerControl* __this, bool isActive, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_SetInvisibility executed");
	if (!State.PanicMode && State.ShowPhantoms) {
		bool wasDead = false;
		auto local = GetPlayerData(*Game::pLocalPlayer);
		if (__this != NULL && local != NULL && !local->fields.IsDead) {
			local->fields.IsDead = true;
			wasDead = true;
		}
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<PhantomEvent>(GetEventPlayerControl(__this).value(),
				isActive ? PHANTOM_ACTIONS::PHANTOM_VANISH : PHANTOM_ACTIONS::PHANTOM_APPEAR));
		}
		PlayerControl_SetInvisibility(__this, isActive, method);
		if (wasDead) {
			local->fields.IsDead = false;
		}
		return;
	}

	PlayerControl_SetInvisibility(__this, isActive, method);
}*/

void dPlayerControl_SetRoleInvisibility(PlayerControl* __this, bool isActive, bool shouldAnimate, bool playFullAnimation, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_SetRoleInvisibility executed");
	/*if (!State.PanicMode && State.ShowPhantoms) {
		bool wasDead = false;
		auto local = GetPlayerData(*Game::pLocalPlayer);
		if (__this != NULL && local != NULL && !local->fields.IsDead) {
			local->fields.IsDead = true;
			wasDead = true;
		}
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<PhantomEvent>(GetEventPlayerControl(__this).value(),
				isActive ? PHANTOM_ACTIONS::PHANTOM_VANISH : PHANTOM_ACTIONS::PHANTOM_APPEAR));
		}
		PlayerControl_SetInvisibility(__this, isActive, method);
		if (wasDead) {
			local->fields.IsDead = false;
		}
		return;
	}*/
	synchronized(Replay::replayEventMutex) {
		State.liveReplayEvents.emplace_back(std::make_unique<PhantomEvent>(GetEventPlayerControl(__this).value(),
			isActive ? PHANTOM_ACTIONS::PHANTOM_VANISH : PHANTOM_ACTIONS::PHANTOM_APPEAR));
	}
	auto pData = GetPlayerData(__this);
	if (pData != NULL && pData->fields.RoleType == RoleTypes__Enum::Phantom && isActive)
		State.vanishedPlayers.push_back(__this->fields.PlayerId);
	else {
		auto it = std::find(State.vanishedPlayers.begin(), State.vanishedPlayers.end(), __this->fields.PlayerId);
		if (it != State.vanishedPlayers.end())
			State.vanishedPlayers.erase(it);
	}

	PlayerControl_SetRoleInvisibility(__this, isActive, shouldAnimate, playFullAnimation, method);
}

void dPlayerControl_CmdCheckProtect(PlayerControl* __this, PlayerControl* target, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_CmdCheckProtect executed");
	if (!State.PanicMode) {
		if (IsHost() && IsInGame())
			PlayerControl_RpcProtectPlayer(__this, target, GetPlayerOutfit(GetPlayerData(__this))->fields.ColorId, NULL);
		else
			PlayerControl_CmdCheckProtect(__this, target, method);
	}
	else PlayerControl_CmdCheckProtect(__this, target, method);
}

void dPlayerControl_SetLevel(PlayerControl* __this, uint32_t level, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_SetLevel executed");

	uint32_t playerLevel = level + 1;

	if (State.SMAC_CheckLevel && (IsInGame() ||
		((State.SMAC_HighLevel != 0 && playerLevel >= (uint32_t)State.SMAC_HighLevel) || (State.SMAC_LowLevel != 0 && playerLevel <= (uint32_t)State.SMAC_LowLevel)))) {
		SMAC_OnCheatDetected(__this, "Abnormal Level");
	}

	if (__this != *Game::pLocalPlayer && level > 2147483647) level = 2147483647; //anti level 0 exploit

	PlayerControl_SetLevel(__this, level, method);
}

PlayerBodyTypes__Enum dHideAndSeekManager_GetBodyType(HideAndSeekManager* __this, PlayerControl* player, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dHideAndSeekManager_GetBodyType executed");
	if (!State.PanicMode && State.ChangeBodyType) {
		bool isImpostor = PlayerIsImpostor(GetPlayerData(player));
		switch (State.BodyType) {
		case 0:
			return isImpostor ? PlayerBodyTypes__Enum::Seeker : PlayerBodyTypes__Enum::Normal;
		case 1:
			return isImpostor ? PlayerBodyTypes__Enum::Seeker : PlayerBodyTypes__Enum::Horse;
		case 2:
			return isImpostor ? PlayerBodyTypes__Enum::LongSeeker : PlayerBodyTypes__Enum::Long;
		default:
			return isImpostor ? PlayerBodyTypes__Enum::Seeker : PlayerBodyTypes__Enum::Normal;
		}
	}
	return HideAndSeekManager_GetBodyType(__this, player, method);
}

PlayerBodyTypes__Enum dNormalGameManager_GetBodyType(NormalGameManager* __this, PlayerControl* player, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dNormalGameManager_GetBodyType executed");
	if (!State.PanicMode && State.ChangeBodyType) {
		switch (State.BodyType) {
		case 0:
			return PlayerBodyTypes__Enum::Normal;
		case 1:
			return PlayerBodyTypes__Enum::Horse;
		case 2:
			return PlayerBodyTypes__Enum::Long;
		}
	}
	return NormalGameManager_GetBodyType(__this, player, method);
}

float dPlayerControl_get_CalculatedAlpha(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_get_CalculatedAlpha executed");
	return PlayerControl_get_CalculatedAlpha(__this, method);
}

bool dPlayerControl_get_Visible(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_get_Visible executed");
	return PlayerControl_get_Visible(__this, method);
}

bool dPlayerControl_IsFlashlightEnabled(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_IsFlashlightEnabled executed");
	if (!State.PanicMode && State.MaxVision) return false;
	return PlayerControl_IsFlashlightEnabled(__this, method);
}

void dPlayerControl_OnDestroy(PlayerControl* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerControl_OnDestroy executed");
	State.BlinkPlayersTab = true;
	PlayerControl_OnDestroy(__this, method);
}

void dBanMenu_Select(BanMenu* __this, int32_t clientId, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dBanMenu_Select executed");
	BanMenu_Select(__this, clientId, method);
}

void dPlayerControl_RpcPlayAnimation(PlayerControl* __this, uint8_t animType, MethodInfo* method) {
	if (State.BypassVisualTasks) {
		PlayerControl_PlayAnimation(__this, animType, NULL);
		auto writer = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId, uint8_t(RpcCalls__Enum::PlayAnimation), SendOption__Enum::None, NULL);
		MessageWriter_WriteByte(writer, animType, NULL);
		MessageWriter_EndMessage(writer, NULL);
		return;
	}
	PlayerControl_RpcPlayAnimation(__this, animType, NULL);
}

void dPlayerControl_RpcSetScanner(PlayerControl* __this, bool value, MethodInfo* method) {
	if (State.BypassVisualTasks) {
		PlayerControl_SetScanner(__this, value, __this->fields.scannerCount + 1);
		auto writer = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId, uint8_t(RpcCalls__Enum::SetScanner), SendOption__Enum::None, NULL);
		MessageWriter_WriteBoolean(writer, value, NULL);
		MessageWriter_WriteByte(writer, __this->fields.scannerCount + 1, NULL);
		MessageWriter_EndMessage(writer, NULL);
		return;
	}
	PlayerControl_RpcSetScanner(__this, value, NULL);
}
