#include "pch-il2cpp.h"
#include "game_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "utility.h"
#include "state.hpp"
#include "logger.h"

namespace GameTab {
    enum Groups {
        General,
        Chat,
        Anticheat,
        Options
    };

    static bool openGeneral = true; //default to visual tab group
    static bool openChat = false;
    static bool openAnticheat = false;
    static bool openOptions = false;

    void CloseOtherGroups(Groups group) {
        openGeneral = group == Groups::General;
        openChat = group == Groups::Chat;
        openAnticheat = group == Groups::Anticheat;
        openOptions = group == Groups::Options;
    }

    void Render() {
        ImGui::SameLine(100 * State.dpiScale);
        ImGui::BeginChild("###Game", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        if (TabGroup("General", openGeneral)) {
            CloseOtherGroups(Groups::General);
        }
        ImGui::SameLine();
        if (TabGroup("Chat", openChat)) {
            CloseOtherGroups(Groups::Chat);
        }

        if (State.Enable_SMAC) {
            ImGui::SameLine();
            if (TabGroup("Anticheat", openAnticheat)) {
                CloseOtherGroups(Groups::Anticheat);
            }
        }

        if (GameOptions().HasOptions() && (IsInGame() || IsInLobby())) {
            ImGui::SameLine();
            if (TabGroup("Options", openOptions)) {
                CloseOtherGroups(Groups::Options);
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

            ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);

            if (IsHost() || !State.SafeMode) {
                CustomListBoxInt(" ", &State.SelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);
            }
            else {
                if (State.SelectedColorId >= (int)COLORS.size()) State.SelectedColorId = 0;
                CustomListBoxInt(" ", &State.SelectedColorId, COLORS, 85.0f * State.dpiScale);
            }
            ImGui::SameLine();
            if (ImGui::Button("Random Color"))
            {
                State.SelectedColorId = GetRandomColorId();
            }

            if (IsInGame() || IsInLobby()) {
                ImGui::SameLine();
                if (ImGui::Button("Set Color"))
                {
                    if (IsHost() || !State.SafeMode) {
                        if (IsInGame())
                            State.rpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
                    }
                    else if (IsColorAvailable(State.SelectedColorId)) {
                        if (IsInGame())
                            State.rpcQueue.push(new RpcSetColor(State.SelectedColorId));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new RpcSetColor(State.SelectedColorId));
                    }
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Snipe Color", &State.SnipeColor)) {
                State.Save();
            }

            if (ToggleButton("Console", &State.ShowConsole)) {
                State.Save();
            }

            /*if (ToggleButton("Auto-Join", &State.AutoJoinLobby))
                State.Save();
            ImGui::SameLine();
            if (InputString("Lobby Code", &State.AutoJoinLobbyCode))
                State.Save();

            if (ImGui::Button("Join Lobby")) {
                AmongUsClient_CoJoinOnlineGameFromCode(*Game::pAmongUsClient,
                    GameCode_GameNameToInt(convert_to_string(State.AutoJoinLobbyCode), NULL),
                    NULL);
            }*/ //doesn't work as of now
            if (IsInGame() || IsInLobby()) ImGui::SameLine();
            if ((IsInGame() || IsInLobby()) && ImGui::Button("Reset Appearance"))
            {
                ControlAppearance(false);
            }


            if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && (IsHost() || !State.SafeMode) && ImGui::Button("Kill Everyone")) {
                for (auto player : GetAllPlayerControl()) {
                    if (IsInGame())
                        State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                    else if (IsInLobby())
                        State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                }
            }
            if (IsInLobby()) ImGui::SameLine();
            if (IsInLobby() && (IsHost() || !State.SafeMode) && ImGui::Button("Allow Everyone to NoClip")) {
                for (auto p : GetAllPlayerControl()) {
                    if (p != *Game::pLocalPlayer) State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, p, 1, true));
				}
                State.NoClip = true;
                ShowHudNotification("Allowed everyone to NoClip!");
            }
            /*if (ImGui::Button("Spawn Dummy")) {
                if (IsInGame()) State.rpcQueue.push(new RpcSpawnDummy());
                if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSpawnDummy());
            }*/
            if (IsInGame() || IsInLobby()) {
                ImGui::SameLine();
                if (ImGui::Button(IsHost() ? "Protect Everyone" : "Visual Protect Everyone")) {
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

            if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && (IsHost() || !State.SafeMode)) {
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
            }

            if (IsInGame() || IsInLobby()) {
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
                if (IsInGame() && !State.InMeeting && ImGui::Button("Kick Everyone From Vents")) {
                    State.rpcQueue.push(new RpcBootAllVents());
                }
                if (State.InMeeting) ImGui::SameLine();
                if ((IsHost() || !State.SafeMode) && State.InMeeting && ImGui::Button("End Meeting")) {
                    State.rpcQueue.push(new RpcEndMeeting());
                    State.InMeeting = false;
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
        }

        if (openChat) {
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
            if (State.ReadAndSendAumChat && (IsInGame() || IsInLobby()) && ImGui::Button("Send to AUM"))
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
            if (State.ChatSpamMode) ImGui::SameLine();
            if (State.ChatSpamMode && ToggleButton("Spam by Everyone", &State.ChatSpamEveryone))
            {
                State.Save();
            }
            if (CustomListBoxInt("Chat Spam Mode", &State.ChatSpamMode, 
                { State.SafeMode ? "With Message (Self-Spam ONLY)" : "With Message", "Blank Chat", State.SafeMode ? "Self Message + Blank Chat" : "Message + Blank Chat" })) State.Save();
        }

        if (openAnticheat) {
            if (IsHost()) CustomListBoxInt("Host Punishment­", &State.SMAC_HostPunishment, SMAC_HOST_PUNISHMENTS, 85.0f * State.dpiScale);
            else CustomListBoxInt("Regular Punishment", &State.SMAC_Punishment, SMAC_PUNISHMENTS, 85.0f * State.dpiScale);

            if (ToggleButton("Add Cheaters to Blacklist", &State.SMAC_AddToBlacklist)) State.Save();
            if (ToggleButton("Punish Blacklisted Players", &State.SMAC_PunishBlacklist)) State.Save();
            if (State.SMAC_PunishBlacklist) {
                if (State.BlacklistPUID.empty())
                    ImGui::Text("No users in blacklist!");
                static std::string newPuid = "";
                InputString("New PUID", &newPuid, ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::SameLine();
                if (newPuid != "" && ImGui::Button("Add PUID")) {
                    State.BlacklistPUID.push_back(newPuid);
                    State.Save();
                    newPuid = "";
                }
                if (!State.BlacklistPUID.empty()) {
                    static int selectedPuidIndex = 0;
                    selectedPuidIndex = std::clamp(selectedPuidIndex, 0, (int)State.BlacklistPUID.size() - 1);
                    std::vector<const char*> puidVector(State.BlacklistPUID.size(), nullptr);
                    for (auto i : State.BlacklistPUID) {
                        puidVector.push_back(i.c_str());
                    }
                    CustomListBoxInt("Player to Delete", &selectedPuidIndex, puidVector);
                    ImGui::SameLine();
                    if (ImGui::Button("Delete"))
                        State.BlacklistPUID.erase(std::find(State.BlacklistPUID.begin(), State.BlacklistPUID.end(), State.BlacklistPUID[selectedPuidIndex]));
                }
            }
            ImGui::NewLine();
            if (ToggleButton("Detect AUM Usage", &State.SMAC_CheckAUM)) State.Save();
            if (ToggleButton("Detect SickoMenu Usage", &State.SMAC_CheckSicko)) State.Save();
            if (ToggleButton("Detect Abnormal Names", &State.SMAC_CheckBadNames)) State.Save();
            if (ToggleButton("Detect Abnormal Set Color", &State.SMAC_CheckColor)) State.Save();
            if (ToggleButton("Detect Abnormal Set Cosmetics", &State.SMAC_CheckCosmetics)) State.Save();
            if (ToggleButton("Detect Abnormal Chat Note", &State.SMAC_CheckChatNote)) State.Save();
            if (ToggleButton("Detect Abnormal Scanner", &State.SMAC_CheckScanner)) State.Save();
            if (ToggleButton("Detect Abnormal Animation", &State.SMAC_CheckAnimation)) State.Save();
            if (ToggleButton("Detect Setting Tasks", &State.SMAC_CheckTasks)) State.Save();
            if (ToggleButton("Detect Setting Roles", &State.SMAC_CheckRole)) State.Save();
            if (ToggleButton("Detect Abnormal Chat", &State.SMAC_CheckChat)) State.Save();
            if (ToggleButton("Detect Abnormal Meetings", &State.SMAC_CheckMeeting)) State.Save();
            if (ToggleButton("Detect Abnormal Body Reports", &State.SMAC_CheckReport)) State.Save();
            if (ToggleButton("Detect Abnormal Murders", &State.SMAC_CheckMurder)) State.Save();
            if (ToggleButton("Detect Abnormal Shapeshift", &State.SMAC_CheckShapeshift)) State.Save();
            if (ToggleButton("Detect Abnormal Vanish", &State.SMAC_CheckVanish)) State.Save();
            if (ToggleButton("Detect Abnormal Player Levels", &State.SMAC_CheckLevel)) State.Save();
            if (State.SMAC_CheckLevel && ImGui::InputInt("Minimum Level to Detect", &State.SMAC_HighLevel)) {
                State.Save();
            }
            if (ToggleButton("Detect Abnormal Venting", &State.SMAC_CheckVent)) State.Save();
            if (ToggleButton("Detect Abnormal Sabotages", &State.SMAC_CheckSabotage)) State.Save();
        }

        if (openOptions) {
            if (GameOptions().HasOptions()) {
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

                    ImGui::Dummy(ImVec2(3, 3)* State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3)* State.dpiScale);

                    ImGui::Text("Max Shapeshifters: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Shapeshifter));
                    ImGui::Text("Shapeshifter Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Shapeshifter));
                    ImGui::Text("Shapeshifter Shift Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, 1.0F));
                    ImGui::Text("Shapeshifter Shift Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Noisemakers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Noisemaker));
                    ImGui::Text("Noisemaker Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Noisemaker));
                    ImGui::Text("Noisemaker Alert Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::NoisemakerAlertDuration, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Trackers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Tracker));
                    ImGui::Text("Tracker Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Tracker));
                    ImGui::Text("Tracking Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerDuration, 1.0F));
                    ImGui::Text("Tracking Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerCooldown, 1.0F));
                    ImGui::Text("Tracking Delay: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerDelay, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Phantoms: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Phantom));
                    ImGui::Text("Phantom Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Phantom));
                    ImGui::Text("Phantom Vanish Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::PhantomCooldown, 1.0F));
                    ImGui::Text("Phantom Vanish Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::PhantomDuration, 1.0F));
                }
                else if (options.GetGameMode() == GameModes__Enum::HideNSeek) {

                    int ImpostorId = options.GetInt(app::Int32OptionNames__Enum::ImpostorPlayerID);
                    if (ImpostorId < 0) {
                        ImGui::Text("Impostor: Round-robin");
                    }
                    else {
                        std::string ImpostorName = std::format("Selected Impostor: {}", convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerDataById(ImpostorId), nullptr)));
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
            else CloseOtherGroups(Groups::General);
        }
        ImGui::EndChild();
    }
}