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

void dPlayerControl_CompleteTask(PlayerControl* __this, uint32_t idx, MethodInfo* method) {
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
		LOG_DEBUG("Exception occurred in PlayerControl_CompleteTask (PlayerControl)");
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

float dPlayerControl_fixedUpdateTimer = 50;
float dPlayerControl_fixedUpdateCount = 0;
void dPlayerControl_FixedUpdate(PlayerControl* __this, MethodInfo* method) {
	try {
		dPlayerControl_fixedUpdateTimer = round(1.f / Time_get_fixedDeltaTime(nullptr));
		if ((IsInGame() || IsInLobby())) {
			auto playerData = GetPlayerData(__this);
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			assert(Object_1_IsNotNull((Object_1*)__this->fields.cosmetics));
			auto nameTextTMP = __this->fields.cosmetics->fields.nameText;

			if (State.SickoDetection && __this == *Game::pLocalPlayer && !IsInGame()) { //don't spam the rpc when you're not in the game, the menu only needs one 420 call to detect usage
				if (State.rpcCooldown == 0) {
					//SickoMenu users can detect this rpc
					MessageWriter* writer = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), __this->fields._.NetId, (uint8_t)420, (SendOption__Enum)1, NULL);
					MessageWriter_EndMessage(writer, NULL);
					State.rpcCooldown = 15;
				}
				else {
					State.rpcCooldown--;
				}
			}

			if (IsInGame() && State.DisableVents && __this->fields.inVent) {
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

			if (!playerData || !localData)
				return;

			app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(playerData, true);
			std::string playerName = "<Unknown>";
			if (outfit != NULL)
				playerName = convert_from_string(GameData_PlayerOutfit_get_PlayerName(outfit, nullptr));

			static int nameDelay = 0;

			if (__this == *Game::pLocalPlayer && !State.userName.empty() &&
				!((State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames) == false) && __this->fields.inVent) && !State.PanicMode) {
				if (State.CustomName) {
					std::string opener = "";
					std::string closer = "";
					if (State.ColoredName && !State.RgbName && !State.ServerSideCustomName) {
						playerName = GetGradientUsername(RemoveHtmlTags(playerName), true);
					}
					if (State.RgbName) {
						const auto calculate = [](float value) {return std::sin(value) * .5f + .5f; };
						auto color_r = calculate(State.RgbNameColor + 0.f);
						auto color_g = calculate(State.RgbNameColor + 4.f);
						auto color_b = calculate(State.RgbNameColor + 2.f);
						std::string rgbCode = std::format("<#{:02x}{:02x}{:02x}{:02x}>", int(color_r * 255), int(color_g * 255), int(color_b * 255), int((State.NameColor1.w + State.NameColor2.w) * 255 / 2));
						opener += rgbCode;
						closer += "</color>";
					}
					if (State.ResizeName) {
						opener += std::format("<size={}%>", State.NameSize * 100);
						closer += "</size>";
					}
					if (State.ItalicName) {
						opener += "<i>";
						closer += "</i>";
					}
					if (State.UnderlineName && (!State.ColoredName || State.RgbName)) {
						opener += "<u>";
						closer += "</u>";
					}
					if (State.StrikethroughName && (!State.ColoredName || State.RgbName)) {
						opener += "<s>";
						closer += "</s>";
					}

					playerName = opener + playerName + closer;

					if (nameDelay <= 0 && State.ServerSideCustomName &&
						convert_from_string(GameData_PlayerOutfit_get_PlayerName(GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer)), nullptr)) != ((State.ColoredName && !State.RgbName) ? GetGradientUsername(RemoveHtmlTags(playerName), true) : playerName)) {
						std::string username = (State.ColoredName && !State.RgbName) ? GetGradientUsername(RemoveHtmlTags(State.userName), true) : State.userName;
						if (IsInGame()) {
							State.rpcQueue.push(new RpcSetName(opener + username + closer));
							nameDelay = 5;
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcSetName(opener + username + closer));
							nameDelay = 5;
						}
						nameDelay = 0;
					}
					else nameDelay--;
				}
			}

			if (State.PlayerColoredDots && !((State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames) == false) && __this->fields.inVent) && !State.PanicMode)
			{
				app::GameData_PlayerOutfit* realOutfit = GetPlayerOutfit(playerData);
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

			if (State.ShowPlayerInfo && IsInLobby() && !State.PanicMode)
			{
				uint32_t playerLevel = playerData->fields.PlayerLevel + 1;
				uint8_t playerId = GetPlayerControlById(playerData->fields.PlayerId)->fields._.OwnerId;
				ClientData* host = InnerNetClient_GetHost((InnerNetClient*)(*Game::pAmongUsClient), NULL);
				ClientData* client = InnerNetClient_GetClientFromCharacter((InnerNetClient*)(*Game::pAmongUsClient), (InnerNetObject*)__this, NULL);
				std::string platformId = "Unknown";
				if (client != NULL) {
					auto platform = client->fields.PlatformData->fields.Platform;
					if (client->fields.Character == *Game::pLocalPlayer && State.SpoofPlatform) platform = Platforms__Enum(State.FakePlatform + 1); //fix incorrect platform showing for yourself
					switch (platform) {
					case Platforms__Enum::StandaloneEpicPC:
						platformId = "Epic Games - PC";
						break;
					case Platforms__Enum::StandaloneSteamPC:
						platformId = "Steam - PC";
						break;
					case Platforms__Enum::StandaloneMac:
						platformId = "Mac";
						break;
					case Platforms__Enum::StandaloneWin10:
						platformId = "Microsoft Store - PC";
						break;
					case Platforms__Enum::StandaloneItch:
						platformId = "itch.io - PC";
						break;
					case Platforms__Enum::IPhone:
						platformId = "iOS/iPadOS - Mobile";
						break;
					case Platforms__Enum::Android:
						platformId = "Android - Mobile";
						break;
					case Platforms__Enum::Switch:
						platformId = "Nintendo Switch - Console";
						break;
					case Platforms__Enum::Xbox:
						platformId = "Xbox - Console";
						break;
					case Platforms__Enum::Playstation:
						platformId = "Playstation - Console";
						break;
					default:
						platformId = "Unknown Platform";
						break;
					}
				}
				std::string isAum = std::count(State.aumUsers.begin(), State.aumUsers.end(), playerData->fields.PlayerId) ? " <#f55>[AUM]</color>" : "";
				std::string isSicko = (playerData == localData || std::count(State.sickoUsers.begin(), State.sickoUsers.end(), playerData->fields.PlayerId)) ? " <#afa>[SICKO]</color>" : "";
				std::string levelText = std::format("<#9ef>Level <#0f0>{}</color> ({}){}{}</color>", playerLevel, platformId, isSicko, isAum);
				std::string friendCode = convert_from_string(playerData->fields.FriendCode);
				if (IsStreamerMode())
					friendCode = "Friend Code Hidden";
				std::string hostFriendCode = convert_from_string(InnerNetClient_GetHost((InnerNetClient*)(*Game::pAmongUsClient), NULL)->fields.FriendCode);
				if (client != NULL && client == host) {
					if (friendCode == "" && !IsStreamerMode())
						playerName = "<size=1.4><#0f0>[HOST]</color> " + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#9ef>No Friend Code</color><#0000>0</color>";
					else
						playerName = "<size=1.4><#0f0>[HOST]</color> " + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#9ef>" + friendCode + "</color><#0000>0</color>";
				}
				else {
					if (friendCode == "" && !IsStreamerMode())
						playerName = "<size=1.4>" + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#9ef>No Friend Code</color><#0000>0</color>";
					else
						playerName = "<size=1.4>" + levelText + "</size></color>\n" + playerName + "</color>\n<size=1.4><#0000>0</color><#9ef>" + friendCode + "</color><#0000>0</color>";
				}
			}
			if (IsInGame() && State.RevealRoles && !((State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames) == false) && __this->fields.inVent) && !State.PanicMode)
			{
				std::string roleName = GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames);
				int completedTasks = 0;
				int totalTasks = 0;
				auto tasks = GetNormalPlayerTasks(__this);
				for (auto task : tasks)
				{
					if (task->fields.taskStep == task->fields.MaxStep) {
						completedTasks++;
						totalTasks++;
					}
					else
						totalTasks++;
				}
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
			if (IsInGame() && State.ShowKillCD
				&& !playerData->fields.IsDead
				&& playerData->fields.Role
				&& playerData->fields.Role->fields.CanUseKillButton
				&& !((State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames) == false) && __this->fields.inVent)
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

			if (IsInGame() && (State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && !(GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames))) && __this->fields.inVent && !State.PanicMode) {
				playerName = "<#0000>" + playerName + "</color>";
			}

			else {
				playerName = playerName; //failsafe
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
								CustomNetworkTransform_SnapTo(player->fields.NetTransform, target, player->fields.NetTransform->fields.lastSequenceId, NULL);
								player->fields.NetTransform->fields.lastSequenceId += 100;
								rotateDelay = 0;//25 * float(GetAllPlayerControl().size());
								f += 360 * 0.017453292f / float(GetAllPlayerControl().size());
							}
						}
						else {
							rotateDelay--;
						}
					}
				}
				if (!State.SafeMode && State.ChatSpam && (IsInGame() || IsInLobby())) {
					static float spamDelay = 0;
					auto player = State.playerToChatAs.has_value() ? State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
					if (spamDelay <= 0) {
						for (auto p : GetAllPlayerControl()) {
							if (p == player || State.ChatSpamEveryone) {
								if (IsInGame()) State.rpcQueue.push(new RpcSendChat(p, State.chatMessage));
								if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSendChat(p, State.chatMessage));
							}
						}
						spamDelay = 25 * float(GetAllPlayerControl().size());
					}
					else spamDelay--;
				}

				if ((IsHost() || !State.SafeMode) && State.ForceColorForEveryone)
				{
					static float forceColorDelay = 0;
					for (auto player : GetAllPlayerControl()) {
						if (forceColorDelay <= 0) {
							app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(GetPlayerData(player));
							auto colorId = outfit->fields.ColorId;
							if (IsInGame() && colorId != State.HostSelectedColorId)
								State.rpcQueue.push(new RpcForceColor(player, State.HostSelectedColorId));
							else if (IsInLobby() && colorId != State.HostSelectedColorId)
								State.lobbyRpcQueue.push(new RpcForceColor(player, State.HostSelectedColorId));
							forceColorDelay = State.CycleDuration * GetAllPlayerControl().size();
						}
						else {
							forceColorDelay--;
						}
					}
				}

				if ((IsHost() || !State.SafeMode) && State.Cycler && State.CycleForEveryone && State.RandomColor)
				{
					static float cycleColorDelay = 0;
					if (cycleColorDelay <= 0) {
						for (auto player : GetAllPlayerControl()) {
							if (IsInGame())
								State.rpcQueue.push(new RpcForceColor(player, randi(0, 17)));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcForceColor(player, randi(0, 17)));
						}
						cycleColorDelay = State.CycleDuration * GetAllPlayerControl().size();
					}
					else {
						cycleColorDelay--;
					}
				}

				if ((IsHost() || !State.SafeMode) && State.ForceNameForEveryone) {
					static float forceNameDelay = 0;
					if (forceNameDelay <= 0) {
						for (auto player : GetAllPlayerControl()) {
							if (player != *Game::pLocalPlayer || !(State.CustomName && ((IsHost() || !State.SafeMode) && State.ServerSideCustomName))) {
								app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(GetPlayerData(player));
								std::string playerName = convert_from_string(GameData_PlayerOutfit_get_PlayerName(outfit, nullptr));
								std::string playerId = std::format("{}", player->fields.PlayerId);
								std::string newName = State.hostUserName + "<size=0>" + playerId + "</size>";
								if (IsInGame() && playerName != newName)
									State.rpcQueue.push(new RpcForceName(player, State.hostUserName + "<size=0>" + playerId + "</size>"));
								else if (IsInLobby() && playerName != newName)
									State.lobbyRpcQueue.push(new RpcForceName(player, State.hostUserName + "<size=0>" + playerId + "</size>"));
							}
						}
						forceNameDelay = State.CycleDuration * GetAllPlayerControl().size();
					}
					else {
						forceNameDelay--;
					}
				}


				if ((IsHost() || !State.SafeMode) && State.Cycler && State.CycleForEveryone && State.CycleName)
				{
					static float cycleNameDelay = 0;
					if (cycleNameDelay <= 0) {
						if (State.cyclerNameGeneration < 2 || (State.cyclerNameGeneration == 2 && State.cyclerUserNames.empty())) {
							for (auto p : GetAllPlayerControl()) {
								if (IsInGame())
									State.rpcQueue.push(new RpcForceName(p, State.cyclerNameGeneration == 1 ? GenerateRandomString(true) : GenerateRandomString()));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcForceName(p, State.cyclerNameGeneration == 1 ? GenerateRandomString(true) : GenerateRandomString()));
								cycleNameDelay = State.CycleDuration;
							}
						}
						else if (State.cyclerNameGeneration == 2) {
							static int nameCtr = 0;
							if ((size_t)nameCtr >= State.cyclerUserNames.size())
								nameCtr = 0;
							for (auto p : GetAllPlayerControl()) {
								if (IsInGame())
									State.rpcQueue.push(new RpcSetName(State.cyclerUserNames[nameCtr]));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcSetName(State.cyclerUserNames[nameCtr]));
							}
							nameCtr++;
							cycleNameDelay = State.CycleDuration;
						}
						else {
							for (auto p : GetAllPlayerControl()) {
								if (IsInGame())
									State.rpcQueue.push(new RpcForceName(p, GenerateRandomString()));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcForceName(p, GenerateRandomString()));
								cycleNameDelay = State.CycleDuration;
							}
						}
					}
					else cycleNameDelay--;
				}
			}

			String* playerNameStr = convert_to_string(playerName);
			app::TMP_Text_set_text((app::TMP_Text*)nameTextTMP, playerNameStr, NULL);

			if (IsColorBlindMode()) {
				auto colorBlindText = __this->fields.cosmetics->fields.colorBlindText;
				String* text = TMP_Text_get_text((TMP_Text*)colorBlindText, NULL);
				if ((State.ShowPlayerInfo && IsInLobby()) || (State.ShowKillCD && IsInGame() && PlayerIsImpostor(GetPlayerData(__this))))
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
				if (isPlaying == State.ShowProtections)
					break;
				if (!State.ShowProtections)
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
			} while (State.PanicMode);

			if ((State.Wallhack || State.IsRevived) && __this == *Game::pLocalPlayer && !State.FreeCam
				&& !State.playerToFollow.has_value() && !State.PanicMode) {
				auto mainCamera = Camera_get_main(NULL);

				Transform* cameraTransform = Component_get_transform((Component_1*)mainCamera, NULL);
				Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);
				Transform_set_position(cameraTransform, { cameraVector3.x, cameraVector3.y, 1000 }, NULL);
			}

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
					if (State.EnableZoom && !State.InMeeting && (IsInGame() || IsInLobby()) && !State.PanicMode) //chat button disappears after meeting
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

			if (playerData->fields.IsDead) {
				PlayerControl_set_Visible(__this, State.ShowGhosts || localData->fields.IsDead, NULL);
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
			app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(playerData);
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

				espPlayerData.Name = convert_from_string(GameData_PlayerOutfit_get_PlayerName(outfit, nullptr));
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
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_FixedUpdate (PlayerControl)");
	}
	PlayerControl_FixedUpdate(__this, method);
}

void dPlayerControl_RpcSyncSettings(PlayerControl* __this, Byte__Array* optionsByteArray, MethodInfo* method) {
	try {
		SaveGameOptions();
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in RpcSyncSettings (PlayerControl)");
	}
	PlayerControl_RpcSyncSettings(__this, optionsByteArray, method);
}

bool dPlayerControl_get_CanMove(PlayerControl* __this, MethodInfo* method) {
	try {
		if (!State.PanicMode) {
			if (((State.AlwaysMove) || (State.MoveInVentAndShapeshift && (((*Game::pLocalPlayer)->fields.inVent) || ((*Game::pLocalPlayer)->fields.shapeshifting)))) && !State.ChatFocused && !((*Game::pLocalPlayer)->fields.petting))
				return true;
		}
	}
	catch (...) {
		Log.Debug("Exception occurred in PlayerControl_get_CanMove (PlayerControl)");
	}
	return PlayerControl_get_CanMove(__this, method);
}

void dPlayerControl_OnGameStart(PlayerControl* __this, MethodInfo* method) {
	try {
		SaveGameOptions();
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_OnGameStart (PlayerControl)");
	}
	PlayerControl_OnGameStart(__this, method);
}

void dPlayerControl_MurderPlayer(PlayerControl* __this, PlayerControl* target, MurderResultFlags__Enum resultFlags, MethodInfo* method)
{
	try {
		if (IsInLobby() && target == *Game::pLocalPlayer) return; //for some reason this kicks you from the lobby
		if (static_cast<int32_t>(resultFlags) & static_cast<int32_t>(MurderResultFlags__Enum::FailedError)) {
			app::PlayerControl_MurderPlayer(__this, target, resultFlags, method);
			return;
		}
		auto killer = GetEventPlayerControl(__this);
		auto victim = GetEventPlayerControl(target);
		if (killer && victim) {
			if (PlayerIsImpostor(GetPlayerData(__this)) && PlayerIsImpostor(GetPlayerData(target))) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<CheatDetectedEvent>(killer.value(), CHEAT_ACTIONS::CHEAT_KILL_IMPOSTOR));
				}
			}
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<KillEvent>(killer.value(), victim.value(), PlayerControl_GetTruePosition(__this, NULL), PlayerControl_GetTruePosition(target, NULL)));
				State.replayDeathTimePerPlayer[target->fields.PlayerId] = std::chrono::system_clock::now();
			}
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
		LOG_DEBUG("Exception occurred in PlayerControl_MurderPlayer (PlayerControl)");
	}
	PlayerControl_MurderPlayer(__this, target, resultFlags, method);
}

void dPlayerControl_CmdCheckMurder(PlayerControl* __this, PlayerControl* target, MethodInfo* method)
{
	if (!State.PanicMode) {
		if (State.DisableKills || (State.GodMode && __this == *Game::pLocalPlayer)) return;

		if (State.AlwaysUseKillExploit || State.NoAbilityCD)
			PlayerControl_RpcMurderPlayer(*Game::pLocalPlayer, target, target->fields.protectedByGuardianId < 0 || State.BypassAngelProt, NULL);
		else if (IsInLobby())
			PlayerControl_RpcMurderPlayer(*Game::pLocalPlayer, target, target->fields.protectedByGuardianId < 0 || State.BypassAngelProt, NULL);
		else if (State.RealRole != RoleTypes__Enum::Impostor && State.RealRole != RoleTypes__Enum::Shapeshifter)
			PlayerControl_RpcMurderPlayer(*Game::pLocalPlayer, target, target->fields.protectedByGuardianId < 0 || State.BypassAngelProt, NULL);
		else if ((*Game::pLocalPlayer)->fields.killTimer > 0)
			PlayerControl_RpcMurderPlayer(*Game::pLocalPlayer, target, target->fields.protectedByGuardianId < 0 || State.BypassAngelProt, NULL);
		else
			PlayerControl_CmdCheckMurder(*Game::pLocalPlayer, target, NULL);
	}
	else PlayerControl_CmdCheckMurder(__this, target, method);
}

void dPlayerControl_RpcShapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method)
{
	if (!State.PanicMode && __this == *Game::pLocalPlayer) PlayerControl_RpcShapeshift(__this, target, (State.PanicMode ? animate : (!State.AnimationlessShapeshift && animate)), method);
	else PlayerControl_RpcShapeshift(__this, target, animate, method);
}

void dPlayerControl_CmdCheckShapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method)
{
	if (!State.PanicMode && !State.SafeMode && __this == *Game::pLocalPlayer) PlayerControl_RpcShapeshift(__this, target, (!State.AnimationlessShapeshift && animate), method);
	else PlayerControl_CmdCheckShapeshift(__this, target, (State.PanicMode ? animate : (!State.AnimationlessShapeshift && animate)), method);
}

void dPlayerControl_CmdCheckRevertShapeshift(PlayerControl* __this, bool animate, MethodInfo* method)
{
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
		LOG_DEBUG("Exception occurred in PlayerControl_RpcRevertShapeshift (PlayerControl)");
	}
}*/

void dPlayerControl_StartMeeting(PlayerControl* __this, GameData_PlayerInfo* target, MethodInfo* method)
{
	try {
		if (!State.PanicMode && State.DisableMeetings) {
			return;
		}
		else {
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<ReportDeadBodyEvent>(GetEventPlayerControl(__this).value(), GetEventPlayer(target), PlayerControl_GetTruePosition(__this, NULL), GetTargetPosition(target)));
			}
		}
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_StartMeeting (PlayerControl)");
	}
	PlayerControl_StartMeeting(__this, target, method);
}

void dPlayerControl_HandleRpc(PlayerControl* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
	try {
		if (!State.PanicMode) {
			HandleRpc(__this, callId, reader);
			if (IsHost() && ((State.DisableMeetings && (callId == (uint8_t)RpcCalls__Enum::ReportDeadBody || callId == (uint8_t)RpcCalls__Enum::StartMeeting)) ||
				(State.DisableSabotages && (callId == (uint8_t)RpcCalls__Enum::CloseDoorsOfType || callId == (uint8_t)RpcCalls__Enum::UpdateSystem)) ||
				(State.DisableKills && callId == (uint8_t)RpcCalls__Enum::CheckMurder))) //we cannot prevent murderplayer because the player will force it
				return;
			int crew = 0, imp = 0;
			for (auto p : GetAllPlayerData()) {
				if (p->fields.IsDead) continue;
				PlayerIsImpostor(p) ? imp++ : crew++;
			}
			bool shouldCheckMeeting = (imp >= crew) || imp == 0;
			if (State.NoGameEnd && shouldCheckMeeting && 
				(callId == (uint8_t)RpcCalls__Enum::ReportDeadBody || callId == (uint8_t)RpcCalls__Enum::StartMeeting))
				return;
		}
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_HandleRpc (PlayerControl)");
	}
	PlayerControl_HandleRpc(__this, callId, reader, NULL);
}

void dRenderer_set_enabled(Renderer* __this, bool value, MethodInfo* method)
{
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
							playerInfo->fields._object->fields.MyPhysics->fields.Animations;
						}
					}
				}
			}
		}
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in Renderer_set_enabled (PlayerControl)");
	}
	Renderer_set_enabled(__this, value, method);
}

void dGameObject_SetActive(GameObject* __this, bool value, MethodInfo* method)
{
	try {
		if (!State.PanicMode) {
			if ((IsInGame() || IsInLobby()) && !value) { //If we're already rendering it, lets skip checking if we should
				for (auto player : GetAllPlayerControl()) {
					auto playerInfo = GetPlayerData(player);
					if (!playerInfo || !player->fields.cosmetics) break; //This happens sometimes during loading
					if ((playerInfo->fields.IsDead && State.ShowGhosts) || (State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames) == false))
					{
						auto nameObject = Component_get_gameObject((Component_1*)player->fields.cosmetics->fields.nameText, NULL);
						if (nameObject == __this) {
							value = true;
							break;
						}
					}
					else if (!State.RevealRoles && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && GameOptions().GetBool(app::BoolOptionNames__Enum::ShowCrewmateNames) == false) {
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
		LOG_DEBUG("Exception occurred in GameObject_SetActive (PlayerControl)");
	}
	GameObject_SetActive(__this, value, method);
}

void dPlayerControl_CmdReportDeadBody(PlayerControl* __this, GameData_PlayerInfo* target, MethodInfo* method) {
	try {
		if (!State.PanicMode && State.DisableMeetings) {
			return;
		}
	}
	catch (...) {
		Log.Debug("Exception occurred in CmdReportDeadBody (PlayerControl)");
	}
	PlayerControl_CmdReportDeadBody(__this, target, method);
}

void dPlayerControl_RpcStartMeeting(PlayerControl* __this, GameData_PlayerInfo* target, MethodInfo* method) {
	try {
		if (State.DisableMeetings) {
			return;
		}
	}
	catch (...) {
		Log.Debug("Exception occurred in PlayerControl_RpcStartMeeting (PlayerControl)");
	}
	PlayerControl_RpcStartMeeting(__this, target, method);
}

void dPlayerControl_Shapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method) {
	try {
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<ShapeShiftEvent>(GetEventPlayerControl(__this).value(), GetEventPlayerControl(target).value()));
		}
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_Shapeshift (PlayerControl)");
	}
	PlayerControl_Shapeshift(__this, target, animate, method);
}

void dPlayerControl_ProtectPlayer(PlayerControl* __this, PlayerControl* target, int32_t colorId, MethodInfo* method) {
	try {
		if (SYNCHRONIZED(Replay::replayEventMutex); target != nullptr) {
			State.liveReplayEvents.emplace_back(std::make_unique<ProtectPlayerEvent>(GetEventPlayerControl(__this).value(), GetEventPlayerControl(target).value()));
		}
		else {
			LOG_ERROR("target is null"); // TownOfHost
		}
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_ProtectPlayer (PlayerControl)");
	}
	PlayerControl_ProtectPlayer(__this, target, colorId, method);
}

void dPlayerControl_TurnOnProtection(PlayerControl* __this, bool visible, int32_t colorId, int32_t guardianPlayerId, MethodInfo* method) {
	try {
		app::PlayerControl_TurnOnProtection(__this, visible || State.ShowProtections, colorId, guardianPlayerId, method);
		std::pair pair { colorId, app::Time_get_time(nullptr) };
		synchronized(State.protectMutex) {
			State.protectMonitor[__this->fields.PlayerId] = pair;
		}
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in PlayerControl_TurnOnProtection (PlayerControl)");
		app::PlayerControl_TurnOnProtection(__this, visible, colorId, guardianPlayerId, method);
	}
}

void dPlayerControl_RemoveProtection(PlayerControl* __this, MethodInfo* method) {
	try {
		State.protectMonitor.erase(__this->fields.PlayerId);
	}
	catch (...) {
		Log.Debug("Exception occurred in PlayerControl_RemoveProtection (PlayerControl)");
	}
	PlayerControl_RemoveProtection(__this, method);
}

void dKillButton_SetTarget(KillButton* __this, PlayerControl* target, MethodInfo* method) {
	auto result = target;
	bool amImpostor = PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer));
	if (State.UnlockKillButton && !amImpostor) {
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
				if (p == *Game::pLocalPlayer) continue; //we don't want to kill ourselves
				auto pData = GetPlayerData(p);
				if (PlayerIsImpostor(pData) && !State.KillImpostors) continue; //neither impostors
				if (pData->fields.IsDead) continue; //nor ghosts
				float currentDist = GetDistanceBetweenPoints_Unity(GetTrueAdjustedPosition(p), localPos);
				if (currentDist < max_dist) {
					new_result = p;
					max_dist = currentDist;
				}
			}
			result = new_result;
		}

		if (!State.PanicMode && result != nullptr && State.AutoKill && (State.AlwaysMove || PlayerControl_get_CanMove(*Game::pLocalPlayer, NULL)) && (*Game::pLocalPlayer)->fields.killTimer <= 0.f) {
			if (IsInGame()) State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, result));
			else State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, result));
		}
		KillButton_SetTarget(__this, result, NULL);
	}
	else if (amImpostor) KillButton_SetTarget(__this, result, NULL);
	else KillButton_SetTarget(__this, NULL, NULL);
}

PlayerControl* dImpostorRole_FindClosestTarget(ImpostorRole* __this, MethodInfo* method) {
	auto result = ImpostorRole_FindClosestTarget(__this, method);
	if (!State.PanicMode && result == nullptr && State.InfiniteKillRange) {
		PlayerControl* new_result = nullptr;
		float max_dist = FLT_MAX;
		auto localPos = GetTrueAdjustedPosition(*Game::pLocalPlayer);
		for (auto p : GetAllPlayerControl()) {
			if (p == *Game::pLocalPlayer) continue; //we don't want to kill ourselves
			auto pData = GetPlayerData(p);
			if (PlayerIsImpostor(pData) && !State.KillImpostors) continue; //neither impostors
			if (pData->fields.IsDead) continue; //nor ghosts
			float currentDist = GetDistanceBetweenPoints_Unity(GetTrueAdjustedPosition(p), localPos);
			if (currentDist < max_dist) {
				new_result = p;
				max_dist = currentDist;
			}
		}
		result = new_result;
	}

	if (!State.PanicMode && result != nullptr && State.AutoKill && (State.AlwaysMove || PlayerControl_get_CanMove(*Game::pLocalPlayer, NULL)) && (*Game::pLocalPlayer)->fields.killTimer <= 0.f) {
		if (IsInGame()) State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, result));
		else State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, result));
	}
	return result;
}

float dConsole_1_CanUse(Console_1* __this, GameData_PlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method) {
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
		LOG_DEBUG("Exception occurred in Console_1_CanUse (PlayerControl)");
	}
	return Console_1_CanUse(__this, pc, canUse, couldUse, method);
}

void dPlayerControl_SetRole(PlayerControl* __this, RoleTypes__Enum role, MethodInfo* method) {
	if (__this == *Game::pLocalPlayer) State.RealRole = role;
	if (!IsInMultiplayerGame() || __this != *Game::pLocalPlayer) {
		PlayerControl_SetRole(__this, role, method);
		return;
	}
	bool hasAlreadySetRole = role == RoleTypes__Enum::GuardianAngel || role == RoleTypes__Enum::CrewmateGhost || role == RoleTypes__Enum::ImpostorGhost;
	if (State.AutoFakeRole && !hasAlreadySetRole) {
		if (State.RealRole != RoleTypes__Enum::Shapeshifter && State.FakeRole == 5 && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead)
			role = RoleTypes__Enum::Impostor;

		else role = (RoleTypes__Enum)State.FakeRole;
	}
	PlayerControl_SetRole(__this, role, method);
}