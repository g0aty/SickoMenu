#include "pch-il2cpp.h"
#include "game_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "imgui/imgui.h"
#include "state.hpp"
#include "utility.h"
#include "esp.hpp"

namespace GameTab {
	enum Groups {
		General,
		Settings
	};

	static bool openGeneral = true; //default to general tab group
	static bool openSettings = false;

	void CloseOtherGroups(Groups group) {
		openGeneral = group == Groups::General;
		openSettings = group == Groups::Settings;
	}

	void Render() {
		try {
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::BeginChild("###Game", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
			if (TabGroup("General", openGeneral)) {
				CloseOtherGroups(Groups::General);
			}
			if ((IsInGame() || IsInLobby()) && GameOptions().HasOptions()) {
				ImGui::SameLine();
				if (TabGroup("Settings", openSettings)) {
					CloseOtherGroups(Groups::Settings);
				}
			}
			if (openGeneral) {
				if (SteppedSliderFloat("Player Speed Multiplier", &State.PlayerSpeed, 0.f, 10.f, 0.05f, "%.2fx", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
					State.PrevPlayerSpeed = State.PlayerSpeed;
				}
				if (SteppedSliderFloat("Kill Distance", &State.KillDistance, 0.f, 20.f, 0.1f, "%.1f m", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
					State.PrevKillDistance = State.KillDistance;
				}
				/*if (GameOptions().GetGameMode() == GameModes__Enum::Normal) {
					if (CustomListBoxInt("Task Bar Updates", &State.TaskBarUpdates, TASKBARUPDATES, 225 * State.dpiScale))
						State.PrevTaskBarUpdates = State.TaskBarUpdates;
				}*/
				if (ToggleButton("No Ability Cooldown", &State.NoAbilityCD)) {
					State.Save();
				}
				ImGui::SameLine();
				if (ToggleButton("Multiply Speed", &State.MultiplySpeed)) {
					State.Save();
				}
				ImGui::SameLine();
				if (ToggleButton("Modify Kill Distance", &State.ModifyKillDistance)) {
					State.Save();
				}

				ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
				ImGui::Separator();
				ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

				if ((!IsHost() && State.SafeMode) && ImGui::Button("Set Color") && (IsInGame() || IsInLobby()))
				{
					bool colorAvailable = true;

					for (PlayerControl* player : GetAllPlayerControl())
					{
						app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(GetPlayerData(player));
						if (outfit == NULL) continue;
						if (State.SelectedColorId == outfit->fields.ColorId && !IsHost())
						{
							colorAvailable = false;
							break;
						}
					}

					if (colorAvailable) {
						if (IsInGame())
							State.rpcQueue.push(new RpcSetColor(State.SelectedColorId));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcSetColor(State.SelectedColorId));
					}
				}

				if ((IsHost() || !State.SafeMode) && ImGui::Button("Set Color") && (IsInGame() || IsInLobby()))
				{
					if (IsInGame())
						State.rpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
					else if (IsInLobby())
						State.lobbyRpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
				}

				uint8_t validColorId = 0;
				if ((size_t)State.SelectedColorId < COLORS.size()) {
					validColorId = (uint8_t)State.SelectedColorId;
					State.Save();
				}
				if (IsHost() || !State.SafeMode) {
					ImGui::SameLine(87 * State.dpiScale);
					CustomListBoxInt(" ", &State.SelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);
				}
				else {
					ImGui::SameLine(87 * State.dpiScale);
					if ((size_t)State.SelectedColorId >= COLORS.size()) {
						State.SelectedColorId = (int)validColorId;
						State.Save();
					}
					CustomListBoxInt(" ", &State.SelectedColorId, COLORS, 85.0f * State.dpiScale);
				}

				ImGui::SameLine(215 * State.dpiScale);
				if (ImGui::Button("Random Color"))
				{
					State.SelectedColorId = GetRandomColorId();
				}

				ImGui::SameLine();
				if (ToggleButton("Snipe Color", &State.SnipeColor)) {
					State.Save();
				}

				if (InputStringMultiline("\n\n\n\n\nChat Message", &State.chatMessage)) {
					State.Save();
				}
				if ((IsInGame() || IsInLobby()) && State.ChatCooldown >= 3.f) {
					ImGui::SameLine();
					if (ImGui::Button("Send"))
					{
						auto player = (!State.SafeMode && State.playerToChatAs.has_value()) ?
							State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
						if (IsInGame()) {
							State.rpcQueue.push(new RpcSendChat(player, State.chatMessage));
							State.MessageSent = true;
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcSendChat(player, State.chatMessage));
							State.MessageSent = true;
						}
					}
				}
				if ((IsInGame() || IsInLobby()) && State.ReadAndSendAumChat) ImGui::SameLine();
				if (State.ReadAndSendAumChat && ImGui::Button("Send to AUM"))
				{
					auto player = (!State.SafeMode && State.playerToChatAs.has_value()) ?
						State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
					if (IsInGame()) {
						State.rpcQueue.push(new RpcForceAumChat(PlayerSelection(player), State.chatMessage, true));
					}
					else if (IsInLobby()) {
						State.lobbyRpcQueue.push(new RpcForceAumChat(PlayerSelection(player), State.chatMessage, true));
					}
				}
				
				if (ToggleButton("Spam", &State.ChatSpam))
				{
					State.Save();
				}
				ImGui::SameLine();
				if (!State.SafeMode && ToggleButton("Spam by Everyone", &State.ChatSpamEveryone))
				{
					State.Save();
				}

				if ((IsInGame() || IsInLobby()) && ImGui::Button("Reset Appearance"))
				{
					ControlAppearance(false);
				}
				/*
				if (IsInGame() || IsInLobby())
				{
					if (ImGui::Button("Save Appearance"))
						SaveOriginalAppearance();
				}*/

				ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

				if (ToggleButton("Console", &State.ShowConsole)) {
					State.Save();
				}

				ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

				/*if (IsInGame() && (*Game::pLocalPlayer)->fields.inVent && ImGui::Button("Force Exit Vent"))
				{
					State.rpcQueue.push(new RpcVent(*Game::pLocalPlayer, 1, true));
				}*/

				if (IsInLobby() && ImGui::Button("Allow Everyone to NoClip")) {
					for (auto p : GetAllPlayerControl())
						State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, p, 1, true));
					State.NoClip = true;
					ShowHudNotification("Allowed everyone to NoClip!");
				}
				ImGui::SameLine();
				if ((IsInGame() || IsInLobby()) && ImGui::Button("Kill Everyone")) {
					for (auto player : GetAllPlayerControl()) {
						if (IsInGame())
							State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
					}
				}
				if ((IsInGame() || IsInLobby()) && (IsHost() || !State.SafeMode)) {
					ImGui::SameLine();
					if (ImGui::Button("Protect Everyone")) {
						for (auto player : GetAllPlayerControl()) {
							uint8_t colorId = GetPlayerOutfit(GetPlayerData(player))->fields.ColorId;
							if (IsInGame())
								State.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, PlayerSelection(player), colorId));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, PlayerSelection(player), colorId));
						}
					}
				}

				if (IsInGame() && ToggleButton("Disable Venting", &State.DisableVents)) {
					State.Save();
				}
				if (IsInGame()) ImGui::SameLine();
				if (IsInGame() && ToggleButton("Spam Report", &State.SpamReport)) {
					State.Save();
				}

				if ((IsInGame() || IsInLobby())) {
					if (ImGui::Button("Kill All Crewmates")) {
						for (auto player : GetAllPlayerControl()) {
							if (!PlayerIsImpostor(GetPlayerData(player))) {
								if (IsInGame())
									State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
							}
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Kill All Impostors")) {
						for (auto player : GetAllPlayerControl()) {
							if (PlayerIsImpostor(GetPlayerData(player))) {
								if (IsInGame())
									State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player,
										player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player,
										player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
						}
					}
					if (!State.SafeMode) {
						ImGui::SameLine();
						if (ImGui::Button("Suicide Crewmates")) {
							for (auto player : GetAllPlayerControl()) {
								if (!PlayerIsImpostor(GetPlayerData(player))) {
									if (IsInGame())
										State.rpcQueue.push(new RpcMurderPlayer(player, player,
											player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
									else if (IsInLobby())
										State.lobbyRpcQueue.push(new RpcMurderPlayer(player, player,
											player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								}
							}
						}
						ImGui::SameLine();
						if (ImGui::Button("Suicide Impostors")) {
							for (auto player : GetAllPlayerControl()) {
								if (PlayerIsImpostor(GetPlayerData(player))) {
									if (IsInGame())
										State.rpcQueue.push(new RpcMurderPlayer(player, player,
											player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
									else if (IsInLobby())
										State.lobbyRpcQueue.push(new RpcMurderPlayer(player, player,
											player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								}
							}
						}
					}

					if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && ImGui::Button("Scan Everyone")) {
						for (auto p : GetAllPlayerControl()) {
							if (IsInGame()) State.rpcQueue.push(new RpcForceScanner(p, true));
							else State.lobbyRpcQueue.push(new RpcForceScanner(p, true));
						}
					}
					if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
					if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && ImGui::Button("Stop Scanning Everyone")) {
						for (auto p : GetAllPlayerControl()) {
							if (IsInGame()) State.rpcQueue.push(new RpcForceScanner(p, false));
							else State.lobbyRpcQueue.push(new RpcForceScanner(p, false));
						}
					}
					if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
					if (IsInGame() && ImGui::Button("Kick Everyone From Vents")) {
						State.rpcQueue.push(new RpcBootAllVents());
					}
					if (State.InMeeting) ImGui::SameLine();
					if ((IsHost() || !State.SafeMode) && State.InMeeting && ImGui::Button("End Meeting")) {
						State.rpcQueue.push(new RpcEndMeeting());
						State.InMeeting = false;
					}
				}

				if (!State.SafeMode && ToggleButton("Force Name for Everyone", &State.ForceNameForEveryone)) {
					State.Save();
				}
				if (!State.SafeMode && InputString("Username", &State.hostUserName)) {
					State.Save();
				}

				if (!State.SafeMode) CustomListBoxInt(" ­", &State.HostSelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);

				if (!State.SafeMode && ToggleButton("Force Color for Everyone", &State.ForceColorForEveryone)) {
					State.Save();
				}
			}

			if (openSettings && (IsInGame() || IsInLobby()) && GameOptions().HasOptions())
			{
				GameOptions options;
				std::string hostText = std::format("Host: {}", RemoveHtmlTags(GetHostUsername()));
				ImGui::Text(const_cast<char*>(hostText.c_str()));

				if (options.GetGameMode() == GameModes__Enum::Normal)
				{
					auto allPlayers = GetAllPlayerControl();
					RoleRates roleRates = RoleRates(options, (int)allPlayers.size());
					// this should be all the major ones. if people want more they're simple enough to add.
					ImGui::Text("Visual Tasks: %s", (options.GetBool(app::BoolOptionNames__Enum::VisualTasks) ? "On" : "Off"));
					switch (options.GetInt(app::Int32OptionNames__Enum::TaskBarMode)) {
					case 0:
						ImGui::Text("Task Bar Updates: Always");
						break;
					case 1:
						ImGui::Text("Task Bar Updates: Meetings");
						break;
					case 2:
						ImGui::Text("Task Bar Updates: Never");
						break;
					default:
						ImGui::Text("Task Bar Updates: Other");
						break;
					}
					ImGui::Text("Confirm Ejects: %s", (options.GetBool(app::BoolOptionNames__Enum::ConfirmImpostor) ? "On" : "Off"));
					switch (options.GetInt(app::Int32OptionNames__Enum::KillDistance)) {
					case 0:
						ImGui::Text("Kill Distance: Short");
						break;
					case 1:
						ImGui::Text("Kill Distance: Medium");
						break;
					case 2:
						ImGui::Text("Kill Distance: Long");
						break;
					default:
						ImGui::Text("Kill Distance: Other");
						break;
					}

					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

					ImGui::Text("Max Engineers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Engineer));
					ImGui::Text("Engineer Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Engineer));
					ImGui::Text("Engineer Vent Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EngineerCooldown, 1.0F));
					ImGui::Text("Engineer Duration in Vent: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime, 1.0F));

					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

					ImGui::Text("Max Scientists: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Scientist));
					ImGui::Text("Scientist Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Scientist));
					ImGui::Text("Scientist Vitals Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ScientistCooldown, 1.0F));
					ImGui::Text("Scientist Battery Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ScientistBatteryCharge, 1.0F));

					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

					ImGui::Text("Max Guardian Angels: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::GuardianAngel));
					ImGui::Text("Guardian Angel Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::GuardianAngel));
					ImGui::Text("Guardian Angel Protect Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::GuardianAngelCooldown, 1.0F));
					ImGui::Text("Guardian Angel Protection Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, 1.0F));

					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

					ImGui::Text("Max Shapeshifters: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Shapeshifter));
					ImGui::Text("Shapeshifter Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Shapeshifter));
					ImGui::Text("Shapeshifter Shift Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, 1.0F));
					ImGui::Text("Shapeshifter Shift Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration, 1.0F));
				}
				else if (options.GetGameMode() == GameModes__Enum::HideNSeek) {

					int ImpostorId = options.GetInt(app::Int32OptionNames__Enum::ImpostorPlayerID);
					if (ImpostorId < 0) {
						ImGui::Text("Impostor: Round-robin");
					}
					else {
						std::string ImpostorName = std::format("Selected Impostor: {}", convert_from_string(GameData_PlayerOutfit_get_PlayerName(GetPlayerOutfit(GetPlayerDataById(ImpostorId)), nullptr)));
						ImGui::Text(const_cast<char*>(ImpostorName.c_str()));
					}
					ImGui::Text("Flashlight Mode: %s", (options.GetBool(app::BoolOptionNames__Enum::UseFlashlight) ? "On" : "Off"));

					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

					ImGui::Text("Vent Uses: %d", options.GetInt(app::Int32OptionNames__Enum::CrewmateVentUses));
					ImGui::Text("Duration in Vent: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::CrewmateTimeInVent, 1.0F));

					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
					ImGui::Separator();
					ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

					ImGui::Text("Hiding Time: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EscapeTime, 1.0F));
					ImGui::Text("Final Hiding Time: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::FinalEscapeTime, 1.0F));
					ImGui::Text("Final Impostor Speed: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::SeekerFinalSpeed, 1.0F));
				}
			}
			else {
				CloseOtherGroups(Groups::General);
			}
			ImGui::EndChild();
		}
		catch (...) {}
	}
}
