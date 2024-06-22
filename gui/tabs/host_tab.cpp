#include "pch-il2cpp.h"
#include "host_tab.h"
#include "utility.h"
#include "game.h"
#include "state.hpp"
#include "gui-helpers.hpp"

namespace HostTab {
	enum Groups {
		Utils,
		Settings
	};

	static bool openUtils = true; //default to utils tab group
	static bool openSettings = false;

	void CloseOtherGroups(Groups group) {
		openUtils = group == Groups::Utils;
		openSettings = group == Groups::Settings;
	}

	static void SetRoleAmount(RoleTypes__Enum type, int amount) {
		auto&& options = GameOptions().GetRoleOptions();
		auto maxCount = options.GetNumPerGame(type);
		if (amount > maxCount)
			options.SetRoleRate(type, amount, 100);
		else if (amount > 0)
			options.SetRoleRate(type, maxCount, 100);
	}

	void SyncAllSettings() {
		if (IsInGame()) State.rpcQueue.push(new RpcSyncSettings());
		if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSyncSettings());
	}

	const ptrdiff_t GetRoleCount(RoleType role)
	{
		return std::count_if(State.assignedRoles.cbegin(), State.assignedRoles.cend(), [role](RoleType i) {return i == role; });
	}

	void Render() {
		if (IsHost()) {
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::BeginChild("###Host", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
			if (TabGroup("Utils", openUtils)) {
				CloseOtherGroups(Groups::Utils);
			}
			if ((IsInGame() || IsInLobby()) && GameOptions().HasOptions()) {
				ImGui::SameLine();
				if (TabGroup("Settings", openSettings)) {
					CloseOtherGroups(Groups::Settings);
				}
			}
			GameOptions options;
			if (openUtils) {
				if (IsInLobby()) {
					ImGui::BeginChild("host#list", ImVec2(200, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
					bool shouldEndListBox = ImGui::ListBoxHeader("Choose Roles", ImVec2(200, 290) * State.dpiScale);
					auto allPlayers = GetAllPlayerData();
					auto playerAmount = allPlayers.size();
					auto maxImpostorAmount = GetMaxImpostorAmount((int)playerAmount);
					for (size_t index = 0; index < playerAmount; index++) {
						auto playerData = allPlayers[index];
						if (playerData == nullptr) continue;
						PlayerControl* playerCtrl = GetPlayerControlById(playerData->fields.PlayerId);
						if (playerCtrl == nullptr) continue;
						State.assignedRolesPlayer[index] = playerCtrl;
						if (State.assignedRolesPlayer[index] == nullptr)
							continue;

						auto outfit = GetPlayerOutfit(playerData);
						if (outfit == NULL) continue;
						const std::string& playerName = convert_from_string(outfit->fields.PlayerName);
						//player colors in host tab by gdjkhp (https://github.com/GDjkhp/AmongUsMenu/commit/53b017183bac503c546f198e2bc03539a338462c)
						if (CustomListBoxInt(playerName.c_str(), reinterpret_cast<int*>(&State.assignedRoles[index]), ROLE_NAMES, 80 * State.dpiScale, AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId))))
						{
							State.engineers_amount = (int)GetRoleCount(RoleType::Engineer);
							State.scientists_amount = (int)GetRoleCount(RoleType::Scientist);
							State.trackers_amount = (int)GetRoleCount(RoleType::Tracker);
							State.noisemakers_amount = (int)GetRoleCount(RoleType::Noisemaker);
							State.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
							State.phantoms_amount = (int)GetRoleCount(RoleType::Phantom);
							State.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
							if (State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount > maxImpostorAmount)
							{
								if (State.assignedRoles[index] == RoleType::Impostor)
									State.assignedRoles[index] = RoleType::Random;
								else if (State.assignedRoles[index] == RoleType::Shapeshifter)
									State.assignedRoles[index] = RoleType::Random;
								else if (State.assignedRoles[index] == RoleType::Tracker)
									State.assignedRoles[index] = RoleType::Random;
								State.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
								State.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
								State.crewmates_amount = (int)GetRoleCount(RoleType::Crewmate);
							}

							if (State.assignedRoles[index] == RoleType::Engineer || State.assignedRoles[index] == RoleType::Scientist || 
								State.assignedRoles[index] == RoleType::Tracker || State.assignedRoles[index] == RoleType::Noisemaker || 
								State.assignedRoles[index] == RoleType::Crewmate) {
								if (State.engineers_amount + State.scientists_amount + State.trackers_amount + State.noisemakers_amount + State.crewmates_amount >= (int)playerAmount)
									State.assignedRoles[index] = RoleType::Random;
							} //Some may set all players to non imps. This hangs the game on beginning. Leave space to Random so we have imps.
							
							if (options.GetGameMode() == GameModes__Enum::HideNSeek)
							{
								if (State.assignedRoles[index] == RoleType::Shapeshifter)
									State.assignedRoles[index] = RoleType::Random;
								else if (State.assignedRoles[index] == RoleType::Phantom)
									State.assignedRoles[index] = RoleType::Random;
								else if (State.assignedRoles[index] == RoleType::Tracker)
									State.assignedRoles[index] = RoleType::Engineer;
								else if (State.assignedRoles[index] == RoleType::Noisemaker)
									State.assignedRoles[index] = RoleType::Engineer;
								else if (State.assignedRoles[index] == RoleType::Crewmate)
									State.assignedRoles[index] = RoleType::Engineer;
							} //Assign other roles in hidenseek causes game bug.
							//These are organized. Do not change the order unless you find it necessary.

							if (!IsInGame()) {
								if (options.GetGameMode() == GameModes__Enum::HideNSeek)
									SetRoleAmount(RoleTypes__Enum::Engineer, 15);
								else
									SetRoleAmount(RoleTypes__Enum::Engineer, State.engineers_amount);
								SetRoleAmount(RoleTypes__Enum::Scientist, State.scientists_amount);
								SetRoleAmount(RoleTypes__Enum::Tracker, State.trackers_amount);
								SetRoleAmount(RoleTypes__Enum::Noisemaker, State.noisemakers_amount);
								SetRoleAmount(RoleTypes__Enum::Shapeshifter, State.shapeshifters_amount);
								SetRoleAmount(RoleTypes__Enum::Phantom, State.phantoms_amount);
								if (options.GetNumImpostors() <= State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount)
									options.SetInt(app::Int32OptionNames__Enum::NumImpostors, State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount);
							}
						}
					}
					if (shouldEndListBox)
						ImGui::ListBoxFooter();
					ImGui::EndChild();
				}
				if (IsInLobby()) ImGui::SameLine();
				ImGui::BeginChild("host#actions", ImVec2(300, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);

				if (IsInLobby() && ToggleButton("Custom Impostor Amount", &State.CustomImpostorAmount))
					State.Save();
				State.ImpostorCount = std::clamp(State.ImpostorCount, 0, int(Game::MAX_PLAYERS));
				if (IsInLobby() && State.CustomImpostorAmount && ImGui::InputInt("Impostor Count", &State.ImpostorCount))
					State.Save();

				/*int32_t maxPlayers = options.GetMaxPlayers();
				maxPlayers = std::clamp(maxPlayers, 0, int(Game::MAX_PLAYERS));
				if (IsInLobby() && ImGui::InputInt("Max Players", &maxPlayers))
					GameOptions().SetInt(app::Int32OptionNames__Enum::MaxPlayers, maxPlayers);*/ //support for more than 15 players

					/*if (IsInLobby() && ToggleButton("Flip Skeld", &State.FlipSkeld))
						State.Save();*/ //to be fixed later
				ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
				if (IsInLobby() && ImGui::Button("Force Start of Game"))
				{
					app::InnerNetClient_SendStartGame((InnerNetClient*)(*Game::pAmongUsClient), NULL);
				}
				if (ToggleButton("Disable Meetings", &State.DisableMeetings))
					State.Save();
				if (ToggleButton("Disable Sabotages", &State.DisableSabotages))
					State.Save();
				if (ToggleButton("Disable Kills", &State.DisableKills))
					State.Save();
				if (State.DisableKills) ImGui::Text("Note: Cheaters can still bypass this feature!");

				/*if (ToggleButton("Disable Specific RPC Call ID", &State.DisableCallId))
					State.Save();
				int callId = State.ToDisableCallId;
				if (ImGui::InputInt("ID to Disable", &callId)) {
					State.ToDisableCallId = (uint8_t)callId;
					State.Save();
				}*/

				if ((State.mapType == Settings::MapType::Airship) && IsInGame() && ImGui::Button("Switch Moving Platform Side"))
				{
					State.rpcQueue.push(new RpcUsePlatform());
				}

				if (State.InMeeting && ImGui::Button("End Meeting")) {
					State.rpcQueue.push(new RpcEndMeeting());
					State.InMeeting = false;
				}

				if (IsInMultiplayerGame() || IsInLobby()) { //lobby isn't possible in freeplay
					if (ToggleButton("Disable Game Ending", &State.NoGameEnd)) {
						State.Save();
					}

					if (IsInGame()) {
						CustomListBoxInt("Reason", &State.SelectedGameEndReasonId, GAMEENDREASON, 120.0f * State.dpiScale);

						ImGui::SameLine();

						if (ImGui::Button("End Game")) {
							State.rpcQueue.push(new RpcEndGame(GameOverReason__Enum(std::clamp(State.SelectedGameEndReasonId, 0, 8))));
						}
					}
				}

				CustomListBoxInt(" ­", &State.HostSelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);

				if (ToggleButton("Force Color for Everyone", &State.ForceColorForEveryone)) {
					State.Save();
				}

				if (ToggleButton("Force Name for Everyone", &State.ForceNameForEveryone)) {
					State.Save();
				}
				if (InputString("Username", &State.hostUserName)) {
					State.Save();
				}

				if (IsHost() && IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && ImGui::Button("Revive Yourself"))
				{
					if (PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))) {
						if (IsInGame()) State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
					}
					else {
						if (IsInGame()) State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Crewmate));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Crewmate));
					}
				}

				if (isReviving && framesPassed == 50)
				{
					State.rpcQueue.push(new RpcVent(*Game::pLocalPlayer, 1, false));
					framesPassed--;
				}
				else if (isReviving && framesPassed <= 0 && (*Game::pLocalPlayer)->fields.inVent) {
					State.rpcQueue.push(new RpcVent(*Game::pLocalPlayer, 1, true)); //for showing you as alive to ALL players
					framesPassed = -1;
					isReviving = false;
				}
				else if (isReviving) framesPassed--;

				ImGui::EndChild();
			}

			if (openSettings) {
				// AU v2022.8.24 has been able to change maps in lobby.
				State.mapHostChoice = options.GetByte(app::ByteOptionNames__Enum::MapId);
				if (State.mapHostChoice > 3)
					State.mapHostChoice--;
				State.mapHostChoice = std::clamp(State.mapHostChoice, 0, (int)MAP_NAMES.size() - 1);
				if (IsInLobby() && CustomListBoxInt("Map", &State.mapHostChoice, MAP_NAMES, 75 * State.dpiScale)) {
					//if (!IsInGame()) {
						// disable flip
						/*if (State.mapHostChoice == 3) {
							options.SetByte(app::ByteOptionNames__Enum::MapId, 0);
							State.FlipSkeld = true;
						}
						else {
							options.SetByte(app::ByteOptionNames__Enum::MapId, State.mapHostChoice);
							State.FlipSkeld = false;
						}*/
						auto id = State.mapHostChoice;
						if (id >= 3) id++;
						options.SetByte(app::ByteOptionNames__Enum::MapId, id);
						SyncAllSettings();
					//}
				}
				auto gamemode = options.GetGameMode();

				if (gamemode == GameModes__Enum::Normal || gamemode == GameModes__Enum::NormalFools) {
					static int emergencyMeetings = 1, emergencyCooldown = 1, discussionTime = 1, 
						votingTime = 1, killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, taskBarMode = 1;

					static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f;
					
					static bool ejects = false, anonVotes = false, visualTasks = false;

					if (ToggleButton("Confirm Ejects", &ejects)) {
						options.SetBool(BoolOptionNames__Enum::ConfirmImpostor, ejects);
						SyncAllSettings();
					}
					else ejects = options.GetBool(BoolOptionNames__Enum::ConfirmImpostor);

					if (ImGui::InputInt("# Emergency Meetings", &emergencyMeetings)) {
						options.SetInt(Int32OptionNames__Enum::NumEmergencyMeetings, emergencyMeetings);
						SyncAllSettings();
					}
					else emergencyMeetings = options.GetInt(Int32OptionNames__Enum::NumEmergencyMeetings);

					if (ToggleButton("Anonymous Votes", &anonVotes)) {
						options.SetBool(BoolOptionNames__Enum::AnonymousVotes, anonVotes);
						SyncAllSettings();
					}
					else anonVotes = options.GetBool(BoolOptionNames__Enum::AnonymousVotes);

					if (ImGui::InputInt("Emergency Cooldown", &emergencyCooldown)) {
						options.SetInt(Int32OptionNames__Enum::EmergencyCooldown, emergencyCooldown);
						SyncAllSettings();
					}
					else emergencyCooldown = options.GetInt(Int32OptionNames__Enum::EmergencyCooldown);

					if (ImGui::InputInt("Discussion Time", &discussionTime)) {
						options.SetInt(Int32OptionNames__Enum::DiscussionTime, discussionTime);
						SyncAllSettings();
					}
					else discussionTime = options.GetInt(Int32OptionNames__Enum::DiscussionTime);

					if (ImGui::InputInt("Voting Time", &votingTime)) {
						options.SetInt(Int32OptionNames__Enum::VotingTime, votingTime);
						SyncAllSettings();
					}
					else votingTime = options.GetInt(Int32OptionNames__Enum::VotingTime);

					if (ImGui::InputFloat("Player Speed", &playerSpeed)) {
						options.SetFloat(FloatOptionNames__Enum::PlayerSpeedMod, playerSpeed);
						SyncAllSettings();
					}
					else playerSpeed = options.GetFloat(FloatOptionNames__Enum::PlayerSpeedMod);

					if (ImGui::InputInt("Task Bar Updates", &taskBarMode)) {
						options.SetInt(Int32OptionNames__Enum::TaskBarMode, taskBarMode);
						SyncAllSettings();
					}
					else taskBarMode = options.GetInt(Int32OptionNames__Enum::TaskBarMode);

					if (ToggleButton("Visual Tasks", &visualTasks)) {
						options.SetBool(BoolOptionNames__Enum::VisualTasks, visualTasks);
						SyncAllSettings();
					}
					else visualTasks = options.GetBool(BoolOptionNames__Enum::VisualTasks);

					if (ImGui::InputFloat("Crewmate Vision", &crewVision)) {
						options.SetFloat(FloatOptionNames__Enum::CrewLightMod, crewVision);
						SyncAllSettings();
					}
					else crewVision = options.GetFloat(FloatOptionNames__Enum::CrewLightMod);

					if (ImGui::InputFloat("Impostor Vision", &impVision)) {
						options.SetFloat(FloatOptionNames__Enum::ImpostorLightMod, impVision);
						SyncAllSettings();
					}
					else impVision = options.GetFloat(FloatOptionNames__Enum::ImpostorLightMod);

					if (ImGui::InputFloat("Kill Cooldown", &killCooldown)) {
						options.SetFloat(FloatOptionNames__Enum::KillCooldown, killCooldown);
						SyncAllSettings();
					}
					else killCooldown = options.GetFloat(FloatOptionNames__Enum::KillCooldown);

					if (ImGui::InputInt("Kill Distance", &killDistance)) {
						options.SetInt(Int32OptionNames__Enum::KillDistance, killDistance);
						SyncAllSettings();
					}
					else killDistance = options.GetInt(Int32OptionNames__Enum::KillDistance);

					if (ImGui::InputInt("# Common Tasks", &commonTasks)) {
						options.SetInt(Int32OptionNames__Enum::NumCommonTasks, commonTasks);
						SyncAllSettings();
					}
					else commonTasks = options.GetInt(Int32OptionNames__Enum::NumCommonTasks);

					if (ImGui::InputInt("# Long Tasks", &longTasks)) {
						options.SetInt(Int32OptionNames__Enum::NumLongTasks, longTasks);
						SyncAllSettings();
					}
					else longTasks = options.GetInt(Int32OptionNames__Enum::NumLongTasks);

					if (ImGui::InputInt("# Short Tasks", &shortTasks)) {
						options.SetInt(Int32OptionNames__Enum::NumShortTasks, shortTasks);
						SyncAllSettings();
					}
					else shortTasks = options.GetInt(Int32OptionNames__Enum::NumShortTasks);

					ImGui::Text("Scientist");
					static float vitalsCooldown = 1.f, batteryDuration = 1.f;

					if (ImGui::InputFloat("Vitals Display Cooldown", &vitalsCooldown)) {
						options.SetFloat(FloatOptionNames__Enum::ScientistCooldown, vitalsCooldown);
						SyncAllSettings();
					}
					else vitalsCooldown = options.GetFloat(FloatOptionNames__Enum::ScientistCooldown);

					if (ImGui::InputFloat("Battery Duration", &batteryDuration)) {
						options.SetFloat(FloatOptionNames__Enum::ScientistBatteryCharge, batteryDuration);
						SyncAllSettings();
					}
					else batteryDuration = options.GetFloat(FloatOptionNames__Enum::ScientistBatteryCharge);

					ImGui::Text("Engineer");
					static float ventCooldown = 1.f, ventDuration = 1.f;

					if (ImGui::InputFloat("Vent Use Cooldown", &ventCooldown)) {
						options.SetFloat(FloatOptionNames__Enum::EngineerCooldown, ventCooldown);
						SyncAllSettings();
					}
					else ventCooldown = options.GetFloat(FloatOptionNames__Enum::EngineerCooldown);

					if (ImGui::InputFloat("Max Time in Vents", &ventDuration)) {
						options.SetFloat(FloatOptionNames__Enum::EngineerInVentMaxTime, ventDuration);
						SyncAllSettings();
					}
					else ventDuration = options.GetFloat(FloatOptionNames__Enum::EngineerInVentMaxTime);

					ImGui::Text("Guardian Angel");
					static float protectCooldown = 1.f, protectDuration = 1.f;
					static bool protectVisible = false;

					if (ImGui::InputFloat("Protect Cooldown", &protectCooldown)) {
						options.SetFloat(FloatOptionNames__Enum::GuardianAngelCooldown, protectCooldown);
						SyncAllSettings();
					}
					else protectCooldown = options.GetFloat(FloatOptionNames__Enum::GuardianAngelCooldown);

					if (ImGui::InputFloat("Protection Duration", &protectDuration)) {
						options.SetFloat(FloatOptionNames__Enum::ProtectionDurationSeconds, protectDuration);
						SyncAllSettings();
					}
					else protectDuration = options.GetFloat(FloatOptionNames__Enum::ProtectionDurationSeconds);

					if (ToggleButton("Protect Visible to Impostors", &protectVisible)) {
						options.SetBool(BoolOptionNames__Enum::ImpostorsCanSeeProtect, protectVisible);
						SyncAllSettings();
					}
					else protectVisible = options.GetBool(BoolOptionNames__Enum::ImpostorsCanSeeProtect);

					ImGui::Text("Shapeshifter");
					static float shapeshiftDuration = 1.f, shapeshiftCooldown = 1.f;
					static bool shapeshiftEvidence = false;

					if (ImGui::InputFloat("Shapeshift Duration", &shapeshiftDuration)) {
						options.SetFloat(FloatOptionNames__Enum::ShapeshifterDuration, shapeshiftDuration);
						SyncAllSettings();
					}
					else shapeshiftDuration = options.GetFloat(FloatOptionNames__Enum::ShapeshifterDuration);

					if (ImGui::InputFloat("Shapesift Cooldown", &shapeshiftCooldown)) {
						options.SetFloat(FloatOptionNames__Enum::ShapeshifterCooldown, shapeshiftCooldown);
						SyncAllSettings();
					}
					else shapeshiftCooldown = options.GetFloat(FloatOptionNames__Enum::ShapeshifterCooldown);

					if (ToggleButton("Leave Shapeshifting Evidence", &shapeshiftEvidence)) {
						options.SetBool(BoolOptionNames__Enum::ShapeshifterLeaveSkin, shapeshiftEvidence);
						SyncAllSettings();
					}
					else shapeshiftEvidence = options.GetBool(BoolOptionNames__Enum::ShapeshifterLeaveSkin);
				}
				
				if (gamemode == GameModes__Enum::HideNSeek || gamemode == GameModes__Enum::SeekFools) {
					static int killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, maxVents = 1;

					static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f,
						hidingTime = 1.f, finalHideTime = 1.f, ventTime = 1.f, crewLight = 1.f, impLight = 1.f,
						finalImpSpeed = 1.f, pingInterval = 1.f;
					
					static bool flashlight = false, seekMap = false, hidePings = false, showNames = false;

					if (ImGui::InputFloat("Crewmate Vision", &crewVision)) {
						options.SetFloat(FloatOptionNames__Enum::CrewLightMod, crewVision);
						SyncAllSettings();
					}
					else crewVision = options.GetFloat(FloatOptionNames__Enum::CrewLightMod);

					if (ImGui::InputFloat("Impostor Vision", &impVision)) {
						options.SetFloat(FloatOptionNames__Enum::ImpostorLightMod, impVision);
						SyncAllSettings();
					}
					else impVision = options.GetFloat(FloatOptionNames__Enum::ImpostorLightMod);

					if (ImGui::InputFloat("Kill Cooldown", &killCooldown)) {
						options.SetFloat(FloatOptionNames__Enum::KillCooldown, killCooldown);
						SyncAllSettings();
					}
					else killCooldown = options.GetFloat(FloatOptionNames__Enum::KillCooldown);

					if (ImGui::InputInt("Kill Distance", &killDistance)) {
						options.SetInt(Int32OptionNames__Enum::KillDistance, killDistance);
						SyncAllSettings();
					}
					else killDistance = options.GetInt(Int32OptionNames__Enum::KillDistance);

					if (ImGui::InputInt("# Common Tasks", &commonTasks)) {
						options.SetInt(Int32OptionNames__Enum::NumCommonTasks, commonTasks);
						SyncAllSettings();
					}
					else commonTasks = options.GetInt(Int32OptionNames__Enum::NumCommonTasks);

					if (ImGui::InputInt("# Long Tasks", &longTasks)) {
						options.SetInt(Int32OptionNames__Enum::NumLongTasks, longTasks);
						SyncAllSettings();
					}
					else longTasks = options.GetInt(Int32OptionNames__Enum::NumLongTasks);

					if (ImGui::InputInt("# Short Tasks", &shortTasks)) {
						options.SetInt(Int32OptionNames__Enum::NumShortTasks, shortTasks);
						SyncAllSettings();
					}
					else shortTasks = options.GetInt(Int32OptionNames__Enum::NumShortTasks);

					if (ImGui::InputFloat("Player Speed", &playerSpeed)) {
						options.SetFloat(FloatOptionNames__Enum::PlayerSpeedMod, playerSpeed);
						SyncAllSettings();
					}
					else playerSpeed = options.GetFloat(FloatOptionNames__Enum::PlayerSpeedMod);

					if (ImGui::InputFloat("Hiding Time", &hidingTime)) {
						options.SetFloat(FloatOptionNames__Enum::EscapeTime, hidingTime);
						SyncAllSettings();
					}
					else hidingTime = options.GetFloat(FloatOptionNames__Enum::EscapeTime);

					if (ImGui::InputFloat("Final Hide Time", &finalHideTime)) {
						options.SetFloat(FloatOptionNames__Enum::FinalEscapeTime, finalHideTime);
						SyncAllSettings();
					}
					else finalHideTime = options.GetFloat(FloatOptionNames__Enum::FinalEscapeTime);

					if (ImGui::InputInt("Max Vent Uses", &maxVents)) {
						options.SetInt(Int32OptionNames__Enum::CrewmateVentUses, maxVents);
						SyncAllSettings();
					}
					else maxVents = options.GetInt(Int32OptionNames__Enum::CrewmateVentUses);

					if (ImGui::InputFloat("Max Time in Vent", &ventTime)) {
						options.SetFloat(FloatOptionNames__Enum::CrewmateTimeInVent, ventTime);
						SyncAllSettings();
					}
					else ventTime = options.GetFloat(FloatOptionNames__Enum::CrewmateTimeInVent);

					if (ToggleButton("Flashlight Mode", &flashlight)) {
						options.SetBool(BoolOptionNames__Enum::UseFlashlight, flashlight);
						SyncAllSettings();
					}
					else flashlight = options.GetBool(BoolOptionNames__Enum::UseFlashlight);

					if (ImGui::InputFloat("Crewmate Flashlight Size", &crewLight)) {
						options.SetFloat(FloatOptionNames__Enum::CrewmateFlashlightSize, crewLight);
						SyncAllSettings();
					}
					else crewLight = options.GetFloat(FloatOptionNames__Enum::CrewmateFlashlightSize);

					if (ImGui::InputFloat("Impostor Flashlight Size", &impLight)) {
						options.SetFloat(FloatOptionNames__Enum::ImpostorFlashlightSize, impLight);
						SyncAllSettings();
					}
					else impLight = options.GetFloat(FloatOptionNames__Enum::ImpostorFlashlightSize);

					if (ImGui::InputFloat("Final Hide Impostor Speed", &finalImpSpeed)) {
						options.SetFloat(FloatOptionNames__Enum::SeekerFinalSpeed, finalImpSpeed);
						SyncAllSettings();
					}
					else finalImpSpeed = options.GetFloat(FloatOptionNames__Enum::SeekerFinalSpeed);

					if (ToggleButton("Final Hide Seek Map", &seekMap)) {
						options.SetBool(BoolOptionNames__Enum::SeekerFinalMap, seekMap);
						SyncAllSettings();
					}
					else seekMap = options.GetBool(BoolOptionNames__Enum::SeekerFinalMap);

					if (ToggleButton("Final Hide Pings", &hidePings)) {
						options.SetBool(BoolOptionNames__Enum::SeekerPings, hidePings);
						SyncAllSettings();
					}
					else hidePings = options.GetBool(BoolOptionNames__Enum::SeekerPings);

					if (ImGui::InputFloat("Ping Interval", &pingInterval)) {
						options.SetFloat(FloatOptionNames__Enum::MaxPingTime, pingInterval);
						SyncAllSettings();
					}
					else pingInterval = options.GetFloat(FloatOptionNames__Enum::MaxPingTime);

					if (ToggleButton("Show Names", &showNames)) {
						options.SetBool(BoolOptionNames__Enum::ShowCrewmateNames, showNames);
						SyncAllSettings();
					}
					else showNames = options.GetBool(BoolOptionNames__Enum::ShowCrewmateNames);
				}
			}

			ImGui::EndChild();
		}
	}
}