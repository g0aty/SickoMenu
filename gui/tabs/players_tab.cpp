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
	};

	static bool openPlayer = true; //default to visual tab group
	static bool openTrolling = false;

	void CloseOtherGroups(Groups group) {
		openPlayer = group == Groups::Player;
		openTrolling = group == Groups::Trolling;
	}

	std::string GetCustomName(bool preserve = true, bool changeSize = false, float size = 1.f, bool italics = false, bool underline = false, bool strike = false, bool color = false, ImVec4 color1 = ImVec4(1.f, 1.f, 1.f, 1.f), ImVec4 color2 = ImVec4(1.f, 1.f, 1.f, 1.f)) {
		if (!State.selectedPlayer.has_value()) return "";
		std::string name = preserve ? convert_from_string(GameData_PlayerInfo_get_PlayerName(State.selectedPlayer.validate().get_PlayerData(), NULL)) : State.hostUserName;
		name = RemoveHtmlTags(name);
		std::string prefix = "", suffix = "";
		if (changeSize) {
			prefix += std::format("<size={}%>", size * 100);
			suffix += "</size>";
		}
		if (italics) {
			prefix += "<i>";
			suffix += "</i>";
		}
		if (underline && !color) {
			prefix += "<u>";
			suffix += "</u>";
		}
		if (strike && !color) {
			prefix += "<s>";
			suffix += "</s>";
		}
		if (color) {
			name = GetGradientUsername(name, false, underline, strike, color1, color2);
		}
		return prefix + name + suffix;
	}

	void Render() {
		if (IsInGame() || IsInLobby()) {
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::BeginChild("players#list", ImVec2(200, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
			auto selectedPlayer = State.selectedPlayer.validate();
			bool shouldEndListBox = ImGui::ListBoxHeader("###players#list", ImVec2(200, 150) * State.dpiScale);
			auto localData = GetPlayerData(*Game::pLocalPlayer);
			for (auto playerCtrl : GetAllPlayerControl()) {
				const auto& player = PlayerSelection(playerCtrl).validate();
				if (!player.has_value())
					continue;
				auto playerData = GetPlayerData(playerCtrl);
				if (playerData->fields.Disconnected)
					continue;
				app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(playerData);
				if (outfit == NULL) continue;
				std::string playerName = RemoveHtmlTags(convert_from_string(GameData_PlayerOutfit_get_PlayerName(outfit, nullptr)));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0) * State.dpiScale);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0) * State.dpiScale);
				if (ImGui::Selectable(std::string("##" + ToString(playerData->fields.PlayerId)).c_str(), selectedPlayer.equals(player))) { //fix selection problems with multiple ppl having same name
					State.selectedPlayer = player;
					selectedPlayer = player;
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
				if (State.RevealRoles && IsInGame())
				{
					std::string roleName = GetRoleName(playerData->fields.Role);
					playerName = playerName + " (" + roleName + ")";
					nameColor = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
				}
				else if (PlayerIsImpostor(localData) && PlayerIsImpostor(playerData))
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->ImpostorRed);
				else if (std::count(State.sickoUsers.begin(), State.sickoUsers.end(), playerData->fields.PlayerId))
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->AcceptedGreen);

				if (playerData->fields.IsDead)
					nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->DisabledGrey);

				ImGui::TextColored(nameColor, playerName.c_str());
			}
			if (shouldEndListBox)
				ImGui::ListBoxFooter();

			if (selectedPlayer.has_value()) //Upon first startup no player is selected.  Also rare case where the playerdata is deleted before the next gui cycle
			{
				ImGui::Text("Is using SickoMenu: %s", selectedPlayer.is_LocalPlayer() || std::count(State.sickoUsers.begin(), State.sickoUsers.end(), selectedPlayer.get_PlayerData()->fields.PlayerId) ? "Yes" : "No");
				ImGui::Text("Is using AUM: %s", std::count(State.aumUsers.begin(), State.aumUsers.end(), selectedPlayer.get_PlayerData()->fields.PlayerId) ? "Yes" : "No");
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
				for (auto client : GetAllClients()) {
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
						break;
					}
				}
				std::string platformText = std::format("Platform: {}", platform);
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
			if ((!selectedPlayer.has_value() || selectedPlayer.is_LocalPlayer()) || State.SafeMode) {
				if ((IsHost() || !State.SafeMode) && State.InMeeting && ImGui::Button("Skip Vote by All")) {
					for (auto player : GetAllPlayerControl()) {
						/*if (player != selectedPlayer.get_PlayerControl()) {
							State.rpcQueue.push(new RpcClearVote(player));
						}*/
						State.rpcQueue.push(new RpcVotePlayer(player, player, true));
					}
					State.InMeeting = false;
				}
			}
			if (openPlayer && selectedPlayer.has_value())
			{
				if (IsInGame() && !State.DisableMeetings) {
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

				if (!selectedPlayer.is_Disconnected())
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

				/*if (IsInGame() && PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))
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
				else {// if (!State.SafeMode)*/
				if (ImGui::Button("Kill"))
				{
					if (IsInGame()) {
						State.rpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), selectedPlayer.get_PlayerControl(),
							selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
					}
					else if (IsInLobby()) {
						State.lobbyRpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), selectedPlayer.get_PlayerControl(),
							selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
					}
				}
				//}
				if (IsInGame() && PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))
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
						State.rpcQueue.push(new CmdCheckMurder(State.selectedPlayer));
						framesPassed = 40;
					}
				}
				else {// if (!State.SafeMode)
					ImGui::SameLine();
					if (ImGui::Button("Telekill"))
					{
						previousPlayerPosition = GetTrueAdjustedPosition(*Game::pLocalPlayer);
						if (IsInGame()) {
							State.rpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), selectedPlayer.get_PlayerControl(),
								selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcMurderPlayer((*Game::pLocalPlayer), selectedPlayer.get_PlayerControl(),
								selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
						}
						framesPassed = 40;
					}
				}

				if ((IsInMultiplayerGame() || IsInLobby()) && !selectedPlayer.is_LocalPlayer()) {
					if (IsHost() && ImGui::Button("Kick")) {
						app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), selectedPlayer.get_PlayerControl()->fields._.OwnerId, false, NULL);
					}
					if (ImGui::Button("Votekick")) {
						if (IsInGame()) {
							State.rpcQueue.push(new RpcVoteKick(selectedPlayer.get_PlayerControl()));
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcVoteKick(selectedPlayer.get_PlayerControl()));
						}
					}
					if (!State.SafeMode) {
						if (ImGui::Button("Kick Exploit")) {
							if (IsInGame()) {
								State.rpcQueue.push(new RpcVoteKick(selectedPlayer.get_PlayerControl(), true));
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcVoteKick(selectedPlayer.get_PlayerControl(), true));
							}
						}
					}
					else if (IsInGame()) {
						if (ImGui::Button("Attempt to Ban")) {
							State.rpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, selectedPlayer.get_PlayerControl(), 200, true));
						}
					}
					if (IsHost()) ImGui::SameLine();
					if (IsHost() && ImGui::Button("Ban")) {
						app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), selectedPlayer.get_PlayerControl()->fields._.OwnerId, true, NULL);
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
					if (ImGui::Button("Shift"))
					{
						if (IsInGame())
							State.rpcQueue.push(new RpcShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
					}
				}
				else if (State.RealRole == RoleTypes__Enum::Shapeshifter && role == RoleTypes__Enum::Shapeshifter) {
					app::ShapeshifterRole* shapeshifterRole = (app::ShapeshifterRole*)playerRole;
					if (shapeshifterRole->fields.cooldownSecondsRemaining <= 0 && ImGui::Button("Shift"))
					{
						if (IsInGame())
							State.rpcQueue.push(new CmdCheckShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new CmdCheckShapeshift(*Game::pLocalPlayer, State.selectedPlayer, !State.AnimationlessShapeshift));
					}
				}

				if (IsHost() || !State.SafeMode)
				{
					if (ImGui::Button("Protect Player")) {
						app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
						auto colorId = outfit->fields.ColorId;
						if (IsInGame())
							State.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, State.selectedPlayer, colorId));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, State.selectedPlayer, colorId));
					}
				}
				else if (State.RealRole == RoleTypes__Enum::GuardianAngel && role == RoleTypes__Enum::GuardianAngel) {
					app::GuardianAngelRole* guardianAngelRole = (app::GuardianAngelRole*)playerRole;
					if (guardianAngelRole->fields.cooldownSecondsRemaining <= 0 && ImGui::Button("Protect Player")) {
						if (IsInGame())
							State.rpcQueue.push(new CmdCheckProtect(*Game::pLocalPlayer, State.selectedPlayer));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new CmdCheckProtect(*Game::pLocalPlayer, State.selectedPlayer));
					}
				}

				if (IsInGame() && !selectedPlayer.is_Disconnected() && (IsInMultiplayerGame() || selectedPlayer.is_LocalPlayer()))
				{
					auto tasks = GetNormalPlayerTasks(selectedPlayer.get_PlayerControl());

					if ((!State.SafeMode || selectedPlayer.is_LocalPlayer()) && ImGui::Button("Complete all Tasks")) {
						for (auto t : tasks) {
							if (t->fields.taskStep != t->fields.MaxStep) State.rpcQueue.push(new RpcForceCompleteTask(selectedPlayer.get_PlayerControl(), t->fields._._Id_k__BackingField));
						}
					}

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

			if (openTrolling && selectedPlayer.has_value()) {
				if (IsInGame() && !GetPlayerData(selectedPlayer.get_PlayerControl())->fields.IsDead) {
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

				if (selectedPlayer.has_value() && IsInGame() && !selectedPlayer.get_PlayerData()->fields.IsDead) {
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

				if (!selectedPlayer.is_LocalPlayer()) {
					app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
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

				if (!State.SafeMode && ImGui::Button("Impersonate Everyone To")) {
					app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
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
							std::string name = convert_from_string(GameData_PlayerInfo_get_PlayerName(selectedPlayer.get_PlayerData(), NULL))
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
						std::queue<RPCInterface*>* queue = nullptr;

						if (IsInGame())
							queue = &State.rpcQueue;
						else if (IsInLobby())
							queue = &State.lobbyRpcQueue;
						if (queue != nullptr) {
							if (IsHost() || !State.SafeMode)
								queue->push(new RpcForceColor(*Game::pLocalPlayer, State.originalColor));
							else
								queue->push(new RpcSetColor(State.originalColor));
							queue->push(new RpcSetPet(State.originalPet));
							queue->push(new RpcSetSkin(State.originalSkin));
							queue->push(new RpcSetHat(State.originalHat));
							queue->push(new RpcSetVisor(State.originalVisor));
							queue->push(new RpcSetNamePlate(State.originalNamePlate));
							queue->push(new RpcSetName(State.originalName));
							State.activeImpersonation = false;
						}
					}
				}

				if (IsInLobby() && ImGui::Button("Allow Player to NoClip")) {
					if (selectedPlayer.is_LocalPlayer()) State.NoClip = true;
					else State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, selectedPlayer.get_PlayerControl(), 1, true));
					ShowHudNotification(std::format("Allowed {} to NoClip!", 
						convert_from_string(GameData_PlayerInfo_get_PlayerName(selectedPlayer.get_PlayerData(), NULL))));
				}

				if (!State.SafeMode) {
					if (ImGui::Button("Suicide")) {
						if (IsInGame()) {
							State.rpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), selectedPlayer.get_PlayerControl(),
								selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), selectedPlayer.get_PlayerControl(),
								selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
						}
					}
				}

				static int murderCount = 0;
				static bool murderLoop = false;
				static int murderDelay = 0;
				if (ImGui::Button("Murder Loop")) {
					murderLoop = true;
					murderCount = 20; //controls how many times the player is to be murdered
				}

				if (murderDelay <= 0) {
					if (murderCount > 0 && selectedPlayer.has_value()) {
						if (IsInGame()) {
							State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, selectedPlayer.get_PlayerControl(),
								selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							murderDelay = 15;
						}
						else if (IsInLobby()) {
							State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, selectedPlayer.get_PlayerControl(),
								selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
							murderDelay = 15;
						}
						murderCount--;
					}
					else {
						murderLoop = false;
						murderCount = 0;
					}
				}
				else murderDelay--;

				static int suicideCount = 0;
				static bool suicideLoop = false;
				static int suicideDelay = 0;
				if (!State.SafeMode) {
					ImGui::SameLine();
					if (ImGui::Button("Suicide Loop")) {
						suicideLoop = true;
						suicideCount = 20; //controls how many times the player is to be suicided
					}

					if (suicideDelay <= 0) {
						if (suicideCount > 0 && selectedPlayer.has_value()) {
							if (IsInGame()) {
								State.rpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), selectedPlayer.get_PlayerControl(),
									selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								suicideDelay = 15;
							}
							else if (IsInLobby()) {
								State.lobbyRpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), selectedPlayer.get_PlayerControl(),
									selectedPlayer.get_PlayerControl()->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
								suicideDelay = 15;
							}
							suicideCount--;
						}
						else {
							suicideLoop = false;
							suicideCount = 0;
						}
					}
					else suicideDelay--;
				}

				if (!State.SafeMode) {
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
					if (ImGui::Button("Kill Impostors By")) {
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
					if (ImGui::Button("Shift Everyone To"))
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
				}

				if ((IsHost() || !State.SafeMode) && State.InMeeting) {
					if (ImGui::Button("Vote Off")) {
						for (auto player : GetAllPlayerControl()) {
							if (player != selectedPlayer.get_PlayerControl()) {
								State.rpcQueue.push(new RpcClearVote(player));
							}
							State.rpcQueue.push(new RpcVotePlayer(player, selectedPlayer.get_PlayerControl()));
						}
					}
				}

				if (!selectedPlayer.is_LocalPlayer()) {
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
						if (IsInGame())
							State.rpcQueue.push(new RpcForceSnapTo(selectedPlayer.get_PlayerControl(), GetTrueAdjustedPosition(*Game::pLocalPlayer)));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcForceSnapTo(selectedPlayer.get_PlayerControl(), GetTrueAdjustedPosition(*Game::pLocalPlayer)));
					}
				}

				if ((IsInGame() || IsInLobby()) && selectedPlayer.has_value())
				{
					if ((State.playerToAttach.equals(State.selectedPlayer) && State.ActiveAttach) || (selectedPlayer.is_LocalPlayer() && selectedPlayer.has_value())) {
						if (ImGui::Button("Stop Attaching")) {
							State.playerToAttach = {};
							State.ActiveAttach = false;
						}
					}
					else {
						if (ImGui::Button("Attach To") && !selectedPlayer.is_LocalPlayer()) {
							State.playerToAttach = State.selectedPlayer;
							State.ActiveAttach = true;
						}
					}
				}

				if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby()) && !selectedPlayer.get_PlayerData()->fields.IsDead) {
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

				if (IsHost() && (IsInGame() || IsInLobby()) && selectedPlayer.get_PlayerData()->fields.IsDead) {
					if (ImGui::Button("Revive"))
					{
						if (IsInGame()) {
							app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
							State.rpcQueue.push(new RpcRevive(selectedPlayer.get_PlayerControl()));
							State.rpcQueue.push(new RpcForceColor(selectedPlayer.get_PlayerControl(), outfit->fields.ColorId));
						}
						else if (IsInLobby()) {
							app::GameData_PlayerOutfit* outfit = GetPlayerOutfit(selectedPlayer.get_PlayerData());
							State.lobbyRpcQueue.push(new RpcRevive(selectedPlayer.get_PlayerControl()));
							State.lobbyRpcQueue.push(new RpcForceColor(selectedPlayer.get_PlayerControl(), outfit->fields.ColorId));
						}
					}
				}

				if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby())) {
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
						if (IsInGame())
							State.rpcQueue.push(new RpcForceScanner(selectedPlayer.get_PlayerControl(), true));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcForceScanner(selectedPlayer.get_PlayerControl(), true));
					}
					ImGui::SameLine();
					if (!State.SafeMode && ImGui::Button("Stop Scanner")) {
						if (IsInGame())
							State.rpcQueue.push(new RpcForceScanner(selectedPlayer.get_PlayerControl(), false));
						else if (IsInLobby())
							State.lobbyRpcQueue.push(new RpcForceScanner(selectedPlayer.get_PlayerControl(), false));
					}
				}

				if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby())) {
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

					//we have to send these rpc messages as ourselves since anticheat only allows you to send rpcs with your own net id
					if (ImGui::Button(!State.SafeMode ? "Force AUM Detection" : "Fake AUM Detection")) {
						if (IsInGame()) State.rpcQueue.push(new RpcForceDetectAum(selectedPlayer, !State.SafeMode));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceDetectAum(selectedPlayer, !State.SafeMode));
					}
					ImGui::SameLine();
					static std::string aumMessage = "";
					if (ImGui::Button(!State.SafeMode ? "Force AUM Chat" : "Fake AUM Chat")) {
						if (IsInGame()) State.rpcQueue.push(new RpcForceAumChat(selectedPlayer, aumMessage, !State.SafeMode));
						if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceAumChat(selectedPlayer, aumMessage, !State.SafeMode));
					}

					InputString("AUM Message", &aumMessage);

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

			ImGui::EndChild();
		}
	}
}