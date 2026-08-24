#include "pch-il2cpp.h"
#include "host_tab.h"
#include "utility.h"
#include "game.h"
#include "state.hpp"
#include "gui-helpers.hpp"
#include "_hooks.h"
#include "sabotage_tab.h"

bool editingAutoStartPlayerCount = false;

namespace HostTab {
    enum Groups {
        Utils,
        Settings,
        Tournaments,
        Moderation
    };

    static bool openUtils = true; //default to utils tab group
    static bool openSettings = false;
    static bool openTournaments = false;
    static bool openModeration = false;

    static bool hideRolesList = false;

    void CloseOtherGroups(Groups group) {
        openUtils = group == Groups::Utils;
        openSettings = group == Groups::Settings;
        openTournaments = group == Groups::Tournaments;
        openModeration = group == Groups::Moderation;
    }

    void OpenSubGroup(const std::string& name) {
        if (name == "Utils") CloseOtherGroups(Groups::Utils);
        else if (name == "Settings") CloseOtherGroups(Groups::Settings);
        else if (name == "Tournaments" && State.TournamentMode) CloseOtherGroups(Groups::Tournaments);
        else if (name == "Moderation" && State.Mod_EnableModeration) CloseOtherGroups(Groups::Moderation);
    }

    /*std::string GetPlayerNameFromFriendCode(std::string friendCode) {
        for (auto p : GetAllPlayerData()) {
            if (p->fields.FriendCode == convert_to_string(friendCode))
                return convert_from_string(GetPlayerOutfit(p)->fields.PlayerName);
        }
        return "";
    }*/ //use if needed

    std::string DisplayScore(float f) {
        return std::format("{}", f == (int)f ? (int)f : f);
    }

    static void SetRoleAmount(RoleTypes__Enum type, int amount, GameOptions& options) {
        auto&& roleOpts = options.GetRoleOptions();
        auto maxCount = roleOpts.GetNumPerGame(type);
        if (amount > maxCount)
            roleOpts.SetRoleRate(type, amount, 100);
        else if (amount > 0)
            roleOpts.SetRoleRate(type, maxCount, 100);
    }

    void SyncAllSettings() {
        if (IsInGame()) State.rpcQueue.push(new RpcSyncSettings());
        if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSyncSettings());
    }

    const ptrdiff_t GetRoleCount(RoleType role)
    {
        return std::count_if(State.assignedRoles.cbegin(), State.assignedRoles.cend(), [role](RoleType i) {return i == role; });
    }

    static bool CaptureHostPreset(Settings::HostPreset& p) {
        if (!GameOptions().HasOptions()) return false;
        GameOptions o;
        auto ro = o.GetRoleOptions();
        p.PlayerSpeed = o.GetFloat(app::FloatOptionNames__Enum::PlayerSpeedMod);
        p.CrewmateVision = o.GetFloat(app::FloatOptionNames__Enum::CrewLightMod);
        p.ImpostorVision = o.GetFloat(app::FloatOptionNames__Enum::ImpostorLightMod);
        p.KillCooldown = o.GetFloat(app::FloatOptionNames__Enum::KillCooldown);
        p.KillDistance = o.GetInt(app::Int32OptionNames__Enum::KillDistance);
        p.NumImpostors = o.GetNumImpostors();
        p.MaxPlayers = o.GetMaxPlayers();
        p.MapId = o.GetMapId();
        p.VisualTasks = o.GetBool(app::BoolOptionNames__Enum::VisualTasks);
        p.ConfirmImpostor = o.GetBool(app::BoolOptionNames__Enum::ConfirmImpostor);
        p.AnonymousVotes = o.GetBool(app::BoolOptionNames__Enum::AnonymousVotes);
        p.NumEmergencyMeetings = o.GetInt(app::Int32OptionNames__Enum::NumEmergencyMeetings);
        p.EmergencyCooldown = o.GetInt(app::Int32OptionNames__Enum::EmergencyCooldown);
        p.DiscussionTime = o.GetInt(app::Int32OptionNames__Enum::DiscussionTime);
        p.VotingTime = o.GetInt(app::Int32OptionNames__Enum::VotingTime);
        p.TaskBarMode = o.GetInt(app::Int32OptionNames__Enum::TaskBarMode);
        p.NumCommonTasks = o.GetInt(app::Int32OptionNames__Enum::NumCommonTasks);
        p.NumLongTasks = o.GetInt(app::Int32OptionNames__Enum::NumLongTasks);
        p.NumShortTasks = o.GetInt(app::Int32OptionNames__Enum::NumShortTasks);
        p.ShapeshifterCooldown = o.GetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown);
        p.ShapeshifterDuration = o.GetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration);
        p.ShapeshifterLeaveSkin = o.GetBool(app::BoolOptionNames__Enum::ShapeshifterLeaveSkin);
        p.GuardianAngelCooldown = o.GetFloat(app::FloatOptionNames__Enum::GuardianAngelCooldown);
        p.GuardianAngelProtectVisible = o.GetBool(app::BoolOptionNames__Enum::ImpostorsCanSeeProtect);
        p.GuardianAngelProtectDuration = o.GetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds);
        p.ScientistCooldown = o.GetFloat(app::FloatOptionNames__Enum::ScientistCooldown);
        p.ScientistBatteryCharge = o.GetFloat(app::FloatOptionNames__Enum::ScientistBatteryCharge);
        p.EngineerCooldown = o.GetFloat(app::FloatOptionNames__Enum::EngineerCooldown);
        p.EngineerInVentMaxTime = o.GetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime);
        p.PhantomCooldown = o.GetFloat(app::FloatOptionNames__Enum::PhantomCooldown);
        p.PhantomDuration = o.GetFloat(app::FloatOptionNames__Enum::PhantomDuration);
        p.TrackerCooldown = o.GetFloat(app::FloatOptionNames__Enum::TrackerCooldown);
        p.TrackerDuration = o.GetFloat(app::FloatOptionNames__Enum::TrackerDuration);
        p.TrackerDelay = o.GetFloat(app::FloatOptionNames__Enum::TrackerDelay);
        p.NoisemakerAlertDuration = o.GetFloat(app::FloatOptionNames__Enum::NoisemakerAlertDuration);
        p.NoisemakerImpostorAlert = o.GetBool(app::BoolOptionNames__Enum::NoisemakerImpostorAlert);
        p.ViperDissolveTime = o.GetFloat(app::FloatOptionNames__Enum::ViperDissolveTime);
        p.DetectiveSuspectLimit = o.GetFloat(app::FloatOptionNames__Enum::DetectiveSuspectLimit);
        p.JudgeTaskRequirement = o.GetFloat(app::FloatOptionNames__Enum::JudgeTaskRequirementPercentage);
        static const app::RoleTypes__Enum roles[] = {
            app::RoleTypes__Enum::Scientist, app::RoleTypes__Enum::Engineer,
            app::RoleTypes__Enum::GuardianAngel, app::RoleTypes__Enum::Shapeshifter,
            app::RoleTypes__Enum::Noisemaker, app::RoleTypes__Enum::Phantom,
            app::RoleTypes__Enum::Tracker, app::RoleTypes__Enum::Detective,
            app::RoleTypes__Enum::Viper, RoleTypes__Enum::Judge
        };
        for (auto role : roles) {
            Settings::RolePreset rp;
            rp.Count = ro.GetNumPerGame(role);
            rp.Chance = ro.GetChancePerGame(role);
            p.RoleRates[(int)role] = rp;
        }
        return true;
    }

    void Render() {
        if (IsHost()) {
            ColorMapping ROLE_NAMES_COLOR[] = {
                {"Random",			ImVec4(1.f, 1.f, 1.f, 1.f)},
                {"Crewmate",		State.CrewmateColor},
                {"Scientist",		State.ScientistColor},
                {"Engineer",		State.EngineerColor},
                {"Noisemaker",		State.NoisemakerColor},
                {"Tracker",			State.TrackerColor},
                {"Detective",		State.DetectiveColor},
                {"Judge",           State.JudgeColor},
                {"Impostor",		State.ImpostorColor},
                {"Shapeshifter",	State.ShapeshifterColor},
                {"Phantom",			State.PhantomColor},
                {"Viper",			State.ViperColor},
            }; // needs to be updated every render
            ColorMapping GAMEENDREASONCOLORS[] = {
                {"Crewmates (Votes)", State.CrewmateColor},
                {"Crewmates (Tasks)", State.CrewmateColor},
                {"Impostors (Votes)", State.ImpostorColor},
                {"Impostors (Kill)", State.ImpostorColor},
                {"Impostors (Sabotage)", State.ImpostorColor},
                {"D/C (Imp)", State.ImpostorColor},
                {"D/C (Crew)", State.CrewmateColor},
                {"Timer (HNS)", State.CrewmateColor},
                {"Kill (HNS)", State.ImpostorColor},
            }; // same here

            ImGui::SameLine(100 * State.dpiScale);
            ImGui::BeginChild("###Host", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
            ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
            if (TabGroup("Utils", openUtils)) {
                CloseOtherGroups(Groups::Utils);
            }
            if (GameOptions().HasOptions()) {
                ImGui::SameLine();
                if (TabGroup("Settings", openSettings)) {
                    CloseOtherGroups(Groups::Settings);
                }
            }
            if (State.TournamentMode) {
                ImGui::SameLine();
                if (TabGroup("Tournaments", openTournaments)) {
                    CloseOtherGroups(Groups::Tournaments);
                }
            }
            if (State.Mod_EnableModeration) {
                ImGui::SameLine();
                if (TabGroup("Moderation", openModeration)) {
                    CloseOtherGroups(Groups::Moderation);
                }
            }
            GameOptions options;
            if (openUtils) {
                if (IsInLobby()) {
                    ImGui::Dummy(ImVec2(0, 2) * State.dpiScale);
                    ImGui::BeginChild("host#list", ImVec2(200, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
                    if (!State.DisableRoleManager && (!hideRolesList || !State.TournamentMode)) {
                        bool shouldEndListBox = ImGui::ListBoxHeader("Choose Roles", ImVec2(200, 290) * State.dpiScale);
                        auto allPlayers = GetAllPlayerData();
                        auto playerAmount = allPlayers.size();
                        auto maxImpostorAmount = GetMaxImpostorAmount((int)playerAmount);
                        for (size_t listIndex = 0; listIndex < playerAmount; listIndex++) {
                            auto playerData = allPlayers[listIndex];
                            if (playerData == nullptr) continue;
                            PlayerControl* playerCtrl = GetPlayerControlById(playerData->fields.PlayerId);
                            if (playerCtrl == nullptr) continue;
                            size_t index = playerData->fields.PlayerId; // stable per-player key, doesn't shift when others join/leave
                            State.assignedRolesPlayer[index] = playerCtrl;
                            if (State.assignedRolesPlayer[index] == nullptr)
                                continue;

                            auto outfit = GetPlayerOutfit(playerData);
                            if (outfit == NULL) continue;
                            //ImVec4 the_info = AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId));
                            //char hex_buf[10];
                            //// Format as #AABBGGRR (standard alpha-first hex)
                            //std::snprintf(hex_buf, sizeof(hex_buf), "#%02X%02X%02X%02X",
                            //    (int)(the_info.w * 255.0f), // Alpha
                            //    (int)(the_info.z * 255.0f)  // Blue
                            //    (int)(the_info.y * 255.0f), // Green
                            //    (int)(the_info.x * 255.0f), // Red
                            //);
                            //const std::string playerName = hex_buf;
                            const std::string& playerName = convert_from_string(outfit->fields.PlayerName);
                            //player colors in host tab by gdjkhp (https://github.com/GDjkhp/AmongUsMenu/commit/53b017183bac503c546f198e2bc03539a338462c)
							//now with role colors in role selection
                            if (CustomListBoxIntColored((playerName + "###" + ToString(playerData)).c_str(), reinterpret_cast<int*>(&State.assignedRoles[index]), ROLE_NAMES, 80 * State.dpiScale, AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId)), 0, RemoveHtmlTags(playerName).c_str(), ROLE_NAMES_COLOR, IM_ARRAYSIZE(ROLE_NAMES_COLOR)))
                            {
                                State.engineers_amount = (int)GetRoleCount(RoleType::Engineer);
                                State.scientists_amount = (int)GetRoleCount(RoleType::Scientist);
                                State.trackers_amount = (int)GetRoleCount(RoleType::Tracker);
                                State.noisemakers_amount = (int)GetRoleCount(RoleType::Noisemaker);
                                State.detectives_amount = (int)GetRoleCount(RoleType::Detective);
                                State.judges_amount = (int)GetRoleCount(RoleType::Judge);
                                State.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
                                State.phantoms_amount = (int)GetRoleCount(RoleType::Phantom);
                                State.vipers_amount = (int)GetRoleCount(RoleType::Viper);
                                State.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
                                if (State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount + State.vipers_amount > maxImpostorAmount)
                                {
                                    if (State.assignedRoles[index] == RoleType::Impostor)
                                        State.assignedRoles[index] = RoleType::Random;
                                    else if (State.assignedRoles[index] == RoleType::Shapeshifter)
                                        State.assignedRoles[index] = RoleType::Random;
                                    else if (State.assignedRoles[index] == RoleType::Phantom)
                                        State.assignedRoles[index] = RoleType::Random;
                                    else if (State.assignedRoles[index] == RoleType::Viper)
                                        State.assignedRoles[index] = RoleType::Random;
                                    State.shapeshifters_amount = (int)GetRoleCount(RoleType::Shapeshifter);
                                    State.phantoms_amount = (int)GetRoleCount(RoleType::Phantom);
                                    State.vipers_amount = (int)GetRoleCount(RoleType::Viper);
                                    State.impostors_amount = (int)GetRoleCount(RoleType::Impostor);
                                    State.crewmates_amount = (int)GetRoleCount(RoleType::Crewmate);
                                }
                                if (State.assignedRoles[index] == RoleType::Engineer || State.assignedRoles[index] == RoleType::Scientist ||
                                    State.assignedRoles[index] == RoleType::Tracker || State.assignedRoles[index] == RoleType::Noisemaker ||
                                    State.assignedRoles[index] == RoleType::Detective || State.assignedRoles[index] == RoleType::Judge ||
                                    State.assignedRoles[index] == RoleType::Crewmate) {
                                    if (State.engineers_amount + State.scientists_amount + State.trackers_amount + State.noisemakers_amount + State.detectives_amount + State.judges_amount + State.crewmates_amount >= (int)playerAmount)
                                        State.assignedRoles[index] = RoleType::Random;
                                } //Some may set all players to non imps. This hangs the game on beginning. Leave space to Random so we have imps. 

                                if (options.GetGameMode() == GameModes__Enum::HideNSeek)
                                {
                                    if (State.assignedRoles[index] == RoleType::Shapeshifter)
                                        State.assignedRoles[index] = RoleType::Impostor;
                                    else if (State.assignedRoles[index] == RoleType::Phantom)
                                        State.assignedRoles[index] = RoleType::Impostor;
                                    else if (State.assignedRoles[index] == RoleType::Viper)
                                        State.assignedRoles[index] = RoleType::Impostor;
                                    else if (State.assignedRoles[index] == RoleType::Tracker)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                    else if (State.assignedRoles[index] == RoleType::Noisemaker)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                    else if (State.assignedRoles[index] == RoleType::Detective)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                    else if (State.assignedRoles[index] == RoleType::Scientist)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                    else if (State.assignedRoles[index] == RoleType::Judge)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                    else if (State.assignedRoles[index] == RoleType::Crewmate)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                    else if (State.assignedRoles[index] == RoleType::Engineer) // what?! lmao (see line 98)
                                        State.assignedRoles[index] = RoleType::Engineer;
                                } //Assign other roles in hidenseek causes game bug.
                                //These are organized. Do not change the order unless you find it necessary.

                                if (!IsInGame()) {
                                    if (options.GetGameMode() == GameModes__Enum::HideNSeek)
                                        SetRoleAmount(RoleTypes__Enum::Engineer, 15, options);
                                    else
                                        SetRoleAmount(RoleTypes__Enum::Engineer, State.engineers_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Scientist, State.scientists_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Tracker, State.trackers_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Noisemaker, State.noisemakers_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Detective, State.detectives_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Judge, State.judges_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Shapeshifter, State.shapeshifters_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Phantom, State.phantoms_amount, options);
                                    SetRoleAmount(RoleTypes__Enum::Viper, State.vipers_amount, options);
                                    if (options.GetNumImpostors() <= State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount + State.vipers_amount)
                                        options.SetInt(app::Int32OptionNames__Enum::NumImpostors, State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount + State.vipers_amount);
                                }
                            }
                        }
                        if (shouldEndListBox)
                            ImGui::ListBoxFooter();
                    }
                    if (!State.DisableRoleManager) ImGui::Dummy(ImVec2(2, 2) * State.dpiScale);
                    ToggleButton("Disable Role Selection", &State.DisableRoleManager);

                    if (State.TournamentMode) {
                        if (AnimatedButton("Randomize Roles")) {
                            std::vector<Game::PlayerId> playerIds = {};
                            std::vector<Game::PlayerId> impostorIds = {};
                            for (auto p : GetAllPlayerControl()) {
                                if (p == NULL || GetPlayerData(p) == NULL) continue;
                                playerIds.push_back(p->fields.PlayerId);
                            }
                            int maxImpostors = (std::min)((int)GetAllPlayerControl().size(), GetMaxImpostorAmount((int)GetAllPlayerControl().size()));
                            for (int i = 0; i < maxImpostors; ++i) {
                                Game::PlayerId randImpostorId = playerIds[randi(0, (int)playerIds.size() - 1)];
                                impostorIds.push_back(randImpostorId);
                                playerIds.erase(std::find(playerIds.begin(), playerIds.end(), randImpostorId));
                                State.assignedRoles[randImpostorId] = RoleType::Impostor;
                            }
                            for (auto i : playerIds)
                                State.assignedRoles[i] = RoleType::Crewmate;
                        }
                        ToggleButton("Hide Roles List", &hideRolesList);
                    }

                    if (!State.DisableRoleManager) {
                        if (ToggleButton("Always", &State.AutoHostRole)) {
                            State.Save();

                            if (!State.AutoHostRole) {
                                auto allPlayers = GetAllPlayerData();
                                for (size_t listIndex = 0; listIndex < allPlayers.size(); listIndex++) {
                                    auto playerData = allPlayers[listIndex];
                                    if (playerData == nullptr) continue;
                                    PlayerControl* playerCtrl = GetPlayerControlById(playerData->fields.PlayerId);
                                    if (playerCtrl == nullptr) continue;

                                    if (*Game::pLocalPlayer == playerCtrl) {
                                        State.assignedRoles[playerData->fields.PlayerId] = RoleType::Random;
                                        break;
                                    }
                                }
                            }
                        }
                        ImGui::SameLine();
                        int hostRoleInt = (int)State.HostRoleToSet;
                        if (CustomListBoxIntColored("###RoleSelector", &hostRoleInt, ROLE_NAMES, 80 * State.dpiScale, ImVec4(1.f, 1.f, 1.f, 0.f), 0, "", ROLE_NAMES_COLOR, IM_ARRAYSIZE(ROLE_NAMES_COLOR))) {
                            if (State.HostRoleToSet == RoleType::Impostor || State.HostRoleToSet == RoleType::Shapeshifter || State.HostRoleToSet == RoleType::Phantom || State.HostRoleToSet == RoleType::Viper) {
                                if (State.impostors_amount + State.shapeshifters_amount + State.phantoms_amount + State.vipers_amount + 1 > GetMaxImpostorAmount((int)GetAllPlayerData().size())) {
                                    State.AutoHostRole = false;
                                }
                                else {
                                    if (options.GetGameMode() == GameModes__Enum::HideNSeek) State.HostRoleToSet = RoleType::Impostor;
                                }
                            }
                            else {
                                if (State.engineers_amount + State.scientists_amount + State.trackers_amount + State.noisemakers_amount + State.detectives_amount + State.judges_amount + State.crewmates_amount + 1 >= (int)GetAllPlayerData().size()) {
                                    State.AutoHostRole = false;
                                }
                                else {
                                    if (options.GetGameMode() == GameModes__Enum::HideNSeek) State.HostRoleToSet = RoleType::Engineer;
                                }
                            }
                            State.HostRoleToSet = (RoleType)hostRoleInt;
                            State.Save();
                        }
                    }
                    ImGui::EndChild();
                }
                if (IsInLobby()) ImGui::SameLine();
                ImGui::BeginChild("host#actions", ImVec2(300, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);

                if (!State.DisableRoleManager && IsInLobby()) {
                    if (ToggleButton("Custom Impostor Amount", &State.CustomImpostorAmount))
                        State.Save();
                    State.ImpostorCount = std::clamp(State.ImpostorCount, 0, int(Game::MAX_PLAYERS));
                    if (State.CustomImpostorAmount && ImGui::InputInt("Impostor Count", &State.ImpostorCount))
                        State.Save();
                }

                const int32_t currentMaxPlayers = options.GetMaxPlayers();
                const int32_t minPlayers = 4, maxAllowedPlayers = static_cast<int32_t>(Game::MAX_PLAYERS);
                int32_t newMaxPlayers = std::clamp(currentMaxPlayers, minPlayers, maxAllowedPlayers);
#define LocalInLobby (((*Game::pAmongUsClient)->fields._.NetworkMode == NetworkModes__Enum::LocalGame) && ((*Game::pAmongUsClient)->fields._.GameState == InnerNetClient_GameStates__Enum::Joined))
                if ((LocalInLobby || !State.SafeMode) && IsInLobby() && ImGui::InputInt("Max Players", &newMaxPlayers)) {
                    newMaxPlayers = std::clamp(newMaxPlayers, minPlayers, maxAllowedPlayers);
                    GameOptions().SetInt(app::Int32OptionNames__Enum::MaxPlayers, newMaxPlayers);
                    SyncAllSettings();
                }


                /*if (IsInLobby() && ToggleButton("Flip Skeld", &State.FlipSkeld))
                    State.Save();*/ //to be fixed later
                if (IsInLobby()) ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
                if (IsInLobby() && AnimatedButton("Force Start of Game")) {
                    app::AmongUsClient_KickNotJoinedPlayers(*Game::pAmongUsClient, NULL);
                    app::InnerNetClient_SendStartGame((InnerNetClient*)(*Game::pAmongUsClient), NULL);
                }
                if (IsInLobby() && State.IsStartCountdownActive &&
                    ColoredButton(ImVec4(1.f, 0.f, 0.f, 1.f), "Cancel Start of Game")) {
                    State.CancelingStartGame = true;
                }

                if (ToggleButton("Enable Moderation System", &State.Mod_EnableModeration))
                    State.Save();

                if (ToggleButton("Always Allow Start Button", &State.AlwaysAllowStart))
                    State.Save();

                if (ToggleButton("Modify Start Countdown", &State.ModifyStartCountdown))
                    State.Save();

                if (State.ModifyStartCountdown && ImGui::InputInt("Countdown Time", &State.StartCountdown)) {
                    State.StartCountdown = std::clamp(State.StartCountdown, 1, !State.SafeMode ? 127 : 5);
                    State.Save();
                }

                if (ToggleButton("Disable Meetings", &State.DisableMeetings))
                    State.Save();

                if (ToggleButton("Disable Sabotages", &State.DisableSabotages))
                    State.Save();

                if (ToggleButton("Disable All Votekicks", &State.DisableAllVotekicks))
                    State.Save();

                {
                    std::vector<const char*> GAMEMODES = { "Default", "Task Speedrun" };
                    if (State.DisableHostAnticheat) GAMEMODES = { "Default", "Task Speedrun", "Battle Royale" };
                    int maxIndex = State.DisableHostAnticheat ? 2 : 1;
                    State.GameMode = std::clamp(State.GameMode, 0, maxIndex);
                    if (IsInLobby() && CustomListBoxInt("Game Mode", &State.GameMode, GAMEMODES, 100 * State.dpiScale)) {
                        State.TaskSpeedrun = (State.GameMode == 1);
                        State.BattleRoyale = (State.DisableHostAnticheat && State.GameMode == 2);
                        State.Save();
                    }

                    if (State.GameMode != 0) {
                        ImGui::SetNextItemWidth(100 * State.dpiScale);
                        if (ImGui::InputInt("Game Duration", &State.GameModeDuration)) {
                            State.GameModeDuration = std::clamp(State.GameModeDuration, 100, 500);
                            State.Save();
                        }
                    }
                }

                if (ToggleButton("Spectator Mode", &State.SpectatorMode))
                    State.Save();

                if (ToggleButton("Show Lobby Timer", &State.ShowLobbyTimer))
                    State.Save();

                if (ToggleButton("Auto Start Game", &State.AutoStartGame))
                    State.Save();

                if (State.AutoStartGame) {
                    ImGui::Text("Start After");
                    ImGui::SameLine();
                    if (ImGui::InputInt("sec", &State.AutoStartTimer))
                        State.Save();
                }

                /*if (ToggleButton("Auto Start Game (By Player Count)", &State.AutoStartGamePlayers))
                    State.Save();
                if (State.AutoStartGamePlayers) {
                    ImGui::Text("Start at");
                    ImGui::SameLine();
                    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
                    editingAutoStartPlayerCount = ImGui::IsItemActive();
                    if (ImGui::InputInt("players##autostart", &State.AutoStartPlayerCount, 1, 100, flags)) {
                    State.AutoStartPlayerCount = std::clamp(State.AutoStartPlayerCount, 1, 15);
                    State.Save();
                    }
                    editingAutoStartPlayerCount = ImGui::IsItemActive();
                }

                if (ToggleButton("Ignore RPCs", &State.IgnoreRPCs))
                    State.Save();*/

                    //if (State.DisableKills) ImGui::Text("Note: Cheaters can still bypass this feature!");

                    /*if (ToggleButton("Disable Specific RPC Call ID", &State.DisableCallId))
                        State.Save();
                    int callId = State.ToDisableCallId;
                    if (ImGui::InputInt("ID to Disable", &callId)) {
                        State.ToDisableCallId = (uint8_t)callId;
                        State.Save();
                    }*/

                if ((State.mapType == Settings::MapType::Airship) && IsInGame() && AnimatedButton("Switch Moving Platform Side"))
                {
                    State.rpcQueue.push(new RpcUsePlatform());
                }

                if ((State.mapType == Settings::MapType::Airship) && IsInGame()) {
                    if (ToggleButton("Spam Moving Platform", &State.SpamMovingPlatform)) {
                        State.Save();
                    }
                }

                if (State.InMeeting && AnimatedButton("End Meeting")) {
                    State.rpcQueue.push(new RpcEndMeeting());
                    State.InMeeting = false;
                }

                if (State.CurrentScene.compare("Tutorial") || IsInLobby()) { //lobby isn't possible in freeplay
                    if (ToggleButton("Disable Game Ending", &State.NoGameEnd)) {
                        State.Save();
                    }

                    if (IsInGame()) {
                        CustomListBoxIntColored("Reason", &State.SelectedGameEndReasonId, GAMEENDREASON, 120.0f * State.dpiScale, ImVec4(1.f, 1.f, 1.f, 0.f), 0, "", GAMEENDREASONCOLORS, IM_ARRAYSIZE(GAMEENDREASONCOLORS));

                        ImGui::SameLine();

                        if (AnimatedButton("End Game")) {
                            State.rpcQueue.push(new RpcEndGame(GameOverReason__Enum(std::clamp(State.SelectedGameEndReasonId, 0, 8))));
                        }
                    }
                }

                CustomListBoxIntColored(" ­", &State.HostSelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale, ImVec4(1.f, 1.f, 1.f, 0.f), 0, "", COLOR_NAMES_COLOR, IM_ARRAYSIZE(COLOR_NAMES_COLOR));

                if (ToggleButton("Force Color for Everyone", &State.ForceColorForEveryone)) {
                    State.Save();
                }

                if (!State.SafeMode) {
                    if (ToggleButton("Force Name for Everyone", &State.ForceNameForEveryone)) {
                        State.Save();
                    }
                    if (InputString("Username", &State.hostUserName)) {
                        State.Save();
                    }
                }

                /*if (IsHost() && IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.IsDead && AnimatedButton("Revive Yourself"))
                {
                    if (PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))) {
                        if (IsInGame()) State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
                        if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
                    }
                    else {
                        if (IsInGame()) State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Crewmate));
                        if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Crewmate));
                    }
                }*/

                if (ToggleButton("Unlock Kill Button", &State.UnlockKillButton)) {
                    State.Save();
                }

                if (ToggleButton("Kill While Vanished", &State.KillInVanish)) {
                    State.Save();
                }

                /*if (ToggleButton("Disable Medbay Scan", &State.DisableMedbayScan)) {
                    State.Save();
                }*/

                if (ToggleButton("Bypass Guardian Angel Protections", &State.BypassAngelProt)) {
                    State.Save();
                }

                /*if (GetAllPlayerControl().size() == 1 && IsInGame()) { \
                    if (!State.farmLoop && AnimatedButton("Level Farm (50000 Kills)")) {
                        State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::ImpostorGhost));
                        State.farmCount = 5000; //controls how many times the player is to be murdered
                        State.farmLoop = true;
                    }
                    if (State.farmLoop && AnimatedButton("Stop Level Farm (End Game by Impostor Kill Win)")) {
                        State.farmLoop = false;
                        State.farmCount = 0;
                        State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, RoleTypes__Enum::Impostor));
                        State.rpcQueue.push(new SetRole(RoleTypes__Enum::Impostor));
                        State.rpcQueue.push(new RpcEndGame(GameOverReason__Enum::ImpostorsByKill));
                    }
                    if (State.farmLoop) ImGui::Text(std::format("({} Kills)", 50000 - 10 * State.farmCount).c_str());
                }*/

                ImGui::EndChild();
            }

            if (openSettings) {
                ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
                if (ImGui::CollapsingHeader("Disable Sabotages", ImGuiTreeNodeFlags_DefaultOpen)) {
                    SabotageTab::RenderDisableSabotages();
                }
                ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
                if (ImGui::CollapsingHeader("Host Presets", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
                    if (ToggleButton("Auto Apply on Host", &State.AutoApplyHostPreset))
                        State.Save();

                    ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

                    // Preset list
                    if (!State.HostPresets.empty()) {
                        std::vector<const char*> presetNames;
                        for (auto& p : State.HostPresets) presetNames.push_back(p.Name.c_str());
                        CustomListBoxInt("##presetselect", &State.SelectedHostPreset, presetNames, 200.0f * State.dpiScale, ImVec4(0, 0, 0, 0), 0, "Preset");
                        ImGui::SameLine();
                        if (AnimatedButton("Apply")) {
                            int idx = std::clamp(State.SelectedHostPreset, 0, (int)State.HostPresets.size() - 1);
                            RequestApplyHostPreset(idx);
                        }
                        ImGui::SameLine();
                        if (AnimatedButton("Update##preset")) {
                            int idx = std::clamp(State.SelectedHostPreset, 0, (int)State.HostPresets.size() - 1);
                            if (CaptureHostPreset(State.HostPresets[idx])) {
                                State.Save();
                            }
                        }
                        ImGui::SameLine();
                        if (AnimatedButton("Delete##preset")) {
                            int idx = std::clamp(State.SelectedHostPreset, 0, (int)State.HostPresets.size() - 1);
                            State.HostPresets.erase(State.HostPresets.begin() + idx);
                            if (State.HostPresets.size() != 0)
                                State.SelectedHostPreset = std::clamp(State.SelectedHostPreset, 0, (int)State.HostPresets.size() - 1);
                            State.Save();
                        }
                    }
                    else {
                        ImGui::TextDisabled("No presets saved.");
                    }

                    ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

                    // Save new preset
                    static std::string newPresetName = "My Preset";
                    ImGui::SetNextItemWidth(160 * State.dpiScale);
                    InputString("Preset Name", &newPresetName);
                    ImGui::SameLine();
                    if (AnimatedButton("Save Current##preset")) {
                        Settings::HostPreset p;
                        p.Name = newPresetName.empty() ? "Preset" : newPresetName;
                        if (CaptureHostPreset(p)) {
                            State.HostPresets.push_back(p);
                            State.SelectedHostPreset = (int)State.HostPresets.size() - 1;
                            State.Save();
                        }
                    }
                }
                ImGui::Dummy(ImVec2(4, 4)* State.dpiScale);
                // AU v2022.8.24 has been able to change maps in lobby.
                // AU v2022.8.24 has been able to change maps in lobby.
                State.mapHostChoice = State.FlipSkeld ? 3 : options.GetByte(app::ByteOptionNames__Enum::MapId);
                /*if (State.mapHostChoice > 3)
                    State.mapHostChoice--;*/
                State.mapHostChoice = std::clamp(State.mapHostChoice, 0, (int)MAP_NAMES.size() - 1);
                if (IsInLobby() && CustomListBoxIntColored("Map", &State.mapHostChoice, MAP_NAMES, 75 * State.dpiScale, ImVec4(1.f, 1.f, 1.f, 0.f), 0, "", MAP_NAMES_COLOR, IM_ARRAYSIZE(MAP_NAMES_COLOR))) {
                    //if (!IsInGame()) {
                        // disable flip
                    if (State.mapHostChoice == 3) {
                        options.SetByte(app::ByteOptionNames__Enum::MapId, 0);
                        State.FlipSkeld = true;
                        SyncAllSettings();
                    }
                    else {
                        options.SetByte(app::ByteOptionNames__Enum::MapId, State.mapHostChoice);
                        State.FlipSkeld = false;
                        SyncAllSettings();
                    }
                    /*auto id = State.mapHostChoice;
                    if (id >= 3) id++;
                    options.SetByte(app::ByteOptionNames__Enum::MapId, id);
                    SyncAllSettings();*/
                    //}
                }
                auto gamemode = options.GetGameMode();

                auto MakeBool = [&](const char* str, bool& v, BoolOptionNames__Enum opt) {
                    if (ToggleButton(str, &v)) {
                        options.SetBool(opt, v);
                        SyncAllSettings();
                    }
                    else v = options.GetBool(opt);
                    };

                auto MakeInt = [&](const char* str, int& v, Int32OptionNames__Enum opt) {
                    if (ImGui::InputInt(str, &v)) {
                        options.SetInt(opt, v);
                        SyncAllSettings();
                    }
                    else v = options.GetInt(opt);
                    };

                auto MakeFloat = [&](const char* str, float& v, FloatOptionNames__Enum opt) {
                    if (ImGui::InputFloat(str, &v)) {
                        options.SetFloat(opt, v);
                        SyncAllSettings();
                    }
                    else v = options.GetFloat(opt);
                    };

                if (gamemode == GameModes__Enum::Normal || gamemode == GameModes__Enum::NormalFools) {
                    static int emergencyMeetings = 1, emergencyCooldown = 1, discussionTime = 1,
                        votingTime = 1, killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, taskBarMode = 1;

                    static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f;

                    static bool ejects = false, anonVotes = false, visualTasks = false;

#pragma region General
                    MakeBool("Confirm Ejects", ejects, BoolOptionNames__Enum::ConfirmImpostor);
                    MakeInt("# Emergency Meetings", emergencyMeetings, Int32OptionNames__Enum::NumEmergencyMeetings);
                    MakeBool("Anonymous Votes", anonVotes, BoolOptionNames__Enum::AnonymousVotes);
                    MakeInt("Emergency Cooldown", emergencyCooldown, Int32OptionNames__Enum::EmergencyCooldown);
                    MakeInt("Discussion Time", discussionTime, Int32OptionNames__Enum::DiscussionTime);
                    MakeInt("Voting Time", votingTime, Int32OptionNames__Enum::VotingTime);
                    // MakeFloat("Player Speed", playerSpeed, FloatOptionNames__Enum::PlayerSpeedMod);
                    // player speed can be between 0 (not included) and 3 (included) in classic mode due to the anticheat, so we separate this float input
                    if (ImGui::InputFloat("Player Speed", &playerSpeed)) {
                        if (State.SafeMode) {
                            if (playerSpeed <= 0.f) playerSpeed = 0.000001f;
                            if (playerSpeed > 3.f) playerSpeed = 3.f;
                        }
                        options.SetFloat(FloatOptionNames__Enum::PlayerSpeedMod, playerSpeed);
                        SyncAllSettings();
                    }
                    else playerSpeed = options.GetFloat(FloatOptionNames__Enum::PlayerSpeedMod);

                    std::string taskBarInfo = "";
                    if (taskBarMode >= 0 && taskBarMode <= 2) {
                        switch (taskBarMode) {
                        case 0:
                            taskBarInfo = " (Always)";
                            break;
                        case 1:
                            taskBarInfo = " (Meetings)";
                            break;
                        case 2:
                            taskBarInfo = " (Never)";
                            break;
                        }
                    }
                    MakeInt(("Task Bar Updates" + taskBarInfo).c_str(), taskBarMode, Int32OptionNames__Enum::TaskBarMode);
                    MakeBool("Visual Tasks", visualTasks, BoolOptionNames__Enum::VisualTasks);
                    MakeFloat("Crewmate Vision", crewVision, FloatOptionNames__Enum::CrewLightMod);
                    MakeFloat("Impostor Vision", impVision, FloatOptionNames__Enum::ImpostorLightMod);
                    // MakeFloat("Kill Cooldown", killCooldown, FloatOptionNames__Enum::KillCooldown);
                    // 0 or lesser kill cooldown leads to the impostors not being able to kill
                    if (ImGui::InputFloat("Kill Cooldown", &killCooldown)) {
                        if (killCooldown <= 0.f) killCooldown = 0.000001f;
                        options.SetFloat(FloatOptionNames__Enum::KillCooldown, killCooldown);
                        SyncAllSettings();
                    }
                    else killCooldown = options.GetFloat(FloatOptionNames__Enum::KillCooldown);
                    std::string killDistInfo = "";
                    if (killDistance >= 0 && killDistance <= 2) {
                        switch (killDistance) {
                        case 0:
                            killDistInfo = " (Short)";
                            break;
                        case 1:
                            killDistInfo = " (Medium)";
                            break;
                        case 2:
                            killDistInfo = " (Long)";
                            break;
                        }
                    }

                    if (ImGui::InputInt(("Kill Distance" + killDistInfo).c_str(), &killDistance)) {
                        if (State.SafeMode) killDistance = std::clamp(killDistance, 0, 2);
                        options.SetInt(Int32OptionNames__Enum::KillDistance, killDistance);
                        SyncAllSettings();
                    }
                    else killDistance = options.GetInt(Int32OptionNames__Enum::KillDistance);

                    // MakeInt(("Kill Distance" + killDistInfo).c_str(), killDistance, Int32OptionNames__Enum::KillDistance);
                    MakeInt("# Short Tasks", shortTasks, Int32OptionNames__Enum::NumShortTasks);
                    MakeInt("# Common Tasks", commonTasks, Int32OptionNames__Enum::NumCommonTasks);
                    MakeInt("# Long Tasks", longTasks, Int32OptionNames__Enum::NumLongTasks);
#pragma endregion
#pragma region Scientist
                    ImGui::Text("Scientist");
                    static float vitalsCooldown = 1.f, batteryDuration = 1.f;

                    MakeFloat("Vitals Display Cooldown", vitalsCooldown, FloatOptionNames__Enum::ScientistCooldown);
                    MakeFloat("Battery Duration", batteryDuration, FloatOptionNames__Enum::ScientistBatteryCharge);
#pragma endregion
#pragma region Engineer
                    ImGui::Text("Engineer");
                    static float ventCooldown = 1.f, ventDuration = 1.f;

                    MakeFloat("Vent Use Cooldown", ventCooldown, FloatOptionNames__Enum::EngineerCooldown);
                    MakeFloat("Max Time in Vents", ventDuration, FloatOptionNames__Enum::EngineerInVentMaxTime);
#pragma endregion
#pragma region Guardian Angel
                    ImGui::Text("Guardian Angel");
                    static float protectCooldown = 1.f, protectDuration = 1.f;
                    static bool protectVisible = false;

                    MakeFloat("Protect Cooldown", protectCooldown, FloatOptionNames__Enum::GuardianAngelCooldown);
                    MakeFloat("Protection Duration", protectDuration, FloatOptionNames__Enum::ProtectionDurationSeconds);
                    MakeBool("Protect Visible to Impostors", protectVisible, BoolOptionNames__Enum::ImpostorsCanSeeProtect);
#pragma endregion
#pragma region Shapeshifter
                    ImGui::Text("Shapeshifter");
                    static float shapeshiftDuration = 1.f, shapeshiftCooldown = 1.f;
                    static bool shapeshiftEvidence = false;

                    MakeFloat("Shapeshift Duration", shapeshiftDuration, FloatOptionNames__Enum::ShapeshifterDuration);
                    MakeFloat("Shapeshift Cooldown", shapeshiftCooldown, FloatOptionNames__Enum::ShapeshifterCooldown);
                    MakeBool("Leave Shapeshifting Evidence", shapeshiftEvidence, BoolOptionNames__Enum::ShapeshifterLeaveSkin);
#pragma endregion
#pragma region Noisemaker
                    ImGui::Text("Noisemaker");
                    static float alertDuration = 1.f;
                    static bool alertImps = false;

                    MakeFloat("Alert Duration", alertDuration, FloatOptionNames__Enum::NoisemakerAlertDuration);
                    MakeBool("Noisemakers Alert Impostors", alertImps, BoolOptionNames__Enum::NoisemakerImpostorAlert);
#pragma endregion
#pragma region Tracker
                    ImGui::Text("Tracker");
                    static float trackerDuration = 1.f, trackerCooldown = 1.f, trackerDelay = 1.f;

                    MakeFloat("Tracker Duration", trackerDuration, FloatOptionNames__Enum::TrackerDuration);
                    MakeFloat("Tracker Cooldown", trackerCooldown, FloatOptionNames__Enum::TrackerCooldown);
                    MakeFloat("Tracker Delay", trackerDelay, FloatOptionNames__Enum::TrackerDelay);
#pragma endregion
#pragma region Phantom
                    ImGui::Text("Phantom");
                    static float phantomDuration = 1.f, phantomCooldown = 1.f;

                    MakeFloat("Phantom Duration", phantomDuration, FloatOptionNames__Enum::PhantomDuration);
                    MakeFloat("Phantom Cooldown", phantomCooldown, FloatOptionNames__Enum::PhantomCooldown);
#pragma endregion
#pragma region Detective
                    ImGui::Text("Detective");
                    static float detectiveSuspectLimit = 1.f;

                    MakeFloat("Detective Suspect Limit", detectiveSuspectLimit, FloatOptionNames__Enum::DetectiveSuspectLimit);
#pragma endregion
#pragma region Viper
                    ImGui::Text("Viper");
                    static float viperDissolveTime = 1.f;

                    MakeFloat("Viper Dissolve Time", viperDissolveTime, FloatOptionNames__Enum::ViperDissolveTime);
#pragma endregion
#pragma region Viper
                    ImGui::Text("Judge");
                    static float judgeTaskRequirement = 50.f;

                    MakeFloat("Tasks Required %", judgeTaskRequirement, FloatOptionNames__Enum::JudgeTaskRequirementPercentage);
#pragma endregion
                }
#pragma region Hide and Seek
                if (gamemode == GameModes__Enum::HideNSeek || gamemode == GameModes__Enum::SeekFools) {
                    static int killDistance = 1, commonTasks = 1, shortTasks = 1, longTasks = 1, maxVents = 1;

                    static float playerSpeed = 1.f, crewVision = 1.f, impVision = 1.f, killCooldown = 1.f,
                        hidingTime = 1.f, finalHideTime = 1.f, ventTime = 1.f, crewLight = 1.f, impLight = 1.f,
                        finalImpSpeed = 1.f, pingInterval = 1.f;

                    static bool flashlight = false, seekMap = false, hidePings = false, showNames = false;

                    MakeFloat("Hider Vision", crewVision, FloatOptionNames__Enum::CrewLightMod);
                    MakeFloat("Seeker Vision", impVision, FloatOptionNames__Enum::ImpostorLightMod);
                    if (ImGui::InputFloat("Kill Cooldown", &killCooldown)) {
                        if (killCooldown <= 0.f) killCooldown = 0.000001f;
                        options.SetFloat(FloatOptionNames__Enum::KillCooldown, killCooldown);
                        SyncAllSettings();
                    }
                    else killCooldown = options.GetFloat(FloatOptionNames__Enum::KillCooldown);

                    std::string killDistInfo = "";
                    if (killDistance >= 0 && killDistance <= 2) {
                        switch (killDistance) {
                        case 0:
                            killDistInfo = " (Short)";
                            break;
                        case 1:
                            killDistInfo = " (Medium)";
                            break;
                        case 2:
                            killDistInfo = " (Long)";
                            break;
                        }
                    }

                    MakeInt(("Kill Distance" + killDistInfo).c_str(), killDistance, Int32OptionNames__Enum::KillDistance);
                    MakeInt("# Short Tasks", shortTasks, Int32OptionNames__Enum::NumShortTasks);
                    MakeInt("# Common Tasks", commonTasks, Int32OptionNames__Enum::NumCommonTasks);
                    MakeInt("# Long Tasks", longTasks, Int32OptionNames__Enum::NumLongTasks);
                    MakeFloat("Player Speed", playerSpeed, FloatOptionNames__Enum::PlayerSpeedMod);
                    MakeFloat("Hiding Time", hidingTime, FloatOptionNames__Enum::EscapeTime);
                    MakeFloat("Final Hide Time", finalHideTime, FloatOptionNames__Enum::FinalEscapeTime);
                    MakeInt("Max Vent Uses", maxVents, Int32OptionNames__Enum::CrewmateVentUses);
                    MakeFloat("Max Time in Vent", ventTime, FloatOptionNames__Enum::CrewmateTimeInVent);
                    MakeBool("Flashlight Mode", flashlight, BoolOptionNames__Enum::UseFlashlight);
                    MakeFloat("Hider Flashlight Size", crewLight, FloatOptionNames__Enum::CrewmateFlashlightSize);
                    MakeFloat("Seeker Flashlight Size", impLight, FloatOptionNames__Enum::ImpostorFlashlightSize);
                    MakeFloat("Final Hide Seeker Speed", finalImpSpeed, FloatOptionNames__Enum::SeekerFinalSpeed);
                    MakeBool("Final Hide Seeker Map", seekMap, BoolOptionNames__Enum::SeekerFinalMap);
                    MakeBool("Final Hide Pings", hidePings, BoolOptionNames__Enum::SeekerPings);
                    MakeFloat("Ping Interval", pingInterval, FloatOptionNames__Enum::MaxPingTime);
                    MakeBool("Show Names", showNames, BoolOptionNames__Enum::ShowCrewmateNames);
                }
#pragma endregion
            }
            if (openTournaments && State.TournamentMode) {
                if (AnimatedButton("Copy All Data") && State.tournamentFriendCodes.size() != 0) {
                    std::string data = "";
                    for (auto i : State.tournamentFriendCodes) {
                        float points = State.tournamentPoints[i], win = State.tournamentWinPoints[i],
                            callout = State.tournamentCalloutPoints[i], death = State.tournamentEarlyDeathPoints[i];
                        std::string text = std::format("\n{}: {} Normal, {} +SV", i, DisplayScore(points), DisplayScore(callout)/*,
                            DisplayScore(win), DisplayScore(death)).c_str()*/); // +W, +D are not required anymore
                        data += text;
                    }
                    ClipboardHelper_PutClipboardString(convert_to_string(data.substr(1)), NULL);
                }
                ImGui::SameLine();
                if (ColoredButton(ImVec4(1.f, 0.f, 0.f, 1.f), "Clear All Data")) {
                    State.tournamentPoints.clear();
                    State.tournamentKillCaps.clear();
                    State.tournamentWinPoints.clear();
                    State.tournamentCalloutPoints.clear();
                    State.tournamentEarlyDeathPoints.clear();
                }

                for (auto i : State.tournamentFriendCodes) {
                    float points = State.tournamentPoints[i], win = State.tournamentWinPoints[i],
                        callout = State.tournamentCalloutPoints[i], death = State.tournamentEarlyDeathPoints[i];
                    std::string text = std::format("{}: {} Normal, {} +SV", i, DisplayScore(points), DisplayScore(callout)/*,
                            DisplayScore(win), DisplayScore(death)).c_str()*/); // +W, +D are not required anymore
                    if (IsInLobby() && State.ChatCooldown >= 3.f && text.size() <= 120 && AnimatedButton("Send")) {
                        //in ideal conditions a message longer than 120 characters should not be possible
                        State.lobbyRpcQueue.push(new RpcSendChat(*Game::pLocalPlayer, text));
                        State.MessageSent = true;
                    }
                    if (IsInLobby() && State.ChatCooldown >= 3.f && text.size() <= 120) ImGui::SameLine();
                    ImGui::Text(text.c_str());
                }
            }
            if (openModeration) {
                ImGui::Dummy(ImVec2(0, 2) * State.dpiScale);
                if (ImGui::CollapsingHeader("Roles", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Dummy(ImVec2(0, 2) * State.dpiScale);
                    static const std::vector<std::pair<const char*, const char*>> ROLE_COMMANDS = {
                        { "/color", "color" }, { "/rules", "r" },
                        { "/sicko", "sicko" }, { "/warn & /unwarn", "warn" },
                        { "/kick & /kickc", "kick" }, { "/ban & /banc", "ban" },
                        { "/callmeeting", "callmeeting" }, { "/endmeeting", "endmeeting" }, { "/start", "start" }, { "/end", "end" },
                    };
                    static int selectedRole = 0;
                    static std::string newRoleName = "";
                    static std::string renameBuf = "";
                    static std::string newMemberCode = "";
                    static int selectedMemberIndex = 0;
                    static bool isRoleDeleted = false;

                    if (isRoleDeleted) isRoleDeleted = false;
                    ImGui::Text("Create Role:");
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(140.0f * State.dpiScale);
                    InputString("##NewRoleName", &newRoleName, ImGuiInputTextFlags_EnterReturnsTrue);
                    ImGui::SameLine();
                    if (AnimatedButton("Add Role")) {
                        if (!newRoleName.empty()) {
                            State.Mod_RoleNames.push_back(newRoleName);
                            State.Mod_RoleMembers.push_back({});
                            State.Mod_RolePermissions.push_back({});
                            State.Mod_RoleRank.push_back(0);
                            selectedRole = (int)State.Mod_RoleNames.size() - 1;
                            newRoleName = "";
                            State.Save();
                        }
                    }

                    ImGui::Dummy(ImVec2(0, 4) * State.dpiScale);

                    if (State.Mod_RoleRank.size() < State.Mod_RoleNames.size()) State.Mod_RoleRank.resize(State.Mod_RoleNames.size(), 0);

                    if (!State.Mod_RoleNames.empty()) {
                        selectedRole = std::clamp(selectedRole, 0, (int)State.Mod_RoleNames.size() - 1);
                        std::vector<const char*> roleVector(State.Mod_RoleNames.size(), nullptr);
                        for (size_t i = 0; i < State.Mod_RoleNames.size(); i++) roleVector[i] = State.Mod_RoleNames[i].c_str();
                        ImGui::Text("Select Role:");
                        ImGui::SameLine();
                        CustomListBoxInt("SelectedRole", &selectedRole, roleVector, 150.0f * State.dpiScale, ImVec4(0, 0, 0, 0), ImGuiComboFlags_None, " ");
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(60.0f * State.dpiScale);
                        ImGui::InputInt("##EditRoleRank", &State.Mod_RoleRank[selectedRole]);
                        ImGui::SameLine();
                        if (AnimatedButton("Set Rank")) {
                            State.Save();
                        }
                    }
                    else {
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No roles created yet.");
                    }

                    if (!State.Mod_RoleNames.empty()) {
                        ImGui::Dummy(ImVec2(0, 4) * State.dpiScale);
                        ImGui::SetNextItemWidth(150.0f * State.dpiScale);
                        InputString("##RenameRole", &renameBuf, ImGuiInputTextFlags_EnterReturnsTrue);
                        ImGui::SameLine();
                        if (AnimatedButton("Rename Role")) {
                            if (!renameBuf.empty()) {
                                State.Mod_RoleNames[selectedRole] = renameBuf;
                                renameBuf = "";
                                State.Save();
                            }
                        }
                        ImGui::SameLine();
                        if (AnimatedButton("Delete Role")) {
                            State.Mod_RoleNames.erase(State.Mod_RoleNames.begin() + selectedRole);
                            State.Mod_RoleMembers.erase(State.Mod_RoleMembers.begin() + selectedRole);
                            State.Mod_RolePermissions.erase(State.Mod_RolePermissions.begin() + selectedRole);
                            State.Mod_RoleRank.erase(State.Mod_RoleRank.begin() + selectedRole);
                            State.Save();
                            isRoleDeleted = true;
                        }

                        if (!isRoleDeleted) {
                            ImGui::Dummy(ImVec2(0, 6) * State.dpiScale);
                            ImGui::Text("Permissions for %s:", State.Mod_RoleNames[selectedRole].c_str());
                            ImVec4 themeCol = State.RgbMenuTheme ? State.RgbColor : (State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor);
                            ImVec4 themeColDark = ImVec4(themeCol.x * 0.7f, themeCol.y * 0.7f, themeCol.z * 0.7f, themeCol.w);
                            ImVec4 themeColDarker = ImVec4(themeCol.x * 0.5f, themeCol.y * 0.5f, themeCol.z * 0.5f, themeCol.w);
                            ImGui::Columns(2, "rolePermCols", false);
                            for (auto& cmd : ROLE_COMMANDS) {
                                bool granted = State.Mod_RolePermissions[selectedRole].count(cmd.second) && State.Mod_RolePermissions[selectedRole][cmd.second];
                                ImGui::PushStyleColor(ImGuiCol_Button, granted ? themeCol : ImVec4(0.f, 0.f, 0.f, 0.f));
                                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, granted ? themeColDarker : themeColDark);
                                ImGui::PushStyleColor(ImGuiCol_ButtonActive, themeCol);
                                if (AnimatedButton((std::string(cmd.first) + "##roleperm" + std::to_string(selectedRole)).c_str())) {
                                    State.Mod_RolePermissions[selectedRole][cmd.second] = !granted;
                                    State.Save();
                                }
                                ImGui::PopStyleColor(3);
                                ImGui::NextColumn();
                            }
                            ImGui::Columns(1);

                            ImGui::Dummy(ImVec2(0, 6) * State.dpiScale);
                            ImGui::Text("Members:");
                            ImGui::SetNextItemWidth(150.0f * State.dpiScale);
                            InputString("##NewMemberCode", &newMemberCode, ImGuiInputTextFlags_EnterReturnsTrue);
                            ImGui::SameLine();
                            if (AnimatedButton("Add (friendcode)##RoleMember")) {
                                if (!newMemberCode.empty()) {
                                    State.Mod_RoleMembers[selectedRole].push_back(newMemberCode);
                                    newMemberCode = "";
                                    State.Save();
                                }
                            }
                            auto& members = State.Mod_RoleMembers[selectedRole];
                            if (!members.empty()) {
                                selectedMemberIndex = std::clamp(selectedMemberIndex, 0, (int)members.size() - 1);
                                std::vector<const char*> memberVector(members.size(), nullptr);
                                for (size_t i = 0; i < members.size(); i++) memberVector[i] = members[i].c_str();
                                CustomListBoxInt("##RemoveRoleMember", &selectedMemberIndex, memberVector, 150.0f * State.dpiScale, ImVec4(0, 0, 0, 0), ImGuiComboFlags_None, " ");
                                ImGui::SameLine();
                                if (AnimatedButton("Remove##RoleMember")) {
                                    members.erase(members.begin() + selectedMemberIndex);
                                    State.Save();
                                }
                            }
                        }
                    }
                }
            }
            ImGui::EndChild();
        }
    }
}