#include "pch-il2cpp.h"
#include "players_tab.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"

namespace PlayersTab {

	int framesPassed = -1;
	Vector2 previousPlayerPosition;

	enum Groups {
		Player,
		Trolling,
		PUID,
		Detection,
	};

	static bool openPlayer = true; //default to visual tab group
	static bool openTrolling = false;
	static bool openPUID = false;
	static bool openDetection = false;

	void CloseOtherGroups(Groups group) {
		openPlayer = group == Groups::Player;
		openTrolling = group == Groups::Trolling;
		openPUID = group == Groups::PUID;
		openDetection = group == Groups::Detection;
	}

	static bool murderLoop = false;
	static bool suicideLoop = false;
	static bool farmLoop = false;
	static int murderCount = 0;
	static int murderDelay = 0;
	static int suicideCount = 0;
	static int suicideDelay = 0;
	static int farmCount = 0;
	static int farmDelay = 0;

	void Render() {
		if ((IsInGame() || IsInLobby()) && !State.BlinkPlayersTab) {
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::BeginChild("players#list", ImVec2(200, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
			auto selectedPlayer = State.selectedPlayer.validate();
			bool shouldEndListBox = ImGui::ListBoxHeader("###players#list", ImVec2(200, 230) * State.dpiScale);
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			std::vector<PlayerSelection> selectedPlayers = {};
			for (auto id : State.selectedPlayers) {
				selectedPlayers.push_back(PlayerSelection(GetPlayerControlById(id)));
			}
			for (auto playerCtrl : GetAllPlayerControl()) {
				const auto& player = PlayerSelection(playerCtrl);
				const auto& validPlayer = PlayerSelection(playerCtrl).validate();
				if (!validPlayer.has_value())
					continue;
				auto playerData = GetPlayerData(playerCtrl);
				if (playerData->fields.Disconnected)
					continue;
				app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(playerData);
				if (outfit == NULL) continue;
				std::string playerName = RemoveHtmlTags(convert_from_string(NetworkedPlayerInfo_get_PlayerName(playerData, nullptr)));
				std::string playerFc = convert_from_string(playerData->fields.FriendCode);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0) * State.dpiScale);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0) * State.dpiScale);
				bool isSelected = std::find(State.selectedPlayers.begin(), State.selectedPlayers.end(), player.get_PlayerId()) != State.selectedPlayers.end();
				if (ImGui::Selectable(std::string("##" + ToString(playerData->fields.PlayerId)).c_str(), isSelected)) { //fix selection problems with multiple ppl having same name
					bool isShifted = ImGui::IsKeyPressed(VK_SHIFT) || ImGui::IsKeyDown(VK_SHIFT);
					bool isCtrl = ImGui::IsKeyDown(0x11) || ImGui::IsKeyDown(0xA2) || ImGui::IsKeyDown(0xA3);
					if (isCtrl) {
						if (isShifted && ImGui::IsKeyDown(0x41)) {
							State.selectedPlayers = {};
						}
						else if (ImGui::IsKeyDown(0x41)) {
							State.selectedPlayers = {};
							for (auto p : GetAllPlayerControl()) {
								State.selectedPlayers.push_back(p->fields.PlayerId);
							}
						}
						auto it = std::find(State.selectedPlayers.begin(), State.selectedPlayers.end(), player.get_PlayerId());
						if (it != State.selectedPlayers.end()) {
							State.selectedPlayers.erase(it);
							if (State.selectedPlayers.size() == 0) {
								State.selectedPlayer = {};
								selectedPlayers = {};
								selectedPlayer = State.selectedPlayer.validate();
							}
						}
						else {
							if (std::find(State.selectedPlayers.begin(), State.selectedPlayers.end(), player.get_PlayerId()) == State.selectedPlayers.end())
								State.selectedPlayers.push_back(player.get_PlayerId());
							State.selectedPlayer = validPlayer;
							selectedPlayer = validPlayer;
						}
					}
					else {
						State.selectedPlayer = validPlayer;
						selectedPlayer = validPlayer;
						State.selectedPlayers = { player.get_PlayerId() };
					}
				}
				ImGui::SameLine();
				auto playerColor = AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId));
				playerColor.w = State.MenuThemeColor.w;
				ImGui::ColorButton(std::string("##" + playerName + "_ColorButton").c_str(), playerColor, ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip);
				ImGui::SameLine();
				ImGui::PopStyleVar(2);
				ImGui::Dummy(ImVec2(0, 0) * State.dpiScale);
				ImGui::SameLine();

				ImVec4 nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->White);
				if (std::find(State.BlacklistFriendCodes.begin(), State.BlacklistFriendCodes.end(), playerFc) != State.BlacklistFriendCodes.end()) {
					playerName = "[-] " + playerName;
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->ImpostorRed);
				}
				else if (std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), playerFc) != State.WhitelistFriendCodes.end()) {
					playerName = "[+] " + playerName;
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->CrewmateBlue);
				}
				if (State.RevealRoles && IsInGame())
				{
					std::string roleName = GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames);
					playerName = playerName + " (" + roleName + ")";
					nameColor = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
				}
				else if (PlayerIsImpostor(localData) && PlayerIsImpostor(playerData))
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->ImpostorRoleRed);
				else if (playerCtrl == *Game::pLocalPlayer || State.modUsers.find(playerData->fields.PlayerId) != State.modUsers.end()) {
					if (playerCtrl == *Game::pLocalPlayer || State.modUsers.at(playerData->fields.PlayerId) == "<#f00>KillNetwork</color>")
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->ImpostorRed);

					if (playerCtrl == *Game::pLocalPlayer || State.modUsers.at(playerData->fields.PlayerId) == "<#5f5>BetterAmongUs</color>")
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->LogSuccessColor);

					if (playerCtrl == *Game::pLocalPlayer || State.modUsers.at(playerData->fields.PlayerId) == "<#f55>AmongUsMenu</color>")
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->Orange);

					if (playerCtrl == *Game::pLocalPlayer || State.modUsers.at(playerData->fields.PlayerId) == "<#0f0>Sicko</color><#f00>Menu</color>")
						nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->AcceptedGreen);
				}

				if (playerData->fields.IsDead)
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->DisabledGrey);

				ImGui::TextColored(nameColor, playerName.c_str());
			}
			if (shouldEndListBox)
				ImGui::ListBoxFooter();

			if (selectedPlayer.has_value() && !selectedPlayer.is_Disconnected() && selectedPlayers.size() == 1 && !selectedPlayers[0].validate().is_Disconnected()) //Upon first startup no player is selected.  Also rare case where the playerdata is deleted before the next gui cycle
			{
				if ((IsInMultiplayerGame() || IsInLobby()) || (selectedPlayer.has_value() && selectedPlayer.is_LocalPlayer())) {
					bool isUsingMod = selectedPlayer.is_LocalPlayer() || State.modUsers.find(selectedPlayer.get_PlayerData()->fields.PlayerId) != State.modUsers.end();
					ImGui::Text("Is using Modified Client: %s", isUsingMod ? "Yes" : "No");
					if (isUsingMod)
						ImGui::Text("Client Name: %s", selectedPlayer.is_LocalPlayer() ? "SickoMenu" : RemoveHtmlTags(State.modUsers.at(selectedPlayer.get_PlayerData()->fields.PlayerId)).c_str());
					std::uint8_t playerId = selectedPlayer.get_PlayerData()->fields.PlayerId;
					std::string playerIdText = std::format("Player ID: {}", playerId);
					ImGui::Text(const_cast<char*>(playerIdText.c_str()));
					std::string friendCode = convert_from_string(selectedPlayer.get_PlayerData()->fields.FriendCode);
					std::string friendCodeText = std::format("Friend Code: {}", (!IsStreamerMode()) ? friendCode : ((friendCode != "") ? friendCode.substr(0, 1) + "..." : ""));
					if (friendCode != "") {
						ImGui::Text(const_cast<char*>(friendCodeText.c_str()));
					}
					std::string puid = convert_from_string(selectedPlayer.get_PlayerData()->fields.Puid);
					std::string puidText = std::format("PUID:\n{}", (!IsStreamerMode()) ? puid : ((puid != "") ? puid.substr(0, 1) + "..." : ""));
					if (puid != "") {
						ImGui::Text(const_cast<char*>(puidText.c_str()));
					}
					uint32_t playerLevel = selectedPlayer.get_PlayerData()->fields.PlayerLevel + 1;
					std::string levelText = std::format("Level: {}", playerLevel);
					ImGui::Text(const_cast<char*>(levelText.c_str()));
					std::string platform = "Unknown";
					auto client = app::InnerNetClient_GetClientFromCharacter((InnerNetClient*)(*Game::pAmongUsClient), selectedPlayer.get_PlayerControl(), NULL);
					if (GetPlayerControlById(selectedPlayer.get_PlayerData()->fields.PlayerId)->fields._.OwnerId == client->fields.Id) {
						switch (client->fields.PlatformData->fields.Platform) {
						case Platforms__Enum::StandaloneEpicPC:
							platform = "Epic Games (PC)";
							break;
						case Platforms__Enum::StandaloneSteamPC:
							platform = "Steam (PC)";
							break;
						case Platforms__Enum::StandaloneMac:
							platform = "Mac";
							break;
						case Platforms__Enum::StandaloneWin10:
							platform = "Microsoft Store (PC)";
							break;
						case Platforms__Enum::StandaloneItch:
							platform = "itch.io (PC)";
							break;
						case Platforms__Enum::IPhone:
							platform = "iOS/iPadOS (Mobile)";
							break;
						case Platforms__Enum::Android:
							platform = "Android (Mobile)";
							break;
						case Platforms__Enum::Switch:
							platform = "Nintendo Switch (Console)";
							break;
						case Platforms__Enum::Xbox:
							platform = "Xbox (Console)";
							break;
						case Platforms__Enum::Playstation:
							platform = "Playstation (Console)";
							break;
						default:
							platform = "Unknown";
							break;
						}
					}
					std::string platformText = std::format("Platform: {}", platform);
					ImGui::Text(platformText.c_str());
					uint64_t psnId = client->fields.PlatformData->fields.PsnPlatformId;
					std::string psnText = std::format("PSN Platform ID: {}", psnId);
					if (psnId != 0) ImGui::Text(const_cast<char*>(psnText.c_str()));
					uint64_t xboxId = client->fields.PlatformData->fields.XboxPlatformId;
					std::string xboxText = std::format("Xbox Platform ID: {}", xboxId);
					if (xboxId != 0) ImGui::Text(const_cast<char*>(xboxText.c_str()));
				}
				else {
					ImGui::Text("Is using Modified Client: No");
					std::uint8_t playerId = selectedPlayer.get_PlayerData()->fields.PlayerId;
					std::string playerIdText = std::format("Player ID: {}", playerId);
					ImGui::Text(const_cast<char*>(playerIdText.c_str()));
					uint32_t playerLevel = selectedPlayer.get_PlayerData()->fields.PlayerLevel + 1;
					std::string levelText = std::format("Level: {}", playerLevel);
					ImGui::Text(const_cast<char*>(levelText.c_str()));
				}
			}

			ImGui::EndChild();
			ImGui::SameLine();
			ImGui::BeginChild("players#actions", ImVec2(300, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
			if (selectedPlayer.has_value()) {
				if (TabGroup("Player", openPlayer)) {
					CloseOtherGroups(Groups::Player);
				}
				ImGui::SameLine();
				if (TabGroup("Trolling", openTrolling)) {
					CloseOtherGroups(Groups::Trolling);
				}
				ImGui::SameLine();
				if (TabGroup("PUID", openPUID)) {
					CloseOtherGroups(Groups::PUID);
				}
				ImGui::SameLine();
				if (TabGroup("Detection", openDetection)) {
					CloseOtherGroups(Groups::Detection);
				}
			}
			if (State.DisableMeetings && IsHost())
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Meetings have been disabled.");
			GameOptions options;
			if (IsInGame() && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && (!State.DisableMeetings || !IsHost())) { //Player selection doesn't matter
				if (!State.InMeeting) {
					if (ImGui::Button("Call Meeting")) {
						RepairSabotage(*Game::pLocalPlayer);
						State.rpcQueue.push(new RpcReportBody({}));
					}
				}
				else {
					if (ImGui::Button("Call Meeting")) {
						RepairSabotage(*Game::pLocalPlayer);
						State.rpcQueue.push(new RpcForceMeeting(*Game::pLocalPlayer, {}));
					}
				}
			}
			if ((IsHost() || !State.SafeMode) && State.InMeeting && ImGui::Button("Skip Vote by All")) {
				Game::PlayerId VoteOffPlayerId = Game::SkippedVote;
				for (auto player : GetAllPlayerControl()) {
					/*if (player != selectedPlayer.get_PlayerControl()) {
						State.rpcQueue.push(new RpcClearVote(player));
					}*/
					State.rpcQueue.push(new RpcVotePlayer(player, player, true));
				}
				State.InMeeting = false;
			}
			if (openPlayer && selectedPlayer.has_value())
			{
				if (IsInGame() && !State.DisableMeetings && selectedPlayers.size() == 1) {
					ImGui::NewLine();
					if (!State.InMeeting) {
						if (ImGui::Button("Report Body") && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead) {
							State.rpcQueue.push(new RpcReportBody(State.selectedPlayer));
						}
					}
					else {
						if (ImGui::Button("Report Body") && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead) {
							State.rpcQueue.push(new RpcForceMeeting(*Game::pLocalPlayer, State.selectedPlayer));
						}
					}
				}

				if (!selectedPlayer.is_Disconnected() && selectedPlayers.size() == 1)
				{
					if (State.playerToFollow.equals(State.selectedPlayer) || (selectedPlayer.is_LocalPlayer() && selectedPlayer.has_value())) {
						if (ImGui::Button("Stop Spectating")) {
							State.playerToFollow = {};
						}
					}
					else {
						if (!selectedPlayer.is_LocalPlayer() && ImGui::Button("Spectate")) {
							State.FreeCam = false;
							State.playerToFollow = State.selectedPlayer;
						}
					}
				}

				if (IsInGame() && PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))
					&& !selectedPlayer.get_PlayerData()->fields.IsDead
					&& !selectedPlayer.get_PlayerControl()->fields.inVent
					&& !selectedPlayer.get_PlayerControl()->fields.inMovingPlat
					&& !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && ((*Game::pLocalPlayer)->fields.killTimer <= 0.0f)
					&& selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0
					&& !State.InMeeting)
				{
					if (ImGui::Button("Kill"))
					{
						State.rpcQueue.push(new CmdCheckMurder(State.selectedPlayer));
					}
				}
				else if (IsHost() || !State.SafeMode) {
					if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && ImGui::Button("Kill"))
					{
						for (PlayerSelection p : selectedPlayers) {
							auto validPlayer = p.validate();
							if (IsInGame()) {
								State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, validPlayer.get_PlayerControl(),
									validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), validPlayer.get_PlayerControl(),
									validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
						}
					}
				}
				if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))
					&& !selectedPlayer.get_PlayerData()->fields.IsDead
					&& !selectedPlayer.get_PlayerControl()->fields.inVent
					&& !selectedPlayer.get_PlayerControl()->fields.inMovingPlat
					&& !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && ((*Game::pLocalPlayer)->fields.killTimer <= 0.0f)
					&& selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0
					&& !State.InMeeting)
				{
					ImGui::SameLine();
					if (ImGui::Button("Telekill"))
					{
						previousPlayerPosition = GetTrueAdjustedPosition(*Game::pLocalPlayer);
						for (auto p : selectedPlayers)
							State.rpcQueue.push(new CmdCheckMurder(p));
						framesPassed = 40;
					}
				}
				else if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && (IsHost() || !State.SafeMode)) {
					ImGui::SameLine();
					if (ImGui::Button("Telekill"))
					{
						previousPlayerPosition = GetTrueAdjustedPosition(*Game::pLocalPlayer);
						for (auto p : selectedPlayers) {
							auto validPlayer = p.validate();
							if (IsInGame()) {
								State.rpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), validPlayer.get_PlayerControl(),
									validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), validPlayer.get_PlayerControl(),
									validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
						}
						framesPassed = 40;
					}
				}

				if ((IsInMultiplayerGame() || IsInLobby()) && (!selectedPlayer.is_LocalPlayer() || selectedPlayers.size() != 1)) {
					if (IsHost() && ImGui::Button("Kick")) {
						State.selectedPlayer = {};
						State.selectedPlayers.clear();
						for (auto p : selectedPlayers) {
							if (p.has_value() && p.validate().get_PlayerControl() != *Game::pLocalPlayer)
								app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), p.validate().get_PlayerControl()->fields._.OwnerId, false, NULL);
						}
					}
					if (ImGui::Button("Votekick")) {
						if (IsHost()) {
							State.selectedPlayer = {};
							State.selectedPlayers.clear();
						}
						for (auto p : selectedPlayers) {
							if (p.has_value() && p.validate().is_LocalPlayer()) continue;
							if (IsInGame()) {
								State.rpcQueue.push(new RpcVoteKick(p.validate().get_PlayerControl()));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcVoteKick(p.validate().get_PlayerControl()));
							}
						}
					}
					if (!State.SafeMode) {
						if (ImGui::Button("Attempt to Kick")) {
							State.selectedPlayer = {};
							State.selectedPlayers.clear();
							for (auto p : selectedPlayers) {
								if (p.has_value() && p.validate().is_LocalPlayer()) continue;
								if (IsInGame()) {
									State.rpcQueue.push(new RpcVoteKick(p.validate().get_PlayerControl(), true));
								}
								else if (IsInLobby()) {
									State.lobbyRpcQueue.push(new RpcVoteKick(p.validate().get_PlayerControl(), true));
								}
							}
						}
					}
					/*else if (IsInGame()) {
						if (ImGui::Button("Attempt to Ban")) {
							for (auto p : selectedPlayers) {
								if (p.has_value() && p.validate().is_LocalPlayer()) continue;
								if (IsInGame()) {
									State.rpcQueue.push(new RpcSpamMeeting(*Game::pLocalPlayer, p.validate().get_PlayerControl(), State.InMeeting));
								}
								else if (IsInLobby()) {
									State.lobbyRpcQueue.push(new RpcSpamMeeting(*Game::pLocalPlayer, p.validate().get_PlayerControl(), State.InMeeting));
								}
							}
						}
					}*/
					
					if (IsHost() && ImGui::Button("Ban")) {
						State.selectedPlayer = {};
						State.selectedPlayers.clear();
						for (auto p : selectedPlayers) {
							if (p.has_value() && p.validate().is_LocalPlayer()) continue;
							app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), p.validate().get_PlayerControl()->fields._.OwnerId, true, NULL);
						}
					}
					
					if (selectedPlayers.size() == 1 && !selectedPlayers[0].validate().is_LocalPlayer()) {
						Game::PlayerId playerId = selectedPlayers[0].validate().get_PlayerControl()->fields.PlayerId;
						std::string friendCode = convert_from_string(selectedPlayers[0].validate().get_PlayerData()->fields.FriendCode);
						if (std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), friendCode) == State.WhitelistFriendCodes.end()) {
							if (std::find(State.BlacklistFriendCodes.begin(), State.BlacklistFriendCodes.end(), friendCode) != State.BlacklistFriendCodes.end()) {
								if (ImGui::Button("Remove from Blacklist")) {
									State.BlacklistFriendCodes.erase(std::find(State.BlacklistFriendCodes.begin(), State.BlacklistFriendCodes.end(), friendCode));
									State.Save();
								}
							}
							else if (ImGui::Button("Add to Blacklist")) {
								State.BlacklistFriendCodes.push_back(friendCode);
								State.Save();
							}
						}
						if (std::find(State.BlacklistFriendCodes.begin(), State.BlacklistFriendCodes.end(), friendCode) == State.BlacklistFriendCodes.end()) {
							if (std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), friendCode) != State.WhitelistFriendCodes.end()) {
								if (ImGui::Button("Remove from Whitelist")) {
									State.WhitelistFriendCodes.erase(std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), friendCode));
									State.Save();
								}
							}
							else if (ImGui::Button("Add to Whitelist")) {
								State.WhitelistFriendCodes.push_back(friendCode);
								State.Save();
							}
						}
					}
				}

				if (framesPassed == 0)
				{
					if (IsInGame())
						State.rpcQueue.push(new RpcSnapTo(previousPlayerPosition));
					else if (IsInLobby())
						State.lobbyRpcQueue.push(new RpcSnapTo(previousPlayerPosition));
					framesPassed--;
				}
				else framesPassed--;

				app::RoleBehaviour* playerRole = localData->fields.Role;
				app::RoleTypes__Enum role = playerRole != nullptr ? playerRole->fields.Role : app::RoleTypes__Enum::Crewmate;

				if (!State.SafeMode)
				{
					if (selectedPlayers.size() == 1 && ImGui::Button("Shift"))
					{
						if (IsInGame())
							State.rpcQueue.push(new RpcShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
					}
				}
				else if (State.RealRole == RoleTypes__Enum::Shapeshifter && role == RoleTypes__Enum::Shapeshifter) {
					app::ShapeshifterRole* shapeshifterRole = (app::ShapeshifterRole*)playerRole;
					if (selectedPlayers.size() == 1 && shapeshifterRole->fields.cooldownSecondsRemaining <= 0 && ImGui::Button("Shift"))
					{
						if (IsInGame())
							State.rpcQueue.push(new CmdCheckShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new CmdCheckShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
					}
				}

				if (State.RealRole == RoleTypes__Enum::GuardianAngel && role == RoleTypes__Enum::GuardianAngel) {
					app::GuardianAngelRole* guardianAngelRole = (app::GuardianAngelRole*)playerRole;
					if (selectedPlayers.size() == 1 && guardianAngelRole->fields.cooldownSecondsRemaining <= 0 && ImGui::Button("Protect")) {
						if (IsInGame())
							State.rpcQueue.push(new CmdCheckProtect(*Game::pLocalPlayer, State.selectedPlayer));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new CmdCheckProtect(*Game::pLocalPlayer, State.selectedPlayer));
					}
				}
				else {
					if (ImGui::Button("Protect")) {
						for (auto p : selectedPlayers) {
							app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(p.validate().get_PlayerData());
							auto colorId = outfit->fields.ColorId;
							if (IsInGame())
								State.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, p, colorId));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, p, colorId));
						}
					}
				}

				if (IsInGame() && !selectedPlayer.is_Disconnected() && (IsInMultiplayerGame() || selectedPlayer.is_LocalPlayer()))
				{
					if ((!State.SafeMode || (selectedPlayer.is_LocalPlayer() && selectedPlayers.size() == 1)) && ImGui::Button("Complete all Tasks")) {
						if (State.SafeMode) {
							auto allTasks = GetNormalPlayerTasks(*Game::pLocalPlayer);
							for (auto t : allTasks) {
								if (t->fields.taskStep != t->fields.MaxStep) State.rpcQueue.push(new RpcForceCompleteTask(*Game::pLocalPlayer, t->fields._._Id_k__BackingField));
							}
						}
						else {
							for (auto p : selectedPlayers) {
								auto allTasks = GetNormalPlayerTasks(p.validate().get_PlayerControl());
								for (auto t : allTasks) {
									if (t->fields.taskStep != t->fields.MaxStep) State.rpcQueue.push(new RpcForceCompleteTask(p.validate().get_PlayerControl(), t->fields._._Id_k__BackingField));
								}
							}
						}
					}

					if (selectedPlayers.size() == 1) {
						auto tasks = GetNormalPlayerTasks(selectedPlayer.get_PlayerControl());

						if (State.RevealRoles && PlayerIsImpostor(selectedPlayer.get_PlayerData()))
						{
							ImGui::TextColored(ImVec4(0.8F, 0.2F, 0.0F, 1.0F), "Fake Tasks:");
						}
						else
						{
							ImGui::Text("Tasks:");
						}

						bool shouldEndListBox = ImGui::ListBoxHeader("###tasks#list"/*, ImVec2(181, 94) * State.dpiScale*/);

						if (selectedPlayer.get_PlayerControl()->fields.myTasks == nullptr)
						{
							ImGui::Text("ERROR: Could not load tasks.");
						}
						else
						{
							for (auto task : tasks)
							{
								if (task->fields.taskStep == task->fields.MaxStep)
									ImGui::TextColored(ImVec4(0.0F, 1.0F, 0.0F, 1.0F), (std::string(TranslateTaskTypes(task->fields._.TaskType))).c_str());
								else {
									if ((!State.SafeMode || selectedPlayer.is_LocalPlayer())) {
										if (ImGui::Button((std::string(TranslateTaskTypes(task->fields._.TaskType))).c_str()))
											State.rpcQueue.push(new RpcForceCompleteTask(selectedPlayer.get_PlayerControl(), task->fields._._Id_k__BackingField));
									}
									else ImGui::Text((std::string(TranslateTaskTypes(task->fields._.TaskType))).c_str());
								}
							}
						}
						if (shouldEndListBox)
							ImGui::ListBoxFooter();
					}
				}
			}

			if (openTrolling && selectedPlayer.has_value()) {
				if (ImGui::Button("Send Blank Chat As")) {
					for (auto p : selectedPlayers) {
						if (IsInGame()) State.rpcQueue.push(new RpcSendChatNote(p.validate().get_PlayerControl(), 1));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSendChatNote(p.validate().get_PlayerControl(), 1));
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Spam Blank Chat As")) {
					for (auto p : selectedPlayers) {
						if (IsInGame()) State.rpcQueue.push(new RpcSpamChatNote(p.validate().get_PlayerControl()));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSpamChatNote(p.validate().get_PlayerControl()));
					}
				}

				if (IsInGame() && !GetPlayerData(selectedPlayer.get_PlayerControl())->fields.IsDead && selectedPlayers.size() == 1) {
					if (!State.InMeeting) {
						if (ImGui::Button("Force Meeting By")) {
							if (IsHost() || !State.SafeMode) State.rpcQueue.push(new RpcForceReportBody(selectedPlayer.get_PlayerControl(), {}));
							else {
								State.rpcQueue.push(new RpcReportBody(selectedPlayer));
								State.rpcQueue.push(new RpcForceMeeting(selectedPlayer.get_PlayerControl(), {}));
							}
						}
					}
					else {
						if (ImGui::Button("Force Meeting By")) {
							State.rpcQueue.push(new RpcForceMeeting(selectedPlayer.get_PlayerControl(), {}));
						}
					}
				}

				if (selectedPlayer.has_value() && IsInGame() && !selectedPlayer.get_PlayerData()->fields.IsDead && selectedPlayers.size() == 1) {
					ImGui::SameLine();
					if (!State.InMeeting) {
						if (ImGui::Button("Self-Report")) {
							if (IsHost() || !State.SafeMode) State.rpcQueue.push(new RpcForceReportBody(selectedPlayer.get_PlayerControl(), selectedPlayer));
							else {
								State.rpcQueue.push(new RpcReportBody(selectedPlayer));
								State.rpcQueue.push(new RpcForceMeeting(selectedPlayer.get_PlayerControl(), selectedPlayer));
							}
						}
					}
					else {
						if (ImGui::Button("Self-Report")) {
							State.rpcQueue.push(new RpcForceMeeting(selectedPlayer.get_PlayerControl(), State.selectedPlayer));
						}
					}
				}

				if (!selectedPlayer.is_LocalPlayer() && selectedPlayers.size() == 1) {
					app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
					if (outfit != NULL) {
						auto petId = outfit->fields.PetId;
						auto skinId = outfit->fields.SkinId;
						auto hatId = outfit->fields.HatId;
						auto visorId = outfit->fields.VisorId;
						auto colorId = outfit->fields.ColorId;
						auto namePlateId = outfit->fields.NamePlateId;
						std::queue<RPCInterface*>* queue = nullptr;
						if (IsInGame())
							queue = &State.rpcQueue;
						else if (IsInLobby())
							queue = &State.lobbyRpcQueue;

						if (!State.activeImpersonation && ImGui::Button("Impersonate")) {
							if (queue != nullptr) {
								if (IsHost() || !State.SafeMode)
									queue->push(new RpcForceColor(*Game::pLocalPlayer, colorId));
								else
									queue->push(new RpcSetColor(GetRandomColorId()));
								queue->push(new RpcSetPet(petId));
								queue->push(new RpcSetSkin(skinId));
								queue->push(new RpcSetVisor(visorId));
								queue->push(new RpcSetHat(hatId));
								queue->push(new RpcSetNamePlate(namePlateId));
								ImpersonateName(selectedPlayer.get_PlayerData());
								queue->push(new RpcSetLevel(*Game::pLocalPlayer, selectedPlayer.get_PlayerData()->fields.PlayerLevel));
								State.activeImpersonation = true;
							}
						}
						if (ImGui::CollapsingHeader("Cosmetics Stealer")) {
							if (ImGui::Button("Name"))
								ImpersonateName(selectedPlayer.get_PlayerData());
							ImGui::SameLine();
							if ((IsHost() || !State.SafeMode) && ImGui::Button("Color") && queue != nullptr)
								queue->push(new RpcForceColor(*Game::pLocalPlayer, colorId));
							ImGui::SameLine();
							if (ImGui::Button("Hat") && queue != nullptr)
								queue->push(new RpcSetHat(hatId));
							ImGui::SameLine();
							if (ImGui::Button("Skin") && queue != nullptr)
								queue->push(new RpcSetSkin(skinId));

							if (ImGui::Button("Visor") && queue != nullptr)
								queue->push(new RpcSetVisor(visorId));
							ImGui::SameLine();
							if (ImGui::Button("Pet") && queue != nullptr)
								queue->push(new RpcSetPet(petId));
							ImGui::SameLine();
							if (ImGui::Button("Nameplate") && queue != nullptr)
								queue->push(new RpcSetNamePlate(namePlateId));
						}

						if (ImGui::CollapsingHeader("Cosmetics Resetter")) {
							ResetOriginalAppearance();
							if (ImGui::Button("Name") && queue != nullptr)
								queue->push(new RpcSetName(State.originalName));
							ImGui::SameLine();
							if (ImGui::Button("Color") && queue != nullptr) {
								if (IsHost() || !State.SafeMode) queue->push(new RpcForceColor(*Game::pLocalPlayer, State.originalColor));
								else queue->push(new RpcSetColor(State.originalColor));
							}
							ImGui::SameLine();
							if (ImGui::Button("Hat") && queue != nullptr)
								queue->push(new RpcSetHat(State.originalHat));
							ImGui::SameLine();
							if (ImGui::Button("Skin") && queue != nullptr)
								queue->push(new RpcSetSkin(State.originalSkin));

							if (ImGui::Button("Visor") && queue != nullptr)
								queue->push(new RpcSetVisor(State.originalVisor));
							ImGui::SameLine();
							if (ImGui::Button("Pet") && queue != nullptr)
								queue->push(new RpcSetPet(State.originalNamePlate));
							ImGui::SameLine();
							if (ImGui::Button("Nameplate") && queue != nullptr)
								queue->push(new RpcSetNamePlate(State.originalNamePlate));
						}
					}
				}

				if (!State.SafeMode && ImGui::Button("Impersonate Everyone To") && selectedPlayers.size() == 1) {
					app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
					auto petId = outfit->fields.PetId;
					auto skinId = outfit->fields.SkinId;
					auto hatId = outfit->fields.HatId;
					auto visorId = outfit->fields.VisorId;
					auto colorId = outfit->fields.ColorId;
					auto namePlateId = outfit->fields.NamePlateId;
					std::queue<RPCInterface*>* queue = nullptr;
					if (IsInGame())
						queue = &State.rpcQueue;
					else if (IsInLobby())
						queue = &State.lobbyRpcQueue;
					if (queue != nullptr) {
						for (auto p : GetAllPlayerControl()) {
							if (p == selectedPlayer.get_PlayerControl()) continue; //preserve the original player
							std::string name = convert_from_string(NetworkedPlayerInfo_get_PlayerName(selectedPlayer.get_PlayerData(), NULL))
								+ std::format("<size=0>{}</size>", p->fields.PlayerId);
							queue->push(new RpcForceColor(p, GetRandomColorId()));
							queue->push(new RpcForcePet(p, petId));
							queue->push(new RpcForceSkin(p, skinId));
							queue->push(new RpcForceVisor(p, visorId));
							queue->push(new RpcForceHat(p, hatId));
							queue->push(new RpcForceNamePlate(p, namePlateId));
							queue->push(new RpcForceName(p, name));
							queue->push(new RpcSetLevel(p, selectedPlayer.get_PlayerData()->fields.PlayerLevel));
							State.activeImpersonation = true;
						}
					}
				}

				if (State.activeImpersonation)
				{
					if (ImGui::Button("Reset Impersonation"))
					{
						ControlAppearance(false);
					}
				}

				if ((IsHost() || !State.SafeMode) && IsInLobby() && ImGui::Button(selectedPlayers.size() == 1 ? "Allow Player to NoClip" : "Allow Players to NoClip")) {
					for (auto p : selectedPlayers) {
						if (p.has_value() && p.validate().is_LocalPlayer()) State.NoClip = true;
						else State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, p.validate().get_PlayerControl(), 1, true));
						if (selectedPlayers.size() == 1) {
							ShowHudNotification(std::format("Allowed {} to NoClip!",
								convert_from_string(NetworkedPlayerInfo_get_PlayerName(p.validate().get_PlayerData(), NULL))));
						}
						else {
							ShowHudNotification(std::format("Allowed {} players to NoClip!", selectedPlayers.size()));
						}
					}
				}

				if (!State.SafeMode) {
					if (ImGui::Button("Suicide")) {
						for (auto p : selectedPlayers) {
							auto validPlayer = p.validate();
							if (IsInGame()) {
								State.rpcQueue.push(new RpcMurderPlayer(validPlayer.get_PlayerControl(), validPlayer.get_PlayerControl(),
									validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcMurderPlayer(validPlayer.get_PlayerControl(), validPlayer.get_PlayerControl(),
									validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
						}
					}
				}

				if (IsHost() || !State.SafeMode) {
					if ((IsInGame() || (IsInLobby() && State.KillInLobbies))) {
						if (!murderLoop && ImGui::Button("Murder Loop")) {
							murderLoop = true;
							murderCount = 200; //controls how many times the player is to be murdered
						}
						if (murderLoop && ImGui::Button(std::format("Stop Murder Loop ({})", 200 - murderCount).c_str())) {
							murderLoop = false;
							murderCount = 0;
						}
					}

					if (murderDelay <= 0) {
						if (murderCount > 0 && selectedPlayer.has_value()) {
							for (auto p : selectedPlayers) {
								auto validPlayer = p.validate();
								if (IsInGame()) {
									State.rpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, validPlayer.get_PlayerControl(), 1, false));
								}
								else if (IsInLobby()) {
									State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, validPlayer.get_PlayerControl(), 1, false));
								}
							}
							murderDelay = 5;
							murderCount--;
						}
						else {
							murderLoop = false;
							murderCount = 0;
						}
					}
					else murderDelay--;

					if (IsInGame() && (State.RealRole == RoleTypes__Enum::Impostor || State.RealRole == RoleTypes__Enum::Shapeshifter || State.RealRole == RoleTypes__Enum::Phantom)) {
						if (ImGui::Button("Level Farm")) {
							State.LevelFarm = true;
							for (auto p : selectedPlayers) {
								auto validPlayer = p.validate();
								State.rpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, validPlayer.get_PlayerControl(), 10000, false));
							}
						}
					}

					/*if (farmDelay <= 0) {
						if (farmCount > 0 && selectedPlayer.has_value()) {
							for (auto p : selectedPlayers) {
								auto validPlayer = p.validate();
								State.rpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, validPlayer.get_PlayerControl(), 1, false));
							}
							farmDelay = 5;
							farmCount--;
							if (farmCount < 1000 && farmCount % 50 == 0) {
								for (auto p : selectedPlayers) {
									auto validPlayer = p.validate();
									if (IsHost() || !State.SafeMode) State.rpcQueue.push(new RpcForceReportBody(validPlayer.get_PlayerControl(), validPlayer));
									break;
								}
							}
							if (State.InMeeting) {
								State.rpcQueue.push(new RpcEndMeeting());
								State.InMeeting = false;
							}
						}
						else {
							farmLoop = false;
							farmCount = 0;
						}
					}
					else farmDelay--;*/
				}

				if (!State.SafeMode && IsInGame()) {
					ImGui::SameLine();
					if (!suicideLoop && ImGui::Button("Suicide Loop")) {
						suicideLoop = true;
						suicideCount = 200; //controls how many times the player is to be murdered
					}
					if (suicideLoop && ImGui::Button(std::format("Stop Suicide Loop ({})", 800 - murderCount * 4).c_str())) {
						suicideLoop = false;
						suicideCount = 0;
					}

					if (suicideDelay <= 0) {
						if (suicideCount > 0 && selectedPlayer.has_value()) {
							for (auto p : selectedPlayers) {
								auto validPlayer = p.validate();
								if (IsInGame()) {
									State.rpcQueue.push(new RpcMurderPlayer(validPlayer.get_PlayerControl(), validPlayer.get_PlayerControl(),
										validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								}
								else if (IsInLobby()) {
									State.lobbyRpcQueue.push(new RpcMurderPlayer(validPlayer.get_PlayerControl(), validPlayer.get_PlayerControl(),
										validPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								}
							}
							suicideDelay = 15;
							suicideCount--;
						}
						else {
							suicideLoop = false;
							suicideCount = 0;
						}
					}
					else suicideDelay--;
				}

				if (!State.SafeMode && selectedPlayers.size() == 1 && (IsInGame() || (IsInLobby() && State.KillInLobbies))) {
					if (ImGui::Button("Kill Crewmates By")) {
						for (auto player : GetAllPlayerControl()) {
							if (!PlayerIsImpostor(GetPlayerData(player))) {
								if (IsInGame())
									State.rpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), player,
										selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), player,
										selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
						}
					}
					if (ImGui::Button("Kill Impostors By") && IsInGame()) {
						for (auto player : GetAllPlayerControl()) {
							if (PlayerIsImpostor(GetPlayerData(player))) {
								if (IsInGame())
									State.rpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), player,
										selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								else if (IsInLobby())
									State.lobbyRpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), player,
										selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							}
						}
					}
				}

				if (!State.SafeMode)
				{
					if (selectedPlayers.size() == 1 && ImGui::Button("Shift Everyone To"))
					{
						for (auto player : GetAllPlayerControl()) {
							if (IsInGame()) {
								State.rpcQueue.push(new RpcShapeshift(player, State.selectedPlayer, !State.AnimationlessShapeshift));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcShapeshift(player, State.selectedPlayer, !State.AnimationlessShapeshift));
							}
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Unshift Everyone"))
					{
						for (auto player : GetAllPlayerControl()) {
							if (IsInGame()) {
								State.rpcQueue.push(new RpcShapeshift(player, PlayerSelection(player), !State.AnimationlessShapeshift));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcShapeshift(player, PlayerSelection(player), !State.AnimationlessShapeshift));
							}
						}
					}
					if (ImGui::Button("Force Vanish"))
					{
						for (auto p : selectedPlayers) {
							auto validPlayer = p.validate();
							if (IsInGame()) {
								State.rpcQueue.push(new RpcVanish(validPlayer.get_PlayerControl()));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcVanish(validPlayer.get_PlayerControl()));
							}
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Force Appear"))
					{
						for (auto p : selectedPlayers) {
							auto validPlayer = p.validate();
							if (IsInGame()) {
								State.rpcQueue.push(new RpcVanish(validPlayer.get_PlayerControl(), true));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcVanish(validPlayer.get_PlayerControl(), true));
							}
						}
					}
				}
				ImGui::NewLine();
				if ((IsHost() || !State.SafeMode) && State.InMeeting && selectedPlayers.size() == 1) {
					if (ImGui::Button("Vote Off")) {
						State.VoteOffPlayerId = selectedPlayer.get_PlayerControl()->fields.PlayerId;
						for (auto player : GetAllPlayerControl()) {
							State.rpcQueue.push(new RpcVotePlayer(player, selectedPlayer.get_PlayerControl()));
						}
					}
				}

				if (!selectedPlayer.is_LocalPlayer() && selectedPlayers.size() == 1) {
					if (ImGui::Button("Teleport To")) {
						if (IsInGame())
							State.rpcQueue.push(new RpcSnapTo(GetTrueAdjustedPosition(selectedPlayer.get_PlayerControl())));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcSnapTo(GetTrueAdjustedPosition(selectedPlayer.get_PlayerControl())));
					}
				}
				ImGui::SameLine();
				if (!selectedPlayer.is_LocalPlayer() && !State.SafeMode) {
					if (ImGui::Button("Teleport To You")) {
						for (auto p : selectedPlayers) {
							if (IsInGame())
								State.rpcQueue.push(new RpcForceSnapTo(p.validate().get_PlayerControl(), GetTrueAdjustedPosition(*Game::pLocalPlayer)));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcForceSnapTo(p.validate().get_PlayerControl(), GetTrueAdjustedPosition(*Game::pLocalPlayer)));
						}
					}
				}

				if ((IsInGame() || IsInLobby()) && selectedPlayer.has_value() && selectedPlayers.size() == 1)
				{
					if (State.ActiveAttach && selectedPlayer.has_value() && (State.playerToAttach.equals(State.selectedPlayer) || selectedPlayer.is_LocalPlayer())) {
						if (ImGui::Button("Stop Attaching")) {
							State.playerToAttach = {};
							State.ActiveAttach = false;
						}
					}
					else {
						if (!selectedPlayer.is_LocalPlayer() && ImGui::Button("Attach To")) {
							State.playerToAttach = State.selectedPlayer;
							State.ActiveAttach = true;
						}
					}
				}

				if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby()) && !selectedPlayer.get_PlayerData()->fields.IsDead && selectedPlayers.size() == 1) {
					if (ImGui::Button("Turn into Ghost"))
					{
						if (PlayerIsImpostor(selectedPlayer.get_PlayerData())) {
							if (IsInGame())
								State.rpcQueue.push(new RpcSetRole(selectedPlayer.get_PlayerControl(), RoleTypes__Enum::ImpostorGhost));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcSetRole(selectedPlayer.get_PlayerControl(), RoleTypes__Enum::ImpostorGhost));
						}
						else {
							if (IsInGame())
								State.rpcQueue.push(new RpcSetRole(selectedPlayer.get_PlayerControl(), RoleTypes__Enum::CrewmateGhost));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcSetRole(selectedPlayer.get_PlayerControl(), RoleTypes__Enum::CrewmateGhost));
						}
					}
				}

				/*if (IsHost() && (IsInGame() || IsInLobby()) && selectedPlayer.get_PlayerData()->fields.IsDead && selectedPlayers.size() == 1) {
					if (ImGui::Button("Revive"))
					{
						if (IsInGame()) {
							State.rpcQueue.push(new RpcRevive(selectedPlayer.get_PlayerControl()));
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcRevive(selectedPlayer.get_PlayerControl()));
						}
					}
				}*/

				if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby()) && selectedPlayers.size() == 1) {
					if (CustomListBoxInt("Select Role", &State.FakeRole, FAKEROLES, 100.0f * State.dpiScale))
						State.Save();
					ImGui::SameLine();
					if (ImGui::Button("Set Role"))
					{
						if (IsInGame())
							State.rpcQueue.push(new RpcSetRole(selectedPlayer.get_PlayerControl(), RoleTypes__Enum(State.FakeRole)));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcSetRole(selectedPlayer.get_PlayerControl(), RoleTypes__Enum(State.FakeRole)));
					}
				}

				if (GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) {
					if (!State.SafeMode && ImGui::Button("Set Scanner")) {
						for (auto p : selectedPlayers) {
							if (IsInGame())
								State.rpcQueue.push(new RpcForceScanner(p.validate().get_PlayerControl(), true));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcForceScanner(p.validate().get_PlayerControl(), true));
						}
					}
					ImGui::SameLine();
					if (!State.SafeMode && ImGui::Button("Stop Scanner")) {
						for (auto p : selectedPlayers) {
							if (IsInGame())
								State.rpcQueue.push(new RpcForceScanner(p.validate().get_PlayerControl(), false));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcForceScanner(p.validate().get_PlayerControl(), false));
						}
					}
				}

				if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby()) && selectedPlayers.size() == 1) {
					ImGui::NewLine(); //force a new line
					if (InputString("Username", &State.hostUserName)) {
						State.Save();
					}
					if (ImGui::Button("Force Name"))
					{
						if (IsInGame())
							State.rpcQueue.push(new RpcForceName(selectedPlayer.get_PlayerControl(), State.hostUserName));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcForceName(selectedPlayer.get_PlayerControl(), State.hostUserName));
					}
					/*static bool changeSize = false, italics = false, underline = false, strike = false;
					static float size = 1.f;
					static ImVec4 col1 = ImVec4(1.f, 1.f, 1.f, 1.f), col2 = ImVec4(1.f, 1.f, 1.f, 1.f);
					if (ImGui::Button("Set Custom Name")) {
						State.rpcQueue.push(new RpcForceName(selectedPlayer.get_PlayerControl(), GetCustomName(false)));
					}
					ImGui::SameLine();
					if (ImGui::Button("Set w/o Modification")) {
						State.rpcQueue.push(new RpcForceName(selectedPlayer.get_PlayerControl(), GetCustomName(true)));
					}
					if (ImGui::CollapsingHeader("Custom Name Options")) {
						ToggleButton("Change Size", &changeSize);
						ImGui::SameLine();
						ToggleButton("Italics", &italics);

						ToggleButton("Underline", &underline);
						ImGui::SameLine();
						ToggleButton("Strikethrough", &strike);

						ImGui::InputFloat("Size", &size);
						ImGui::ColorEdit4("Color 1", (float*)&col1);
						ImGui::ColorEdit4("Color 2", (float*)&col2);
					}
				}*/

					if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby())) {
						CustomListBoxInt(" ", &State.HostSelectedColorId, COLORS, 85.0f * State.dpiScale);
						ImGui::SameLine();
						if (ImGui::Button("Force Color"))
						{
							if (IsInGame()) {
								if (IsHost())
									State.rpcQueue.push(new RpcForceColor(selectedPlayer.get_PlayerControl(), State.HostSelectedColorId));
								else
									State.rpcQueue.push(new RpcForceColor(selectedPlayer.get_PlayerControl(), State.HostSelectedColorId));
							}
							else if (IsInLobby()) {
								if (IsHost())
									State.lobbyRpcQueue.push(new RpcForceColor(selectedPlayer.get_PlayerControl(), State.HostSelectedColorId));
								else
									State.lobbyRpcQueue.push(new RpcForceColor(selectedPlayer.get_PlayerControl(), State.HostSelectedColorId));
							}
						}
					}

					if (!State.SafeMode && (IsInGame() || IsInLobby())) {
						static int level = 0;
						ImGui::InputInt("Level", &level);
						if (ImGui::Button("Force Level")) {
							if (IsInGame())
								State.rpcQueue.push(new RpcSetLevel(selectedPlayer.get_PlayerControl(), level));
							else if (IsInLobby())
								State.lobbyRpcQueue.push(new RpcSetLevel(selectedPlayer.get_PlayerControl(), level));
						}
					}
				}
				if (State.selectedPlayers.size() == 1) {
					if ((IsInGame() || IsInLobby()) && !selectedPlayer.is_Disconnected() && !selectedPlayer.is_LocalPlayer())
					{
						if (State.playerToWhisper.equals(State.selectedPlayer) && State.activeWhisper) {
							if (ImGui::Button("Stop Whispering To")) {
								State.playerToWhisper = {};
								State.activeWhisper = false;
							}
						}
						else {
							if (ImGui::Button("Whisper To")) {
								State.playerToWhisper = State.selectedPlayer;
								State.activeWhisper = true;
							}
						}
					}

					if (!State.SafeMode && (IsInGame() || IsInLobby()) && !selectedPlayer.is_Disconnected() && !selectedPlayer.is_LocalPlayer())
					{
						if (State.playerToChatAs.equals(State.selectedPlayer) && State.activeChatSpoof) {
							if (ImGui::Button("Stop Chatting As")) {
								State.playerToChatAs = {};
								State.activeChatSpoof = false;
							}
						}
						else {
							if (ImGui::Button("Chat As")) {
								State.playerToChatAs = State.selectedPlayer;
								State.activeChatSpoof = true;
							}
						}
					}
				}
			}
			if (openPUID && selectedPlayer.has_value()) {
				ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
				if (ImGui::Button("Steal Data")) {
					State.StealedPUID = convert_from_string(selectedPlayer.get_PlayerData()->fields.Puid);
					State.StealedFC = convert_from_string(selectedPlayer.get_PlayerData()->fields.FriendCode);
					State.Save();
				}
				ImGui::Dummy(ImVec2(15, 15) * State.dpiScale);
				if (InputString("PUID", &State.StealedPUID)) {
					State.Save();
				}
				ImGui::Dummy(ImVec2(2, 2) * State.dpiScale);
				if (InputString("Friend Code", &State.StealedFC)) {
					State.Save();
				}
				ImGui::Dummy(ImVec2(10, 10)* State.dpiScale);
				{
					if (convert_from_string(selectedPlayer.get_PlayerData()->fields.Puid) != "" && ImGui::Button("Copy PUID"))
						ClipboardHelper_PutClipboardString(selectedPlayer.get_PlayerData()->fields.Puid, NULL);
				}
				ImGui::SameLine();
				{
					if (convert_from_string(selectedPlayer.get_PlayerData()->fields.FriendCode) != "" && ImGui::Button("Copy Friend Code"))
						ClipboardHelper_PutClipboardString(selectedPlayer.get_PlayerData()->fields.FriendCode, NULL);
				}
			}

			if (openDetection && selectedPlayer.has_value()) {
				{
					static std::string aumMessage = "";
					if (ImGui::Button(!State.SafeMode || IsHost() ? "Force AUM Chat" : "Fake AUM Chat")) {
						if (IsInGame()) State.rpcQueue.push(new RpcForceAumChat(selectedPlayer, aumMessage, !State.SafeMode));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceAumChat(selectedPlayer, aumMessage, !State.SafeMode));
					}
					{
						ImGui::SameLine();
						static std::string knMessage = "";
						if (ImGui::Button(!State.SafeMode || IsHost() ? "Force KN Chat" : "Fake KN Chat")) {
							if (IsInGame()) State.rpcQueue.push(new RpcForceKNChat(selectedPlayer, knMessage, !State.SafeMode));
							if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceKNChat(selectedPlayer, knMessage, !State.SafeMode));
						}

						InputString("AUM Message", &aumMessage);

						InputString("KN Message", &knMessage);
					}
				}

				ImGui::NewLine();
				{
					//we have to send these rpc messages as ourselves since anticheat only allows you to send rpcs with your own net id
					if (ImGui::Button(!State.SafeMode || IsHost() ? "Force AmongUsMenu Detection" : "Fake AmongUsMenu Detection")) {
						if (IsInGame()) State.rpcQueue.push(new RpcForceDetectAum(selectedPlayer, !State.SafeMode));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceDetectAum(selectedPlayer, !State.SafeMode));
					}

					ImGui::NewLine();
					if (ImGui::Button(!State.SafeMode || IsHost() ? "Force KillNetwork Detection" : "Fake KillNetwork Detection")) {
						if (IsInGame()) State.rpcQueue.push(new RpcForceDetectKN(selectedPlayer, !State.SafeMode));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceDetectKN(selectedPlayer, !State.SafeMode));
					}

					ImGui::NewLine();
					if (ImGui::Button(!State.SafeMode || IsHost() ? "Force BetterAmongUs Detection" : "Fake BetterAmongUs Detection")) {
						if (IsInGame()) State.rpcQueue.push(new RpcForceDetectBAU(selectedPlayer, !State.SafeMode));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceDetectBAU(selectedPlayer, !State.SafeMode));
					}
				}
			}
	ImGui::EndChild();
		}

		if (openPlayer || State.selectedPlayers.size() == 0 || IsInLobby()) { //clear murder/suicide loops
			murderLoop = false;
			suicideLoop = false;
			murderCount = 0;
			suicideCount = 0;
		}

		static int blinkDelay = 0;
		static bool isBlinking = false;
		if (State.BlinkPlayersTab && !isBlinking) {
			blinkDelay = 30;
			isBlinking = true;
		}
		if (isBlinking && State.BlinkPlayersTab) {
			if (blinkDelay >= 0) blinkDelay--;
			if (blinkDelay == 0) {
				State.BlinkPlayersTab = false;
				isBlinking = false;
			}
		}
	}
}
