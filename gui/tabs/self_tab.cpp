#include "pch-il2cpp.h"
#include "self_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "utility.h"
#include "state.hpp"
#include "logger.h"

extern void RevealAnonymousVotes(); // in MeetingHud.cpp

namespace SelfTab {
    enum Groups {
        Visuals,
        Utils,
        Randomizers
    };

    static bool openVisuals = true; //default to visual tab group
    static bool openUtils = false;
    static bool openRandomizers = false;

    void CloseOtherGroups(Groups group) {
        openVisuals = group == Groups::Visuals;
        openUtils = group == Groups::Utils;
        openRandomizers = group == Groups::Randomizers;
    }

    void Render() {
        ImGui::SameLine(100 * State.dpiScale);
        ImGui::BeginChild("###Self", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        if (TabGroup("Visuals", openVisuals)) {
            CloseOtherGroups(Groups::Visuals);
        }
        ImGui::SameLine();
        if (TabGroup("Utils", openUtils)) {
            CloseOtherGroups(Groups::Utils);
        }
        ImGui::SameLine();
        if (TabGroup("Randomizers", openRandomizers)) {
            CloseOtherGroups(Groups::Randomizers);
        }

        if (openVisuals) {
            ImGui::Dummy(ImVec2(4, 4)* State.dpiScale);
            if (ToggleButton("Max Vision", &State.MaxVision)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Wallhack", &State.Wallhack)) {
                State.Save();
            }
            ImGui::SameLine();
            ToggleButton("Disable HUD", &State.DisableHud);

            if (ToggleButton("Freecam", &State.FreeCam)) {
                State.playerToFollow = PlayerSelection();
                State.Save();
            }

            ImGui::SameLine(145.0f * State.dpiScale);
            SteppedSliderFloat("  ", &State.FreeCamSpeed, 0.f, 10.f, 0.05f, "%.2fx Speed", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            if (ToggleButton("Zoom", &State.EnableZoom)) {
                State.Save();
                if (!State.EnableZoom) RefreshChat();
            }

            ImGui::SameLine(145.0f * State.dpiScale);
            SteppedSliderFloat("   ", &State.CameraHeight, 0.5f, 10.0f, 0.1f, "%.2fx Zoom", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

            if (ToggleButton("Always show Chat Button", &State.ChatAlwaysActive)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Allow Paste in Chat", &State.ChatPaste)) { //add copying later
                State.Save();
            }
            
            if (ToggleButton("Read Messages by Ghosts", &State.ReadGhostMessages)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Read and Send AUM Chat", &State.ReadAndSendAumChat)) {
                State.Save();
            }
            if (State.ReadAndSendAumChat) ImGui::Text("Send AUM chat messages in regular chat by typing \"/aum [message]\"!");
            /*static int framesPassed = 0;
            if (ImGui::Button("Refresh Chat Button")) {
                State.RefreshChatButton = true;
                framesPassed = 100;
            }

            if (framesPassed == 0) State.RefreshChatButton = false;
            else framesPassed--;*/

            if (IsHost() || !State.SafeMode) {
                if (ToggleButton("Custom Name", &State.CustomName)) {
                    State.Save();
                }
            }
            else {
                if (ToggleButton("Custom Name (Client-sided ONLY)", &State.CustomName)) {
                    State.Save();
                }
            }

            if ((IsHost() || !State.SafeMode))
                ImGui::SameLine();

            if ((IsHost() || !State.SafeMode)&& ToggleButton("Server-sided Custom Name", &State.ServerSideCustomName)) {
                State.Save();
            }

            if (State.CustomName && ImGui::CollapsingHeader("Custom Name Options"))
            {
                if (ToggleButton("Size", &State.ResizeName)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Italics", &State.ItalicName)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Underline", &State.UnderlineName)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Strikethrough", &State.StrikethroughName)) {
                    State.Save();
                }

                if (ImGui::ColorEdit4("Starting Gradient Color", (float*)&State.NameColor1, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ImGui::ColorEdit4("Ending Gradient Color", (float*)&State.NameColor2, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Colored", &State.ColoredName)) {
                    State.Save();
                }

                if (ToggleButton("RGB", &State.RgbName)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ImGui::InputFloat("Name Size", &State.NameSize)) {
                    State.Save();
                }
            }

            if (ToggleButton("Reveal Roles", &State.RevealRoles)) {
                State.Save();
            }
            ImGui::SameLine(120.0f * State.dpiScale);
            if (ToggleButton("Abbrv. Role", &State.AbbreviatedRoleNames))
            {
                State.Save();
            }
            ImGui::SameLine(240.0f * State.dpiScale);
            if (ToggleButton("Player Colored Dots Next To Names", &State.PlayerColoredDots))
            {
                State.Save();
            }
            if (ToggleButton("Show Player Info in Lobby", &State.ShowPlayerInfo))
            {
                State.Save();
            }

            if (ToggleButton("Reveal Votes", &State.RevealVotes)) {
                State.Save();
            }
            if (!IsInGame() && !IsInLobby()
                || GameOptions().GetGameMode() != GameModes__Enum::Normal
                || GameOptions().GetBool(app::BoolOptionNames__Enum::AnonymousVotes)) {
                ImGui::SameLine();
                if (ToggleButton("Reveal Anonymous Votes", &State.RevealAnonymousVotes)) {
                    State.Save();
                    RevealAnonymousVotes();
                }
            }

            if (ToggleButton("See Ghosts", &State.ShowGhosts)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("See Protections", &State.ShowProtections))
            {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("See Kill Cooldown", &State.ShowKillCD)) {
                State.Save();
            }

            if (ToggleButton("Disable Kill Animation", &State.DisableKillAnimation)) {
                State.Save();
            }

            if (ToggleButton("Show Host", &State.ShowHost)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Show Vote Kicks", &State.ShowVoteKicks)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Show FPS", &State.ShowFps)) {
                State.Save();
            }

            if (State.InMeeting && ImGui::Button("Move in Meeting"))
            {
                State.rpcQueue.push(new EndMeeting());
                State.InMeeting = false;
            }
        }

        if (openUtils) {
            if (ToggleButton("Unlock Vents", &State.UnlockVents)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Move While in Vent & Shapeshifting", &State.MoveInVentAndShapeshift)) {
                if (!State.MoveInVentAndShapeshift && (State.InMeeting || (*Game::pLocalPlayer)->fields.inVent)) {
                    (*Game::pLocalPlayer)->fields.moveable = false;
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Always Move", &State.AlwaysMove)) {
                State.Save();
            }

            if (ToggleButton("Unlock Kill Button", &State.UnlockKillButton)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Shapeshift without Animation", &State.AnimationlessShapeshift)) {
                State.Save();
            }

            if (ToggleButton("NoClip", &State.NoClip)) {
                State.Save();
            }

            if (ToggleButton("Kill Other Impostors", &State.KillImpostors)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Infinite Kill Range", &State.InfiniteKillRange)) {
                State.Save();
            }

            if (ToggleButton("Bypass Guardian Angel Protections", &State.BypassAngelProt)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Autokill", &State.AutoKill)) {
                State.Save();
            }

            if (ToggleButton("Do Tasks as Impostor", &State.DoTasksAsImpostor)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Always Use Kill Exploit", &State.AlwaysUseKillExploit)) {
                State.Save();
            }

            if (ToggleButton("Fake Alive", &State.FakeAlive)) {
                State.Save();
            }
            if (IsHost() || !State.SafeMode) {
                ImGui::SameLine();
                if (ToggleButton("God Mode", &State.GodMode))
                    State.Save();
            }

            if (ToggleButton("(Shift + Right Click) to Teleport", &State.ShiftRightClickTP)) {
                State.Save();
            }
            if (!State.SafeMode) ImGui::SameLine();
            if (!State.SafeMode && ToggleButton("Hold ALT to Teleport Everyone", &State.TeleportEveryone)) {
                State.Save();
            }
            if (ToggleButton((State.SafeMode ? "Rotate Everyone (Client-sided ONLY)" : "Rotate Everyone"), &State.RotateEveryone)) {
                State.Save();
            }
            if (!State.SafeMode) ImGui::SameLine();
            if (!State.SafeMode && State.RotateEveryone && ToggleButton("Server-sided Rotation", &State.RotateServerSide)) {
                State.Save();
            }
            if (ImGui::InputFloat("Rotation Radius", &State.RotateRadius, 0.0f, 0.0f, "%.2f m")) {
                State.Save();
            }

            if (ImGui::InputFloat("X Coordinate", &State.xCoordinate, 0.0f, 0.0f, "%.4f X")) {
                State.Save();
            }

            if (ImGui::InputFloat("Y Coordinate", &State.yCoordinate, 0.0f, 0.0f, "%.4f Y")) {
                State.Save();
            }

            if (ToggleButton("Relative Teleport", &State.RelativeTeleport)) {
                State.Save();
            }
            if (IsInGame() || IsInLobby())
                ImGui::SameLine();
            if ((IsInGame() || IsInLobby()) && ImGui::Button("Get Current Position"))
            {
                Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
                State.xCoordinate = position.x;
                State.yCoordinate = position.y;
            }
            if (IsInGame() || IsInLobby())
                ImGui::SameLine();

            if ((IsInGame() || IsInLobby()) && ImGui::Button("Teleport To"))
            {
                Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
                Vector2 target = { (State.RelativeTeleport ? position.x : 0.f) + State.xCoordinate, (State.RelativeTeleport ? position.y : 0.f) + State.yCoordinate };
                if (IsInGame()) {
                    State.rpcQueue.push(new RpcSnapTo(target));
                }
                else if (IsInLobby()) {
                    State.lobbyRpcQueue.push(new RpcSnapTo(target));
                }
            }
            if (!State.SafeMode && (IsInGame() || IsInLobby())) {
                ImGui::SameLine();
                if (ImGui::Button("Teleport Everyone To"))
                {
                    Vector2 position = GetTrueAdjustedPosition(*Game::pLocalPlayer);
                    Vector2 target = { (State.RelativeTeleport ? position.x : 0.f) + State.xCoordinate, (State.RelativeTeleport ? position.y : 0.f) + State.yCoordinate };
                    std::queue<RPCInterface*>* queue = nullptr;
                    if (IsInGame())
                        queue = &State.rpcQueue;
                    else if (IsInLobby())
                        queue = &State.lobbyRpcQueue;
                    for (auto player : GetAllPlayerControl()) {
                        queue->push(new RpcForceSnapTo(player, target));
                    }
                }
            }

            if (CustomListBoxInt("Select Role", &State.FakeRole, FAKEROLES, 100.0f * State.dpiScale))
                State.Save();
            ImGui::SameLine();
            if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby()) && ImGui::Button("Set Role")) {
                State.FakeRole = std::clamp(State.FakeRole, 0, 7);
                if (IsInGame())
                    State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum(State.FakeRole)));
                else if (IsInLobby())
                    State.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum(State.FakeRole)));
            }
            if (IsHost() || !State.SafeMode) ImGui::SameLine();
            if ((IsHost() || !State.SafeMode) && (IsInGame() || IsInLobby()) && ImGui::Button("Set for Everyone")) {
                if (IsInGame()) {
                    for (auto player : GetAllPlayerControl())
                        State.rpcQueue.push(new RpcSetRole(player, RoleTypes__Enum(State.FakeRole)));
                }
                else if (IsInLobby()) {
                    for (auto player : GetAllPlayerControl())
                        State.lobbyRpcQueue.push(new RpcSetRole(player, RoleTypes__Enum(State.FakeRole)));
                }
            }
            if ((IsInGame() || IsInLobby()) && ImGui::Button("Set Fake Role")) {
                auto localData = GetPlayerData(*Game::pLocalPlayer);
                State.FakeRole = std::clamp(State.FakeRole, 0, 7);
                if (IsInGame())
                    State.rpcQueue.push(new SetRole(RoleTypes__Enum(State.FakeRole)));
                else if (IsInLobby())
                    State.lobbyRpcQueue.push(new SetRole(RoleTypes__Enum(State.FakeRole)));
            }
            ImGui::SameLine();
            if (ToggleButton("Automatically Set Fake Role", &State.AutoFakeRole)) {
                State.Save();
            }
            if (IsInLobby() || IsInGame()) {
                ImGui::SameLine();
                std::string roleText = FAKEROLES[int(State.RealRole)];
                ImGui::Text(("Real Role: " + roleText).c_str());
            }
        }

        if (openRandomizers) {
            if (ToggleButton("Cycler", &State.Cycler)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Cycle in Meeting", &State.CycleInMeeting)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Cycle Between Players", &State.CycleBetweenPlayers)) {
                State.Save();
            }

            if (SteppedSliderFloat("Cycle Timer", &State.CycleTimer, 0.2f, 1.f, 0.02f, "%.2fs", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
                State.PrevCycleTimer = State.CycleTimer;
                State.CycleDuration = State.CycleTimer * 50;
            }

            ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
            if (ImGui::CollapsingHeader("Cycler Options")) {
                if (ToggleButton("Cycle Name", &State.CycleName)) {
                    State.Save();
                }


                ImGui::SameLine(120.0f * State.dpiScale);
                if (ToggleButton("Cycle Color", &State.RandomColor)) {
                    State.Save();
                }

                ImGui::SameLine(240.0f * State.dpiScale);
                if (ToggleButton("Cycle Hat", &State.RandomHat)) {
                    State.Save();
                }

                if (ToggleButton("Cycle Visor", &State.RandomVisor)) {
                    State.Save();
                }

                ImGui::SameLine(120.0f * State.dpiScale);
                if (ToggleButton("Cycle Skin", &State.RandomSkin)) {
                    State.Save();
                }

                ImGui::SameLine(240.0f * State.dpiScale);
                if (ToggleButton("Cycle Pet", &State.RandomPet)) {
                    State.Save();
                }

                if (ToggleButton("Cycle Nameplate", &State.RandomNamePlate)) {
                    State.Save();
                }

                if (IsHost() || !State.SafeMode) {
                    ImGui::SameLine(140.0f * State.dpiScale);
                    if (ToggleButton("Cycle for Everyone (name & color only)", &State.CycleForEveryone)) {
                        State.Save();
                    }
                }
            }


            if (ImGui::CollapsingHeader("Cycler Name Options")) {
                if (CustomListBoxInt("Cycler Name Generation", &State.cyclerNameGeneration, NAMEGENERATION, 75 * State.dpiScale)) {
                    State.Save();
                }
                if (State.cyclerNameGeneration == 2) {
                    if (State.cyclerUserNames.empty())
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username generation will fall back to word combo as you have no names in the cycler.");
                    static std::string newName = "";
                    InputString("New Name", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (ImGui::Button("Add Name")) {
                        State.cyclerUserNames.push_back(newName);
                        State.Save();
                        newName = "";
                    }
                    if (!(IsHost() || !State.SafeMode) && !IsNameValid(newName)) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username will be detected by anticheat. This name will be ignored.");
                    }
                    if (!State.cyclerUserNames.empty()) {
                        static int selectedNameIndex = 0;
                        selectedNameIndex = std::clamp(selectedNameIndex, 0, (int)State.cyclerUserNames.size() - 1);
                        std::vector<const char*> nameVector(State.cyclerUserNames.size(), nullptr);
                        for (size_t i = 0; i < State.cyclerUserNames.size(); i++) {
                            nameVector[i] = State.cyclerUserNames[i].c_str();
                        }
                        CustomListBoxInt("Cycler Name to Delete", &selectedNameIndex, nameVector);
                        ImGui::SameLine();
                        if (ImGui::Button("Delete"))
                            State.cyclerUserNames.erase(State.cyclerUserNames.begin() + selectedNameIndex);
                    }
                }
            }

            if (ToggleButton("Confuser (Randomize Appearance at Will)", &State.confuser)) {
                State.Save();
            }

            if (ImGui::CollapsingHeader("Confuser Options")) {
                if ((IsInGame() || IsInLobby()) && ImGui::Button("Confuse Now")) {
                    ControlAppearance(true);
                }
                if (IsInGame() || IsInLobby()) {
                    if (IsHost() || !State.SafeMode)
                        ImGui::SameLine();
                }
                if ((IsInGame() || IsInLobby()) && (IsHost() || !State.SafeMode) && ImGui::Button("Randomize Everyone")) {
                    std::queue<RPCInterface*>* queue = nullptr;
                    if (IsInGame())
                        queue = &State.rpcQueue;
                    else if (IsInLobby())
                        queue = &State.lobbyRpcQueue;
                    std::vector availableHats = { "hat_NoHat", "hat_AbominalHat", "hat_anchor", "hat_antenna", "hat_Antenna_Black", "hat_arrowhead", "hat_Astronaut-Blue", "hat_Astronaut-Cyan", "hat_Astronaut-Orange", "hat_astronaut", "hat_axe", "hat_babybean", "hat_Baguette", "hat_BananaGreen", "hat_BananaPurple", "hat_bandanaWBY", "hat_Bandana_Blue", "hat_Bandana_Green", "hat_Bandana_Pink", "hat_Bandana_Red", "hat_Bandana_White", "hat_Bandana_Yellow", "hat_baseball_Black", "hat_baseball_Green", "hat_baseball_Lightblue", "hat_baseball_LightGreen", "hat_baseball_Lilac", "hat_baseball_Orange", "hat_baseball_Pink", "hat_baseball_Purple", "hat_baseball_Red", "hat_baseball_White", "hat_baseball_Yellow", "hat_Basketball", "hat_bat_crewcolor", "hat_bat_green", "hat_bat_ice", "hat_beachball", "hat_Beanie_Black", "hat_Beanie_Blue", "hat_Beanie_Green", "hat_Beanie_Lightblue", "hat_Beanie_LightGreen", "hat_Beanie_LightPurple", "hat_Beanie_Pink", "hat_Beanie_Purple", "hat_Beanie_White", "hat_Beanie_Yellow", "hat_bearyCold", "hat_bone", "hat_Bowlingball", "hat_brainslug", "hat_BreadLoaf", "hat_bucket", "hat_bucketHat", "hat_bushhat", "hat_Butter", "hat_caiatl", "hat_caitlin", "hat_candycorn", "hat_captain", "hat_cashHat", "hat_cat_grey", "hat_cat_orange", "hat_cat_pink", "hat_cat_snow", "hat_chalice", "hat_cheeseBleu", "hat_cheeseMoldy", "hat_cheeseSwiss", "hat_ChefWhiteBlue", "hat_cherryOrange", "hat_cherryPink", "hat_Chocolate", "hat_chocolateCandy", "hat_chocolateMatcha", "hat_chocolateVanillaStrawb", "hat_clagger", "hat_clown_purple", "hat_comper", "hat_croissant", "hat_crownBean", "hat_crownDouble", "hat_crownTall", "hat_CuppaJoe", "hat_Deitied", "hat_devilhorns_black", "hat_devilhorns_crewcolor", "hat_devilhorns_green", "hat_devilhorns_murky", "hat_devilhorns_white", "hat_devilhorns_yellow", "hat_Doc_black", "hat_Doc_Orange", "hat_Doc_Purple", "hat_Doc_Red", "hat_Doc_White", "hat_Dodgeball", "hat_Dorag_Black", "hat_Dorag_Desert", "hat_Dorag_Jungle", "hat_Dorag_Purple", "hat_Dorag_Sky", "hat_Dorag_Snow", "hat_Dorag_Yellow", "hat_doubletophat", "hat_DrillMetal", "hat_DrillStone", "hat_DrillWood", "hat_EarmuffGreen", "hat_EarmuffsPink", "hat_EarmuffsYellow", "hat_EarnmuffBlue", "hat_eggGreen", "hat_eggYellow", "hat_enforcer", "hat_erisMorn", "hat_fairywings", "hat_fishCap", "hat_fishhed", "hat_fishingHat", "hat_flowerpot", "hat_frankenbolts", "hat_frankenbride", "hat_fungleFlower", "hat_geoff", "hat_glowstick", "hat_glowstickCyan", "hat_glowstickOrange", "hat_glowstickPink", "hat_glowstickPurple", "hat_glowstickYellow", "hat_goggles", "hat_Goggles_Black", "hat_Goggles_Chrome", "hat_GovtDesert", "hat_GovtHeadset", "hat_halospartan", "hat_hardhat", "hat_Hardhat_black", "hat_Hardhat_Blue", "hat_Hardhat_Green", "hat_Hardhat_Orange", "hat_Hardhat_Pink", "hat_Hardhat_Purple", "hat_Hardhat_Red", "hat_Hardhat_White", "hat_HardtopHat", "hat_headslug_Purple", "hat_headslug_Red", "hat_headslug_White", "hat_headslug_Yellow", "hat_Heart", "hat_heim", "hat_Herohood_Black", "hat_Herohood_Blue", "hat_Herohood_Pink", "hat_Herohood_Purple", "hat_Herohood_Red", "hat_Herohood_Yellow", "hat_hl_fubuki", "hat_hl_gura", "hat_hl_korone", "hat_hl_marine", "hat_hl_mio", "hat_hl_moona", "hat_hl_okayu", "hat_hl_pekora", "hat_hl_risu", "hat_hl_watson", "hat_hunter", "hat_IceCreamMatcha", "hat_IceCreamMint", "hat_IceCreamNeo", "hat_IceCreamStrawberry", "hat_IceCreamUbe", "hat_IceCreamVanilla", "hat_Igloo", "hat_Janitor", "hat_jayce", "hat_jinx", "hat_killerplant", "hat_lilShroom", "hat_maraSov", "hat_mareLwyd", "hat_military", "hat_MilitaryWinter", "hat_MinerBlack", "hat_MinerYellow", "hat_mira_bush", "hat_mira_case", "hat_mira_cloud", "hat_mira_flower", "hat_mira_flower_red", "hat_mira_gem", "hat_mira_headset_blue", "hat_mira_headset_pink", "hat_mira_headset_yellow", "hat_mira_leaf", "hat_mira_milk", "hat_mira_sign_blue", "hat_mohawk_bubblegum", "hat_mohawk_bumblebee", "hat_mohawk_purple_green", "hat_mohawk_rainbow", "hat_mummy", "hat_mushbuns", "hat_mushroomBeret", "hat_mysteryBones", "hat_NewYear2023", "hat_OrangeHat", "hat_osiris", "hat_pack01_Astronaut0001", "hat_pack02_Tengallon0001", "hat_pack02_Tengallon0002", "hat_pack03_Stickynote0004", "hat_pack04_Geoffmask0001", "hat_pack06holiday_candycane0001", "hat_PancakeStack", "hat_paperhat", "hat_Paperhat_Black", "hat_Paperhat_Blue", "hat_Paperhat_Cyan", "hat_Paperhat_Lightblue", "hat_Paperhat_Pink", "hat_Paperhat_Yellow", "hat_papermask", "hat_partyhat", "hat_pickaxe", "hat_Pineapple", "hat_PizzaSliceHat", "hat_pk01_BaseballCap", "hat_pk02_Crown", "hat_pk02_Eyebrows", "hat_pk02_HaloHat", "hat_pk02_HeroCap", "hat_pk02_PipCap", "hat_pk02_PlungerHat", "hat_pk02_ScubaHat", "hat_pk02_StickminHat", "hat_pk02_StrawHat", "hat_pk02_TenGallonHat", "hat_pk02_ThirdEyeHat", "hat_pk02_ToiletPaperHat", "hat_pk02_Toppat", "hat_pk03_Fedora", "hat_pk03_Goggles", "hat_pk03_Headphones", "hat_pk03_Security1", "hat_pk03_StrapHat", "hat_pk03_Traffic", "hat_pk04_Antenna", "hat_pk04_Archae", "hat_pk04_Balloon", "hat_pk04_Banana", "hat_pk04_Bandana", "hat_pk04_Beanie", "hat_pk04_Bear", "hat_pk04_BirdNest", "hat_pk04_CCC", "hat_pk04_Chef", "hat_pk04_DoRag", "hat_pk04_Fez", "hat_pk04_GeneralHat", "hat_pk04_HunterCap", "hat_pk04_JungleHat", "hat_pk04_MinerCap", "hat_pk04_MiniCrewmate", "hat_pk04_Pompadour", "hat_pk04_RamHorns", "hat_pk04_Slippery", "hat_pk04_Snowman", "hat_pk04_Vagabond", "hat_pk04_WinterHat", "hat_pk05_Burthat", "hat_pk05_Cheese", "hat_pk05_cheesetoppat", "hat_pk05_Cherry", "hat_pk05_davehat", "hat_pk05_Egg", "hat_pk05_Ellie", "hat_pk05_EllieToppat", "hat_pk05_Ellryhat", "hat_pk05_Fedora", "hat_pk05_Flamingo", "hat_pk05_FlowerPin", "hat_pk05_GeoffreyToppat", "hat_pk05_Helmet", "hat_pk05_HenryToppat", "hat_pk05_Macbethhat", "hat_pk05_Plant", "hat_pk05_RHM", "hat_pk05_Svenhat", "hat_pk05_Wizardhat", "hat_pk06_Candycanes", "hat_pk06_ElfHat", "hat_pk06_Lights", "hat_pk06_Present", "hat_pk06_Reindeer", "hat_pk06_Santa", "hat_pk06_Snowman", "hat_pk06_tree", "hat_pkHW01_BatWings", "hat_pkHW01_CatEyes", "hat_pkHW01_Horns", "hat_pkHW01_Machete", "hat_pkHW01_Mohawk", "hat_pkHW01_Pirate", "hat_pkHW01_PlagueHat", "hat_pkHW01_Pumpkin", "hat_pkHW01_ScaryBag", "hat_pkHW01_Witch", "hat_pkHW01_Wolf", "hat_Plunger_Blue", "hat_Plunger_Yellow", "hat_police", "hat_Ponytail", "hat_Pot", "hat_Present", "hat_Prototype", "hat_pusheenGreyHat", "hat_PusheenicornHat", "hat_pusheenMintHat", "hat_pusheenPinkHat", "hat_pusheenPurpleHat", "hat_pusheenSitHat", "hat_pusheenSleepHat", "hat_pyramid", "hat_rabbitEars", "hat_Ramhorn_Black", "hat_Ramhorn_Red", "hat_Ramhorn_White", "hat_ratchet", "hat_Records", "hat_RockIce", "hat_RockLava", "hat_Rubberglove", "hat_Rupert", "hat_russian", "hat_saint14", "hat_sausage", "hat_savathun", "hat_schnapp", "hat_screamghostface", "hat_Scrudge", "hat_sharkfin", "hat_shaxx", "hat_shovel", "hat_SlothHat", "hat_SnowbeanieGreen", "hat_SnowbeanieOrange", "hat_SnowBeaniePurple", "hat_SnowbeanieRed", "hat_Snowman", "hat_Soccer", "hat_Sorry", "hat_starBalloon", "hat_starhorse", "hat_Starless", "hat_StarTopper", "hat_stethescope", "hat_StrawberryLeavesHat", "hat_TenGallon_Black", "hat_TenGallon_White", "hat_ThomasC", "hat_tinFoil", "hat_titan", "hat_ToastButterHat", "hat_tombstone", "hat_tophat", "hat_ToppatHair", "hat_towelwizard", "hat_Traffic_Blue", "hat_traffic_purple", "hat_Traffic_Red", "hat_Traffic_Yellow", "hat_Unicorn", "hat_vi", "hat_viking", "hat_Visor", "hat_Voleyball", "hat_w21_candycane_blue", "hat_w21_candycane_bubble", "hat_w21_candycane_chocolate", "hat_w21_candycane_mint", "hat_w21_elf_pink", "hat_w21_elf_swe", "hat_w21_gingerbread", "hat_w21_holly", "hat_w21_krampus", "hat_w21_lights_white", "hat_w21_lights_yellow", "hat_w21_log", "hat_w21_mistletoe", "hat_w21_mittens", "hat_w21_nutcracker", "hat_w21_pinecone", "hat_w21_present_evil", "hat_w21_present_greenyellow", "hat_w21_present_redwhite", "hat_w21_present_whiteblue", "hat_w21_santa_evil", "hat_w21_santa_green", "hat_w21_santa_mint", "hat_w21_santa_pink", "hat_w21_santa_white", "hat_w21_santa_yellow", "hat_w21_snowflake", "hat_w21_snowman", "hat_w21_snowman_evil", "hat_w21_snowman_greenred", "hat_w21_snowman_redgreen", "hat_w21_snowman_swe", "hat_w21_winterpuff", "hat_wallcap", "hat_warlock", "hat_whitetophat", "hat_wigJudge", "hat_wigTall", "hat_WilfordIV", "hat_Winston", "hat_WinterGreen", "hat_WinterHelmet", "hat_WinterRed", "hat_WinterYellow", "hat_witch_green", "hat_witch_murky", "hat_witch_pink", "hat_witch_white", "hat_wolf_grey", "hat_wolf_murky", "hat_Zipper" };
                    std::vector availableSkins = { "skin_None", "skin_Abominalskin", "skin_ApronGreen", "skin_Archae", "skin_Astro", "skin_Astronaut-Blueskin", "skin_Astronaut-Cyanskin", "skin_Astronaut-Orangeskin", "skin_Bananaskin", "skin_benoit", "skin_Bling", "skin_BlueApronskin", "skin_BlueSuspskin", "skin_Box1skin", "skin_BubbleWrapskin", "skin_Burlapskin", "skin_BushSign1skin", "skin_Bushskin", "skin_BusinessFem-Aquaskin", "skin_BusinessFem-Tanskin", "skin_BusinessFemskin", "skin_caitlin", "skin_Capt", "skin_CCC", "skin_ChefBlackskin", "skin_ChefBlue", "skin_ChefRed", "skin_clown", "skin_D2Cskin", "skin_D2Hunter", "skin_D2Osiris", "skin_D2Saint14", "skin_D2Shaxx", "skin_D2Titan", "skin_D2Warlock", "skin_enforcer", "skin_fairy", "skin_FishingSkinskin", "skin_fishmonger", "skin_FishSkinskin", "skin_General", "skin_greedygrampaskin", "skin_halospartan", "skin_Hazmat-Blackskin", "skin_Hazmat-Blueskin", "skin_Hazmat-Greenskin", "skin_Hazmat-Pinkskin", "skin_Hazmat-Redskin", "skin_Hazmat-Whiteskin", "skin_Hazmat", "skin_heim", "skin_hl_fubuki", "skin_hl_gura", "skin_hl_korone", "skin_hl_marine", "skin_hl_mio", "skin_hl_moona", "skin_hl_okayu", "skin_hl_pekora", "skin_hl_risu", "skin_hl_watson", "skin_Horse1skin", "skin_Hotdogskin", "skin_InnerTubeSkinskin", "skin_JacketGreenskin", "skin_JacketPurpleskin", "skin_JacketYellowskin", "skin_Janitorskin", "skin_jayce", "skin_jinx", "skin_LifeVestSkinskin", "skin_Mech", "skin_MechanicRed", "skin_Military", "skin_MilitaryDesert", "skin_MilitarySnowskin", "skin_Miner", "skin_MinerBlackskin", "skin_mummy", "skin_OrangeSuspskin", "skin_PinkApronskin", "skin_PinkSuspskin", "skin_Police", "skin_presentskin", "skin_prisoner", "skin_PrisonerBlue", "skin_PrisonerTanskin", "skin_pumpkin", "skin_PusheenGreyskin", "skin_Pusheenicornskin", "skin_PusheenMintskin", "skin_PusheenPinkskin", "skin_PusheenPurpleskin", "skin_ratchet", "skin_rhm", "skin_RockIceskin", "skin_RockLavaskin", "skin_Sack1skin", "skin_scarfskin", "skin_Science", "skin_Scientist-Blueskin", "skin_Scientist-Darkskin", "skin_screamghostface", "skin_Security", "skin_Skin_SuitRedskin", "skin_Slothskin", "skin_SportsBlueskin", "skin_SportsRedskin", "skin_SuitB", "skin_SuitW", "skin_SweaterBlueskin", "skin_SweaterPinkskin", "skin_Sweaterskin", "skin_SweaterYellowskin", "skin_Tarmac", "skin_ToppatSuitFem", "skin_ToppatVest", "skin_uglysweaterskin", "skin_vampire", "skin_vi", "skin_w21_deer", "skin_w21_elf", "skin_w21_msclaus", "skin_w21_nutcracker", "skin_w21_santa", "skin_w21_snowmate", "skin_w21_tree", "skin_Wall", "skin_Winter", "skin_witch", "skin_YellowApronskin", "skin_YellowSuspskin" };
                    std::vector availableVisors = { "visor_EmptyVisor", "visor_anime", "visor_BaconVisor", "visor_BananaVisor", "visor_beautyMark", "visor_BillyG", "visor_Blush", "visor_Bomba", "visor_BubbleBumVisor", "visor_Candycane", "visor_Carrot", "visor_chimkin", "visor_clownnose", "visor_Crack", "visor_CucumberVisor", "visor_D2CGoggles", "visor_Dirty", "visor_Dotdot", "visor_doubleeyepatch", "visor_eliksni", "visor_erisBandage", "visor_eyeball", "visor_EyepatchL", "visor_EyepatchR", "visor_fishhook", "visor_Galeforce", "visor_heim", "visor_hl_ah", "visor_hl_bored", "visor_hl_hmph", "visor_hl_marine", "visor_hl_nothoughts", "visor_hl_nudge", "visor_hl_smug", "visor_hl_sweepy", "visor_hl_teehee", "visor_hl_wrong", "visor_IceBeard", "visor_IceCreamChocolateVisor", "visor_IceCreamMintVisor", "visor_IceCreamStrawberryVisor", "visor_IceCreamUbeVisor", "visor_is_beard", "visor_JanitorStache", "visor_jinx", "visor_Krieghaus", "visor_Lava", "visor_LolliBlue", "visor_LolliBrown", "visor_LolliOrange", "visor_lollipopCrew", "visor_lollipopLemon", "visor_lollipopLime", "visor_LolliRed", "visor_marshmallow", "visor_masque_blue", "visor_masque_green", "visor_masque_red", "visor_masque_white", "visor_mira_card_blue", "visor_mira_card_red", "visor_mira_glasses", "visor_mira_mask_black", "visor_mira_mask_blue", "visor_mira_mask_green", "visor_mira_mask_purple", "visor_mira_mask_red", "visor_mira_mask_white", "visor_Mouth", "visor_mummy", "visor_PiercingL", "visor_PiercingR", "visor_PizzaVisor", "visor_pk01_AngeryVisor", "visor_pk01_DumStickerVisor", "visor_pk01_FredVisor", "visor_pk01_HazmatVisor", "visor_pk01_MonoclesVisor", "visor_pk01_PaperMaskVisor", "visor_pk01_PlagueVisor", "visor_pk01_RHMVisor", "visor_pk01_Security1Visor", "visor_Plsno", "visor_polus_ice", "visor_pusheenGorgeousVisor", "visor_pusheenKissyVisor", "visor_pusheenKoolKatVisor", "visor_pusheenOmNomNomVisor", "visor_pusheenSmileVisor", "visor_pusheenYaaaaaayVisor", "visor_Reginald", "visor_Rudolph", "visor_savathun", "visor_Scar", "visor_SciGoggles", "visor_shopglasses", "visor_shuttershadesBlue", "visor_shuttershadesLime", "visor_shuttershadesPink", "visor_shuttershadesPurple", "visor_shuttershadesWhite", "visor_shuttershadesYellow", "visor_SkiGoggleBlack", "visor_SKiGogglesOrange", "visor_SkiGogglesWhite", "visor_SmallGlasses", "visor_SmallGlassesBlue", "visor_SmallGlassesRed", "visor_starfish", "visor_Stealthgoggles", "visor_Stickynote_Cyan", "visor_Stickynote_Green", "visor_Stickynote_Orange", "visor_Stickynote_Pink", "visor_Stickynote_Purple", "visor_Straw", "visor_sunscreenv", "visor_teary", "visor_ToastVisor", "visor_tvColorTest", "visor_vr_Vr-Black", "visor_vr_Vr-White", "visor_w21_carrot", "visor_w21_nutstache", "visor_w21_nye", "visor_w21_santabeard", "visor_wash", "visor_WinstonStache" };
                    std::vector availablePets = { "pet_EmptyPet", "pet_Alien", "pet_Bedcrab", "pet_BredPet", "pet_Bush", "pet_Charles", "pet_Charles_Red", "pet_ChewiePet", "pet_clank", "pet_coaltonpet", "pet_Creb", "pet_Crewmate", "pet_Cube", "pet_D2GhostPet", "pet_D2PoukaPet", "pet_D2WormPet", "pet_Doggy", "pet_Ellie", "pet_frankendog", "pet_GuiltySpark", "pet_HamPet", "pet_Hamster", "pet_HolidayHamPet", "pet_Lava", "pet_nuggetPet", "pet_Pip", "pet_poro", "pet_Pusheen", "pet_Robot", "pet_Snow", "pet_Squig", "pet_Stickmin", "pet_Stormy", "pet_test", "pet_UFO", "pet_YuleGoatPet" };
                    std::vector availableNamePlates = { "nameplate_NoPlate", "nameplate_cliffs", "nameplate_grill", "nameplate_plant", "nameplate_sandcastle", "nameplate_zipline", "nameplate_pusheen_01", "nameplate_pusheen_02", "nameplate_pusheen_03", "nameplate_pusheen_04", "nameplate_flagAro", "nameplate_flagMlm", "nameplate_hunter", "nameplate_Polus_DVD", "nameplate_Polus_Ground", "nameplate_Polus_Lava", "nameplate_Polus_Planet", "nameplate_Polus_Snow", "nameplate_Polus_SpecimenBlue", "nameplate_Polus_SpecimenGreen", "nameplate_Polus_SpecimenPurple", "nameplate_is_yard", "nameplate_is_dig", "nameplate_is_game", "nameplate_is_ghost", "nameplate_is_green", "nameplate_is_sand", "nameplate_is_trees", "nameplate_Mira_Cafeteria", "nameplate_Mira_Glass", "nameplate_Mira_Tiles", "nameplate_Mira_Vines", "nameplate_Mira_Wood", "nameplate_hw_candy", "nameplate_hw_woods", "nameplate_hw_pumpkin" };
                    //help me out with the nameplates, couldn't find them in the game assets
                    for (auto player : GetAllPlayerControl()) {
                        std::string name = "";
                        if (State.confuserNameGeneration == 0 || (State.cyclerNameGeneration == 2 && State.cyclerUserNames.empty()))
                            name = GenerateRandomString();
                        else if (State.confuserNameGeneration == 1)
                            name = GenerateRandomString(true);
                        else if (State.confuserNameGeneration == 2) {
                            if (!State.cyclerUserNames.empty())
                                name = State.cyclerUserNames[randi(0, State.cyclerUserNames.size() - 1)] + "<size=0>" + std::to_string(player->fields.PlayerId) + "</size>";
                        }
                        else
                            name = GenerateRandomString();
                        queue->push(new RpcForceName(player, name));
                        queue->push(new RpcForceColor(player, randi(0, 17)));
                        queue->push(new RpcForceHat(player, convert_to_string(availableHats[randi(0, availableHats.size() - 1)])));
                        queue->push(new RpcForceSkin(player, convert_to_string(availableSkins[randi(0, availableSkins.size() - 1)])));
                        queue->push(new RpcForceVisor(player, convert_to_string(availableVisors[randi(0, availableVisors.size() - 1)])));
                        queue->push(new RpcForcePet(player, convert_to_string(availablePets[randi(0, availablePets.size() - 1)])));
                        queue->push(new RpcForceNamePlate(player, convert_to_string(availableNamePlates[randi(0, availableNamePlates.size() - 1)])));
                    }
                }
                
                ImGui::Text("Confuse when:");
                if (ToggleButton("Joining Lobby", &State.confuseOnJoin)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Game Starts", &State.confuseOnStart)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Killing", &State.confuseOnKill)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Venting", &State.confuseOnVent)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (ToggleButton("Meeting", &State.confuseOnMeeting)) {
                    State.Save();
                }
            }
            if (ImGui::CollapsingHeader("Confuser Name Options")) {
                if (CustomListBoxInt("Confuser Name Generation", &State.confuserNameGeneration, NAMEGENERATION, 75 * State.dpiScale)) {
                    State.Save();
                }
                if (State.confuserNameGeneration == 2) {
                    if (State.cyclerUserNames.empty())
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username generation will fall back to word combo as you have no names in the cycler.");
                    static std::string newName = "";
                    InputString("New Name ", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (ImGui::Button("Add Name ")) {
                        State.cyclerUserNames.push_back(newName);
                        State.Save();
                        newName = "";
                    }
                    if (!(IsHost() || !State.SafeMode) && !IsNameValid(newName)) {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username will be detected by anticheat. This name will be ignored.");
                    }
                    if (!State.cyclerUserNames.empty()) {
                        static int selectedNameIndex = 0;
                        selectedNameIndex = std::clamp(selectedNameIndex, 0, (int)State.cyclerUserNames.size() - 1);
                        std::vector<const char*> nameVector(State.cyclerUserNames.size(), nullptr);
                        for (size_t i = 0; i < State.cyclerUserNames.size(); i++) {
                            nameVector[i] = State.cyclerUserNames[i].c_str();
                        }
                        CustomListBoxInt("Confuser Name to Delete", &selectedNameIndex, nameVector);
                        ImGui::SameLine();
                        if (ImGui::Button("Delete "))
                            State.cyclerUserNames.erase(State.cyclerUserNames.begin() + selectedNameIndex);
                    }
                }
            }
        }
        ImGui::EndChild();
    }
}