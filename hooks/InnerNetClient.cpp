#include "pch-il2cpp.h"
#include "_hooks.h"
#include "utility.h"
#include "state.hpp"
#include "game.h"
#include "logger.h"
#include "utility.h"
#include "replay.hpp"
#include "profiler.h"
#include <sstream>
#include "esp.hpp"
#include <chrono>

using namespace std::string_view_literals;

static bool autoStartedGame = false;

static bool OpenDoor(OpenableDoor* door) {
    if ("PlainDoor"sv == door->klass->name) {
        app::PlainDoor_SetDoorway(reinterpret_cast<PlainDoor*>(door), true, {});
    }
    else if ("MushroomWallDoor"sv == door->klass->name) {
        app::MushroomWallDoor_SetDoorway(reinterpret_cast<MushroomWallDoor*>(door), true, {});
    }
    else {
        return false;
    }
    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Doors, door->fields.Id | 64));
    return true;
}

static void onGameEnd() {
    try {
        LOG_DEBUG("Reset All");
        Replay::Reset();
        State.modUsers.clear();
        State.activeImpersonation = false;
        State.FollowerCam = nullptr;
        State.EnableZoom = false;
        State.FreeCam = false;
        State.MatchEnd = std::chrono::system_clock::now();
        std::fill(State.assignedRoles.begin(), State.assignedRoles.end(), RoleType::Random); //Clear Pre assigned roles to avoid bugs.
        State.engineers_amount = 0;
        State.scientists_amount = 0;
        State.shapeshifters_amount = 0;
        State.impostors_amount = 0;
        State.crewmates_amount = 0; //We need to reset these. Or if the host doesn't turn on host tab ,these value won't update.
        State.IsRevived = false;
        State.protectMonitor.clear();
        State.vanishedPlayers.clear();
        State.VoteKicks = 0;
        State.OutfitCooldown = 50;
        State.CanChangeOutfit = false;
        State.GameLoaded = false;
        State.RealRole = RoleTypes__Enum::Crewmate;
        State.mapType = Settings::MapType::Ship;
        State.SpeedrunTimer = 0.f;
        autoStartedGame = false;

        if (State.PanicMode && State.TempPanicMode) {
            State.PanicMode = false;
            State.TempPanicMode = false;
        }

        State.tournamentFirstMeetingOver = false;
        State.tournamentKillCaps.clear();
        State.tournamentAssignedImpostors.clear();
        State.tournamentAliveImpostors.clear();
        State.tournamentCallers.clear();
        State.tournamentCalledOut.clear();
        State.tournamentCorrectCallers.clear();
        State.tournamentAllTasksCompleted.clear();
        State.SpeedrunOver = false;

        drawing_t& instance = Esp::GetDrawing();
        synchronized(instance.m_DrawingMutex) {
            instance.m_Players = {};
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in onGameEnd (InnerNetClient)");
    }
}

void dInnerNetClient_Update(InnerNetClient* __this, MethodInfo* method)
{
    if (State.ShowHookLogs) LOG_DEBUG("Hook dInnerNetClient_Update executed");
    try {
        if (!State.PanicMode) {
            static bool onStart = true;
            if (!IsInLobby()) {
                State.LobbyTimer = 600.f;
                State.JoinedAsHost = false;
            }

            if (!IsInGame()) {
                if (State.PlayMedbayScan) {
                    State.PlayMedbayScan = false;
                }
                if (State.PlayWeaponsAnimation) {
                    State.PlayWeaponsAnimation = false;
                }
            }

            if ((IsInGame() || IsInLobby()) && State.CanChangeOutfit) { //removed hotkeynoclip cuz even if noclip setting is saved and turned on it doesn't work
                if (!(GetPlayerData(*Game::pLocalPlayer)->fields.IsDead)) {
                    if (!State.PanicMode && (State.NoClip || State.IsRevived))
                        app::GameObject_set_layer(app::Component_get_gameObject((Component_1*)(*Game::pLocalPlayer), NULL), app::LayerMask_NameToLayer(convert_to_string("Ghost"), NULL), NULL);
                    else
                        app::GameObject_set_layer(app::Component_get_gameObject((Component_1*)(*Game::pLocalPlayer), NULL), app::LayerMask_NameToLayer(convert_to_string("Players"), NULL), NULL);
                }
                else
                    app::GameObject_set_layer(app::Component_get_gameObject((Component_1*)(*Game::pLocalPlayer), NULL), app::LayerMask_NameToLayer(convert_to_string("Ghost"), NULL), NULL);
                /*for (auto player : GetAllPlayerControl()) {
                    if (player != *Game::pLocalPlayer)
                        app::GameObject_set_layer(app::Component_get_gameObject((Component_1*)(player), NULL), app::LayerMask_NameToLayer(convert_to_string("Ghost"), NULL), NULL);
                }*/ //unintentionally prevents admin from working, workaround can be found later
            }

            if (!IsInGame()) {
                State.InMeeting = false;
                State.DisableLights = false;
                State.AutoRepairSabotage = false;
                State.CloseAllDoors = false;
                State.SpamReport = false;
                State.DisableVents = false;

                if (!IsInLobby()) {
                    State.selectedPlayers = {};
                    State.EnableZoom = false; //intended as we don't want stuff like the taskbar and danger meter disappearing on game start
                    State.FreeCam = false; //moving after game start / on joining new game
                    State.ChatFocused = false; //failsafe
                }
            }
            else {
                if (!State.rpcQueue.empty()) {
                    auto rpc = State.rpcQueue.front();
                    //Looks like there is a check on Task completion when u are dead.
                    //The maximum amount of Tasks that can be completed per Update is at 6 (but it's 1 cuz u still get kicked).
                    static auto maxProcessedTasks = 0;
                    if (!State.SafeMode) {
                        maxProcessedTasks = 765; //max tasks per task type = 255, # task types = 3, max tasks = 765 simple math
                    }
                    else {
                        maxProcessedTasks = 1; //originally 6
                    }
                    auto processedTaskCompletes = 0;
                    if (dynamic_cast<RpcCompleteTask*>(rpc))
                    {
                        if (processedTaskCompletes < maxProcessedTasks)
                        {
                            State.rpcQueue.pop();
                            rpc->Process();
                            processedTaskCompletes++;
                        }
                    }
                    else
                    {
                        State.rpcQueue.pop();
                        rpc->Process();
                    }
                    delete rpc;
                }

                if (State.CloseAllDoors) {
                    for (auto door : State.mapDoors) {
                        State.rpcQueue.push(new RpcCloseDoorsOfType(door, false));
                    }
                    State.CloseAllDoors = false;
                }

                if (State.MoveInVentAndShapeshift && (((*Game::pLocalPlayer)->fields.inVent) || (*Game::pLocalPlayer)->fields.shapeshifting)) {
                    (*Game::pLocalPlayer)->fields.moveable = true;
                }
            }

            if (IsInGame() || IsInLobby()) {
                State.versionShower = nullptr;
                if (State.AlwaysMove && !State.ChatFocused)
                    (*Game::pLocalPlayer)->fields.moveable = true;
                if (State.FakeAlive && GetPlayerData(*Game::pLocalPlayer)->fields.IsDead) {
                    GetPlayerData(*Game::pLocalPlayer)->fields.IsDead = false;
                }
            }

            if (State.SnipeColor && (IsInGame() || IsInLobby())) {
                if ((IsColorAvailable(State.SelectedColorId) || !State.SafeMode) && GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer))->fields.ColorId != State.SelectedColorId) {
                    std::queue<RPCInterface*>* queue = nullptr;
                    if (IsInGame())
                        queue = &State.rpcQueue;
                    else if (IsInLobby())
                        queue = &State.lobbyRpcQueue;

                    if (!IsHost() || State.SafeMode) {
                        queue->push(new RpcSetColor(State.SelectedColorId));
                        LOG_INFO("Successfully sniped your desired color!");
                    }
                    else {
                        queue->push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
                        LOG_INFO("Successfully sniped your desired color!");
                    }
                }
            }

            if (State.SpoofLevel && (IsInGame() || IsInLobby()) && !State.activeImpersonation) {
                int fakeLevel = State.SafeMode ? std::clamp(State.FakeLevel, 1, 100001) : State.FakeLevel;
                if (IsInGame() && (GetPlayerData(*Game::pLocalPlayer)->fields.PlayerLevel + 1) != fakeLevel)
                    State.rpcQueue.push(new RpcSetLevel(*Game::pLocalPlayer, fakeLevel));
                else if (IsInLobby() && (GetPlayerData(*Game::pLocalPlayer)->fields.PlayerLevel + 1) != fakeLevel)
                    State.lobbyRpcQueue.push(new RpcSetLevel(*Game::pLocalPlayer, fakeLevel));
            }

            if (IsInLobby()) {
                if (State.originalName == "-") {
                    auto outfit = GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer));
                    if (outfit != NULL) {
                        State.originalName = convert_from_string(outfit->fields.PlayerName);
                        auto petId = outfit->fields.PetId;
                        auto skinId = outfit->fields.SkinId;
                        auto hatId = outfit->fields.HatId;
                        auto visorId = outfit->fields.VisorId;
                        auto namePlateId = outfit->fields.NamePlateId;
                        State.originalPet = petId;
                        State.originalSkin = skinId;
                        State.originalHat = hatId;
                        State.originalVisor = visorId;
                        State.originalNamePlate = namePlateId;
                    }
                }

                if (!State.lobbyRpcQueue.empty()) {
                    auto rpc = State.lobbyRpcQueue.front();
                    State.lobbyRpcQueue.pop();

                    rpc->Process();
                    delete rpc;
                }
            }

            static int rpcDelay = 0;
            if ((IsInGame() || IsInLobby()) && !State.taskRpcQueue.empty()) {
                if (rpcDelay <= 0) {
                    auto rpc = State.taskRpcQueue.front();
                    State.taskRpcQueue.pop();
                    rpc->Process();
                    delete rpc;
                    rpcDelay = State.SafeMode ? int(0.1 * GetFps()) : 0;
                }
                else rpcDelay--;
            }

            if (!IsInGame() && !State.rpcQueue.empty()) State.rpcQueue = {};
            if (!IsInLobby() && !State.lobbyRpcQueue.empty()) State.lobbyRpcQueue = {};
            if (!IsInGame() && !IsInLobby() && !State.taskRpcQueue.empty()) State.taskRpcQueue = {};

            if ((IsInGame() || IsInLobby()) && GameOptions().GetGameMode() == GameModes__Enum::Normal) {
                auto localData = GetPlayerData(*Game::pLocalPlayer);
                app::RoleBehaviour* playerRole = localData->fields.Role;
                app::RoleTypes__Enum role = playerRole != nullptr ? (playerRole)->fields.Role : app::RoleTypes__Enum::Crewmate;
                if (State.NoAbilityCD) {
                    if (role == RoleTypes__Enum::Engineer)
                    {
                        app::EngineerRole* engineerRole = (app::EngineerRole*)playerRole;
                        if (engineerRole->fields.cooldownSecondsRemaining > 0.0f)
                            engineerRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                        engineerRole->fields.inVentTimeRemaining = 30.0f; //Can be anything as it will always be written
                    }
                    if (role == RoleTypes__Enum::Scientist) {
                        app::ScientistRole* scientistRole = (app::ScientistRole*)playerRole;
                        if (scientistRole->fields.currentCooldown > 0.0f)
                            scientistRole->fields.currentCooldown = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                        scientistRole->fields.currentCharge = 69420.0f + 1.0f; //Can be anything as it will always be written
                    }
                    if (role == RoleTypes__Enum::Tracker)
                    {
                        app::TrackerRole* trackerRole = (app::TrackerRole*)playerRole;
                        if (trackerRole->fields.cooldownSecondsRemaining > 0.0f)
                            trackerRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                        trackerRole->fields.delaySecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                        trackerRole->fields.durationSecondsRemaining = 69420.f; //Can be anything as it will always be written
                    }
                    if (role == RoleTypes__Enum::Scientist) {
                        app::ScientistRole* scientistRole = (app::ScientistRole*)playerRole;
                        if (scientistRole->fields.currentCooldown > 0.0f)
                            scientistRole->fields.currentCooldown = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                        scientistRole->fields.currentCharge = 69420.0f + 1.0f; //Can be anything as it will always be written
                    }
                    if (role == RoleTypes__Enum::GuardianAngel) {
                        app::GuardianAngelRole* guardianAngelRole = (app::GuardianAngelRole*)playerRole;
                        if (guardianAngelRole->fields.cooldownSecondsRemaining > 0.0f)
                            guardianAngelRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                    }
                    if (IsHost() || !State.SafeMode) {
                        if (GameLogicOptions().GetKillCooldown() > 0)
                            (*Game::pLocalPlayer)->fields.killTimer = 0;
                        else
                            GameLogicOptions().SetFloat(app::FloatOptionNames__Enum::KillCooldown, 0.0042069f); //force cooldown > 0 as ur unable to kill otherwise
                        if (IsHost()) {
                            GameLogicOptions().SetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, 0); //force set cooldown, otherwise u get kicked
                            GameLogicOptions().SetFloat(app::FloatOptionNames__Enum::PhantomCooldown, 0); //force set cooldown, otherwise u get kicked
                            GameLogicOptions().SetFloat(app::FloatOptionNames__Enum::PhantomDuration, 0); //force set cooldown, otherwise u get kicked
                        }
                        else {
                            if (role == RoleTypes__Enum::Shapeshifter) {
                                app::ShapeshifterRole* shapeshifterRole = (app::ShapeshifterRole*)playerRole;
                                if (shapeshifterRole->fields.cooldownSecondsRemaining > 0.0f)
                                    shapeshifterRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                            }
                            if (role == RoleTypes__Enum::Phantom) {
                                app::PhantomRole* phantomRole = (app::PhantomRole*)playerRole;
                                if (phantomRole->fields.cooldownSecondsRemaining > 0.0f)
                                    phantomRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                            }
                        }
                    }
                    if (role == RoleTypes__Enum::Shapeshifter) {
                        /*app::ShapeshifterRole* shapeshifterRole = (app::ShapeshifterRole*)playerRole;
                        shapeshifterRole->fields.durationSecondsRemaining = 69420.0f; //Can be anything as it will always be written*/
                    }
                    if (role == RoleTypes__Enum::Phantom) {
                        app::PhantomRole* phantomRole = (app::PhantomRole*)playerRole;
                        //phantomRole->fields.durationSecondsRemaining = 69420.0f; //Can be anything as it will always be written
                    }
                    if (IsInGame()) {
                        (*Game::pLocalPlayer)->fields.RemainingEmergencies = 69420;
                        //if (GameOptions().HasOptions())
                            //(*Game::pShipStatus)->fields.EmergencyCooldown = (float)GameOptions().GetInt(app::Int32OptionNames__Enum::EmergencyCooldown);
                    }
                }
            }
            if ((IsInGame() || IsInLobby()) && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && State.NoAbilityCD) {
                auto localData = GetPlayerData(*Game::pLocalPlayer);
                app::RoleBehaviour* playerRole = localData->fields.Role;
                app::RoleTypes__Enum role = playerRole != nullptr ? (playerRole)->fields.Role : app::RoleTypes__Enum::Crewmate;
                if (IsHost() || !State.SafeMode) (*Game::pLocalPlayer)->fields.killTimer = 0;
                if (role == RoleTypes__Enum::Engineer)
                {
                    app::EngineerRole* engineerRole = (app::EngineerRole*)playerRole;
                    if (engineerRole->fields.cooldownSecondsRemaining > 0.0f)
                        engineerRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                    engineerRole->fields.inVentTimeRemaining = 30.0f; //Can be anything as it will always be written
                }
            }

            if (!State.NoAbilityCD && (IsInGame() || IsInLobby()) && GameOptions().HasOptions()) {
                auto localData = GetPlayerData(*Game::pLocalPlayer);
                app::RoleBehaviour* playerRole = localData->fields.Role;
                app::RoleTypes__Enum role = playerRole != nullptr ? (playerRole)->fields.Role : app::RoleTypes__Enum::Crewmate;
                GameOptions options;
                if (role == RoleTypes__Enum::Engineer)
                {
                    app::EngineerRole* engineerRole = (app::EngineerRole*)playerRole;
                    float ventTime = options.GetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime, 1.0F);;
                    if (engineerRole->fields.inVentTimeRemaining > ventTime)
                        engineerRole->fields.inVentTimeRemaining = ventTime;
                }
                if (role == RoleTypes__Enum::Scientist) {
                    app::ScientistRole* scientistRole = (app::ScientistRole*)playerRole;
                    float charge = options.GetFloat(app::FloatOptionNames__Enum::ScientistBatteryCharge, 1.0F);
                    if (scientistRole->fields.currentCharge > charge)
                        scientistRole->fields.currentCharge = charge;
                }
                if (role == RoleTypes__Enum::Shapeshifter) {
                    /*app::ShapeshifterRole* shapeshifterRole = (app::ShapeshifterRole*)playerRole;
                    float shiftTime = options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration, 1.0F);
                    if (shapeshifterRole->fields.durationSecondsRemaining > shiftTime)
                        shapeshifterRole->fields.durationSecondsRemaining = shiftTime;*/
                }

                int emergencies = options.GetInt(app::Int32OptionNames__Enum::NumEmergencyMeetings, 1);
                if (IsInGame() && (*Game::pLocalPlayer)->fields.RemainingEmergencies > emergencies)
                    (*Game::pLocalPlayer)->fields.RemainingEmergencies = emergencies;
            }

            static int weaponsDelay = 0;

            if (weaponsDelay <= 0 && IsInGame()) {
                if (State.PlayWeaponsAnimation == true) {
                    State.rpcQueue.push(new RpcPlayAnimation(6));
                    weaponsDelay = 50; //Should be approximately 1 second
                }
            }
            else {
                weaponsDelay--;
            }

            if (State.Cycler && State.CycleName) {
                State.SetName = false;
                State.ServerSideCustomName = false;
            }

            if (State.CycleForEveryone) {
                if (State.CycleName)
                    State.ForceNameForEveryone = false;
                if (State.RandomColor)
                    State.ForceColorForEveryone = false;
            }

            /*if (!IsHost()) {
                State.DisableMeetings = false;
                State.DisableSabotages = false;
                State.NoGameEnd = false;
                State.ForceColorForEveryone = false;
            }*/

            if (!IsHost() && State.SafeMode) {
                //State.CycleForEveryone = false;
                //State.ForceNameForEveryone = false;
                State.TeleportEveryone = false;
                //State.GodMode = false;
            }

            if (State.CycleTimer < 0.2f) {
                State.CycleTimer = 0.2f;
                State.Save();
            }

            if (State.CycleDuration <= 10) {
                State.CycleDuration = 10;
                State.Save();
            }

            /*static int joinDelay = 500; //should be 10s
            if (joinDelay <= 0 && State.AutoJoinLobby) {
                AmongUsClient_CoJoinOnlineGameFromCode(*Game::pAmongUsClient,
                    GameCode_GameNameToInt(convert_to_string(State.AutoJoinLobbyCode), NULL),
                    NULL);
                joinDelay = 500; //Should be approximately 10s
            }
            else {
                joinDelay--;
            }*/

            static int reportDelay = 0;
            if (reportDelay <= 0 && State.SpamReport && (IsHost() || !State.SafeMode) && IsInGame()) {
                for (auto p : GetAllPlayerControl()) {
                    if (State.InMeeting)
                        State.rpcQueue.push(new RpcForceMeeting(p, PlayerSelection(p)));
                    else
                        State.rpcQueue.push(new RpcReportBody(PlayerSelection(p)));
                }
                reportDelay = 50; //Should be approximately 1 second
            }
            else {
                reportDelay--;
            }

            static int reportDelays = 0;
            if (reportDelays <= 0 && State.CrashSpamReport && IsInGame() && !State.GameLoaded) {
                for (auto p : GetAllPlayerControl()) {
                    State.rpcQueue.push(new RpcReportBody(PlayerSelection(p)));
                    return;
                }
                reportDelays = 0;
            }
            else {
                reportDelays--;
            }

            static int nameChangeCycleDelay = 0; //If we spam too many name changes, we're banned
            if (nameChangeCycleDelay <= 0 && State.SetName && !State.activeImpersonation && !State.ServerSideCustomName) {
                if ((((IsInGame() || IsInLobby()) && (convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(*Game::pLocalPlayer), nullptr)) != State.userName))
                    || ((!IsInGame() && !IsInLobby()) && GetPlayerName() != State.userName))
                    && !State.userName.empty() && (IsNameValid(State.userName) || (IsHost() || !State.SafeMode))) {
                    //SetPlayerName(State.userName);
                    //LOG_INFO("Name mismatch, setting name to \"" + State.userName + "\"");
                    if (IsInGame())
                        State.rpcQueue.push(new RpcSetName(State.userName));
                    else if (IsInLobby())
                        State.lobbyRpcQueue.push(new RpcSetName(State.userName));
                    nameChangeCycleDelay = 10; //Should be approximately 0.2 second
                }
            }
            else {
                nameChangeCycleDelay--;
            }

            if (!IsInGame() && !IsInLobby() && !IsHost() && GameOptions().HasOptions()) {
                GameOptions options;
                float speedMod = options.GetFloat(FloatOptionNames__Enum::PlayerSpeedMod, 1.f);
                if (speedMod <= 0.f) options.SetFloat(FloatOptionNames__Enum::PlayerSpeedMod, 1.f);
                if (speedMod > 3.f) options.SetFloat(FloatOptionNames__Enum::PlayerSpeedMod, 3.f);
            }

            static int nameCtr = 1;

            static int cycleNameDelay = 0; //If we spam too many name changes, we're banned
            static int colorChangeCycleDelay = 0; //If we spam too many color changes, we're banned?
            static int changeCycleDelay = 0; //controls the actual cosmetic cycler

            if (State.Cycler)
                State.CycleBetweenPlayers = false;
            if (State.CycleBetweenPlayers)
                State.Cycler = false;

            if (State.Cycler && (!State.InMeeting || State.CycleInMeeting) && State.CanChangeOutfit) {
                if (State.CycleName && cycleNameDelay <= 0) {
                    std::vector<std::string> validNames;
                    for (std::string i : State.cyclerUserNames) {
                        if (!IsNameValid(i)) continue; // Screw you, g0aty from the past
                        validNames.push_back(i);
                    }
                    for (auto p : GetAllPlayerControl()) {
                        if (p != *Game::pLocalPlayer && !((IsHost() || !State.SafeMode) && State.CycleForEveryone)) continue;
                        if (State.cyclerNameGeneration < 2 || (State.cyclerNameGeneration == 2 && ((IsHost() || !State.SafeMode) ? State.cyclerUserNames.empty() : validNames.empty()))) {
                            if (IsHost())
                                PlayerControl_RpcSetName(p, State.cyclerNameGeneration == 1 ?
                                    convert_to_string(GenerateRandomString(true)) : convert_to_string(GenerateRandomString()), NULL);
                            else
                                PlayerControl_CmdCheckName(p, State.cyclerNameGeneration == 1 ?
                                    convert_to_string(GenerateRandomString(true)) : convert_to_string(GenerateRandomString()), NULL);
                        }
                        else if (State.cyclerNameGeneration == 2) {
                            static int nameCtr = 0;
                            if (cycleNameDelay <= 0) {
                                if ((size_t)nameCtr >= ((IsHost() || !State.SafeMode) ? State.cyclerUserNames.size() : validNames.size()))
                                    nameCtr = 0;
                                if (IsHost()) PlayerControl_RpcSetName(p, convert_to_string(State.cyclerUserNames[nameCtr]), NULL);
                                else  PlayerControl_CmdCheckName(p, convert_to_string(State.cyclerUserNames[nameCtr]), NULL);
                                nameCtr++;
                            }
                        }
                        else {
                            if (IsHost()) PlayerControl_RpcSetName(p, convert_to_string(GenerateRandomString()), NULL);
                            else PlayerControl_CmdCheckName(p, convert_to_string(GenerateRandomString()), NULL);
                        }
                    }
                    cycleNameDelay = int(State.CycleTimer * GetFps()); // Far better
                }
                else cycleNameDelay--;

                if (colorChangeCycleDelay <= 0 && State.RandomColor && !State.activeImpersonation && !State.CycleForEveryone) {
                    if ((IsHost() || !State.SafeMode) && State.CycleForEveryone) {
                        for (auto p : GetAllPlayerControl()) {
                            PlayerControl_CmdCheckColor(p, GetRandomColorId(), NULL);
                        }
                    }
                    else PlayerControl_CmdCheckColor(*Game::pLocalPlayer, GetRandomColorId(), NULL);
                    colorChangeCycleDelay = int(State.CycleTimer * GetFps()); //idk how long this is
                }
                else colorChangeCycleDelay--;

                if (changeCycleDelay <= 0 && !State.activeImpersonation && (State.RandomHat || State.RandomSkin || State.RandomVisor || State.RandomPet || State.RandomNamePlate)) {
                    if (State.RandomHat) {
                        std::vector availableHats = { "hat_NoHat", "hat_AbominalHat", "hat_anchor", "hat_antenna", "hat_Antenna_Black", "hat_arrowhead", "hat_Astronaut-Blue", "hat_Astronaut-Cyan", "hat_Astronaut-Orange", "hat_astronaut", "hat_axe", "hat_babybean", "hat_Baguette", "hat_BananaGreen", "hat_BananaPurple", "hat_bandanaWBY", "hat_Bandana_Blue", "hat_Bandana_Green", "hat_Bandana_Pink", "hat_Bandana_Red", "hat_Bandana_White", "hat_Bandana_Yellow", "hat_baseball_Black", "hat_baseball_Green", "hat_baseball_Lightblue", "hat_baseball_LightGreen", "hat_baseball_Lilac", "hat_baseball_Orange", "hat_baseball_Pink", "hat_baseball_Purple", "hat_baseball_Red", "hat_baseball_White", "hat_baseball_Yellow", "hat_Basketball", "hat_bat_crewcolor", "hat_bat_green", "hat_bat_ice", "hat_beachball", "hat_Beanie_Black", "hat_Beanie_Blue", "hat_Beanie_Green", "hat_Beanie_Lightblue", "hat_Beanie_LightGreen", "hat_Beanie_LightPurple", "hat_Beanie_Pink", "hat_Beanie_Purple", "hat_Beanie_White", "hat_Beanie_Yellow", "hat_bearyCold", "hat_bone", "hat_Bowlingball", "hat_brainslug", "hat_BreadLoaf", "hat_bucket", "hat_bucketHat", "hat_bushhat", "hat_Butter", "hat_caiatl", "hat_caitlin", "hat_candycorn", "hat_captain", "hat_cashHat", "hat_cat_grey", "hat_cat_orange", "hat_cat_pink", "hat_cat_snow", "hat_chalice", "hat_cheeseBleu", "hat_cheeseMoldy", "hat_cheeseSwiss", "hat_ChefWhiteBlue", "hat_cherryOrange", "hat_cherryPink", "hat_Chocolate", "hat_chocolateCandy", "hat_chocolateMatcha", "hat_chocolateVanillaStrawb", "hat_clagger", "hat_clown_purple", "hat_comper", "hat_croissant", "hat_crownBean", "hat_crownDouble", "hat_crownTall", "hat_CuppaJoe", "hat_Deitied", "hat_devilhorns_black", "hat_devilhorns_crewcolor", "hat_devilhorns_green", "hat_devilhorns_murky", "hat_devilhorns_white", "hat_devilhorns_yellow", "hat_Doc_black", "hat_Doc_Orange", "hat_Doc_Purple", "hat_Doc_Red", "hat_Doc_White", "hat_Dodgeball", "hat_Dorag_Black", "hat_Dorag_Desert", "hat_Dorag_Jungle", "hat_Dorag_Purple", "hat_Dorag_Sky", "hat_Dorag_Snow", "hat_Dorag_Yellow", "hat_doubletophat", "hat_DrillMetal", "hat_DrillStone", "hat_DrillWood", "hat_EarmuffGreen", "hat_EarmuffsPink", "hat_EarmuffsYellow", "hat_EarnmuffBlue", "hat_eggGreen", "hat_eggYellow", "hat_enforcer", "hat_erisMorn", "hat_fairywings", "hat_fishCap", "hat_fishhed", "hat_fishingHat", "hat_flowerpot", "hat_frankenbolts", "hat_frankenbride", "hat_fungleFlower", "hat_geoff", "hat_glowstick", "hat_glowstickCyan", "hat_glowstickOrange", "hat_glowstickPink", "hat_glowstickPurple", "hat_glowstickYellow", "hat_goggles", "hat_Goggles_Black", "hat_Goggles_Chrome", "hat_GovtDesert", "hat_GovtHeadset", "hat_halospartan", "hat_hardhat", "hat_Hardhat_black", "hat_Hardhat_Blue", "hat_Hardhat_Green", "hat_Hardhat_Orange", "hat_Hardhat_Pink", "hat_Hardhat_Purple", "hat_Hardhat_Red", "hat_Hardhat_White", "hat_HardtopHat", "hat_headslug_Purple", "hat_headslug_Red", "hat_headslug_White", "hat_headslug_Yellow", "hat_Heart", "hat_heim", "hat_Herohood_Black", "hat_Herohood_Blue", "hat_Herohood_Pink", "hat_Herohood_Purple", "hat_Herohood_Red", "hat_Herohood_Yellow", "hat_hl_fubuki", "hat_hl_gura", "hat_hl_korone", "hat_hl_marine", "hat_hl_mio", "hat_hl_moona", "hat_hl_okayu", "hat_hl_pekora", "hat_hl_risu", "hat_hl_watson", "hat_hunter", "hat_IceCreamMatcha", "hat_IceCreamMint", "hat_IceCreamNeo", "hat_IceCreamStrawberry", "hat_IceCreamUbe", "hat_IceCreamVanilla", "hat_Igloo", "hat_Janitor", "hat_jayce", "hat_jinx", "hat_killerplant", "hat_lilShroom", "hat_maraSov", "hat_mareLwyd", "hat_military", "hat_MilitaryWinter", "hat_MinerBlack", "hat_MinerYellow", "hat_mira_bush", "hat_mira_case", "hat_mira_cloud", "hat_mira_flower", "hat_mira_flower_red", "hat_mira_gem", "hat_mira_headset_blue", "hat_mira_headset_pink", "hat_mira_headset_yellow", "hat_mira_leaf", "hat_mira_milk", "hat_mira_sign_blue", "hat_mohawk_bubblegum", "hat_mohawk_bumblebee", "hat_mohawk_purple_green", "hat_mohawk_rainbow", "hat_mummy", "hat_mushbuns", "hat_mushroomBeret", "hat_mysteryBones", "hat_NewYear2023", "hat_OrangeHat", "hat_osiris", "hat_pack01_Astronaut0001", "hat_pack02_Tengallon0001", "hat_pack02_Tengallon0002", "hat_pack03_Stickynote0004", "hat_pack04_Geoffmask0001", "hat_pack06holiday_candycane0001", "hat_PancakeStack", "hat_paperhat", "hat_Paperhat_Black", "hat_Paperhat_Blue", "hat_Paperhat_Cyan", "hat_Paperhat_Lightblue", "hat_Paperhat_Pink", "hat_Paperhat_Yellow", "hat_papermask", "hat_partyhat", "hat_pickaxe", "hat_Pineapple", "hat_PizzaSliceHat", "hat_pk01_BaseballCap", "hat_pk02_Crown", "hat_pk02_Eyebrows", "hat_pk02_HaloHat", "hat_pk02_HeroCap", "hat_pk02_PipCap", "hat_pk02_PlungerHat", "hat_pk02_ScubaHat", "hat_pk02_StickminHat", "hat_pk02_StrawHat", "hat_pk02_TenGallonHat", "hat_pk02_ThirdEyeHat", "hat_pk02_ToiletPaperHat", "hat_pk02_Toppat", "hat_pk03_Fedora", "hat_pk03_Goggles", "hat_pk03_Headphones", "hat_pk03_Security1", "hat_pk03_StrapHat", "hat_pk03_Traffic", "hat_pk04_Antenna", "hat_pk04_Archae", "hat_pk04_Balloon", "hat_pk04_Banana", "hat_pk04_Bandana", "hat_pk04_Beanie", "hat_pk04_Bear", "hat_pk04_BirdNest", "hat_pk04_CCC", "hat_pk04_Chef", "hat_pk04_DoRag", "hat_pk04_Fez", "hat_pk04_GeneralHat", "hat_pk04_HunterCap", "hat_pk04_JungleHat", "hat_pk04_MinerCap", "hat_pk04_MiniCrewmate", "hat_pk04_Pompadour", "hat_pk04_RamHorns", "hat_pk04_Slippery", "hat_pk04_Snowman", "hat_pk04_Vagabond", "hat_pk04_WinterHat", "hat_pk05_Burthat", "hat_pk05_Cheese", "hat_pk05_cheesetoppat", "hat_pk05_Cherry", "hat_pk05_davehat", "hat_pk05_Egg", "hat_pk05_Ellie", "hat_pk05_EllieToppat", "hat_pk05_Ellryhat", "hat_pk05_Fedora", "hat_pk05_Flamingo", "hat_pk05_FlowerPin", "hat_pk05_GeoffreyToppat", "hat_pk05_Helmet", "hat_pk05_HenryToppat", "hat_pk05_Macbethhat", "hat_pk05_Plant", "hat_pk05_RHM", "hat_pk05_Svenhat", "hat_pk05_Wizardhat", "hat_pk06_Candycanes", "hat_pk06_ElfHat", "hat_pk06_Lights", "hat_pk06_Present", "hat_pk06_Reindeer", "hat_pk06_Santa", "hat_pk06_Snowman", "hat_pk06_tree", "hat_pkHW01_BatWings", "hat_pkHW01_CatEyes", "hat_pkHW01_Horns", "hat_pkHW01_Machete", "hat_pkHW01_Mohawk", "hat_pkHW01_Pirate", "hat_pkHW01_PlagueHat", "hat_pkHW01_Pumpkin", "hat_pkHW01_ScaryBag", "hat_pkHW01_Witch", "hat_pkHW01_Wolf", "hat_Plunger_Blue", "hat_Plunger_Yellow", "hat_police", "hat_Ponytail", "hat_Pot", "hat_Present", "hat_Prototype", "hat_pusheenGreyHat", "hat_PusheenicornHat", "hat_pusheenMintHat", "hat_pusheenPinkHat", "hat_pusheenPurpleHat", "hat_pusheenSitHat", "hat_pusheenSleepHat", "hat_pyramid", "hat_rabbitEars", "hat_Ramhorn_Black", "hat_Ramhorn_Red", "hat_Ramhorn_White", "hat_ratchet", "hat_Records", "hat_RockIce", "hat_RockLava", "hat_Rubberglove", "hat_Rupert", "hat_russian", "hat_saint14", "hat_sausage", "hat_savathun", "hat_schnapp", "hat_screamghostface", "hat_Scrudge", "hat_sharkfin", "hat_shaxx", "hat_shovel", "hat_SlothHat", "hat_SnowbeanieGreen", "hat_SnowbeanieOrange", "hat_SnowBeaniePurple", "hat_SnowbeanieRed", "hat_Snowman", "hat_Soccer", "hat_Sorry", "hat_starBalloon", "hat_starhorse", "hat_Starless", "hat_StarTopper", "hat_stethescope", "hat_StrawberryLeavesHat", "hat_TenGallon_Black", "hat_TenGallon_White", "hat_ThomasC", "hat_tinFoil", "hat_titan", "hat_ToastButterHat", "hat_tombstone", "hat_tophat", "hat_ToppatHair", "hat_towelwizard", "hat_Traffic_Blue", "hat_traffic_purple", "hat_Traffic_Red", "hat_Traffic_Yellow", "hat_Unicorn", "hat_vi", "hat_viking", "hat_Visor", "hat_Voleyball", "hat_w21_candycane_blue", "hat_w21_candycane_bubble", "hat_w21_candycane_chocolate", "hat_w21_candycane_mint", "hat_w21_elf_pink", "hat_w21_elf_swe", "hat_w21_gingerbread", "hat_w21_holly", "hat_w21_krampus", "hat_w21_lights_white", "hat_w21_lights_yellow", "hat_w21_log", "hat_w21_mistletoe", "hat_w21_mittens", "hat_w21_nutcracker", "hat_w21_pinecone", "hat_w21_present_evil", "hat_w21_present_greenyellow", "hat_w21_present_redwhite", "hat_w21_present_whiteblue", "hat_w21_santa_evil", "hat_w21_santa_green", "hat_w21_santa_mint", "hat_w21_santa_pink", "hat_w21_santa_white", "hat_w21_santa_yellow", "hat_w21_snowflake", "hat_w21_snowman", "hat_w21_snowman_evil", "hat_w21_snowman_greenred", "hat_w21_snowman_redgreen", "hat_w21_snowman_swe", "hat_w21_winterpuff", "hat_wallcap", "hat_warlock", "hat_whitetophat", "hat_wigJudge", "hat_wigTall", "hat_WilfordIV", "hat_Winston", "hat_WinterGreen", "hat_WinterHelmet", "hat_WinterRed", "hat_WinterYellow", "hat_witch_green", "hat_witch_murky", "hat_witch_pink", "hat_witch_white", "hat_wolf_grey", "hat_wolf_murky", "hat_Zipper" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetHat(p, convert_to_string(availableHats[randi(0, availableHats.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetHat(*Game::pLocalPlayer, convert_to_string(availableHats[randi(0, availableHats.size() - 1)]), NULL);
                    }
                    if (State.RandomSkin) {
                        std::vector availableSkins = { "skin_None", "skin_Abominalskin", "skin_ApronGreen", "skin_Archae", "skin_Astro", "skin_Astronaut-Blueskin", "skin_Astronaut-Cyanskin", "skin_Astronaut-Orangeskin", "skin_Bananaskin", "skin_benoit", "skin_Bling", "skin_BlueApronskin", "skin_BlueSuspskin", "skin_Box1skin", "skin_BubbleWrapskin", "skin_Burlapskin", "skin_BushSign1skin", "skin_Bushskin", "skin_BusinessFem-Aquaskin", "skin_BusinessFem-Tanskin", "skin_BusinessFemskin", "skin_caitlin", "skin_Capt", "skin_CCC", "skin_ChefBlackskin", "skin_ChefBlue", "skin_ChefRed", "skin_clown", "skin_D2Cskin", "skin_D2Hunter", "skin_D2Osiris", "skin_D2Saint14", "skin_D2Shaxx", "skin_D2Titan", "skin_D2Warlock", "skin_enforcer", "skin_fairy", "skin_FishingSkinskin", "skin_fishmonger", "skin_FishSkinskin", "skin_General", "skin_greedygrampaskin", "skin_halospartan", "skin_Hazmat-Blackskin", "skin_Hazmat-Blueskin", "skin_Hazmat-Greenskin", "skin_Hazmat-Pinkskin", "skin_Hazmat-Redskin", "skin_Hazmat-Whiteskin", "skin_Hazmat", "skin_heim", "skin_hl_fubuki", "skin_hl_gura", "skin_hl_korone", "skin_hl_marine", "skin_hl_mio", "skin_hl_moona", "skin_hl_okayu", "skin_hl_pekora", "skin_hl_risu", "skin_hl_watson", "skin_Horse1skin", "skin_Hotdogskin", "skin_InnerTubeSkinskin", "skin_JacketGreenskin", "skin_JacketPurpleskin", "skin_JacketYellowskin", "skin_Janitorskin", "skin_jayce", "skin_jinx", "skin_LifeVestSkinskin", "skin_Mech", "skin_MechanicRed", "skin_Military", "skin_MilitaryDesert", "skin_MilitarySnowskin", "skin_Miner", "skin_MinerBlackskin", "skin_mummy", "skin_OrangeSuspskin", "skin_PinkApronskin", "skin_PinkSuspskin", "skin_Police", "skin_presentskin", "skin_prisoner", "skin_PrisonerBlue", "skin_PrisonerTanskin", "skin_pumpkin", "skin_PusheenGreyskin", "skin_Pusheenicornskin", "skin_PusheenMintskin", "skin_PusheenPinkskin", "skin_PusheenPurpleskin", "skin_ratchet", "skin_rhm", "skin_RockIceskin", "skin_RockLavaskin", "skin_Sack1skin", "skin_scarfskin", "skin_Science", "skin_Scientist-Blueskin", "skin_Scientist-Darkskin", "skin_screamghostface", "skin_Security", "skin_Skin_SuitRedskin", "skin_Slothskin", "skin_SportsBlueskin", "skin_SportsRedskin", "skin_SuitB", "skin_SuitW", "skin_SweaterBlueskin", "skin_SweaterPinkskin", "skin_Sweaterskin", "skin_SweaterYellowskin", "skin_Tarmac", "skin_ToppatSuitFem", "skin_ToppatVest", "skin_uglysweaterskin", "skin_vampire", "skin_vi", "skin_w21_deer", "skin_w21_elf", "skin_w21_msclaus", "skin_w21_nutcracker", "skin_w21_santa", "skin_w21_snowmate", "skin_w21_tree", "skin_Wall", "skin_Winter", "skin_witch", "skin_YellowApronskin", "skin_YellowSuspskin" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetSkin(p, convert_to_string(availableSkins[randi(0, availableSkins.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetSkin(*Game::pLocalPlayer, convert_to_string(availableSkins[randi(0, availableSkins.size() - 1)]), NULL);
                    }
                    if (State.RandomVisor) {
                        std::vector availableVisors = { "visor_EmptyVisor", "visor_anime", "visor_BaconVisor", "visor_BananaVisor", "visor_beautyMark", "visor_BillyG", "visor_Blush", "visor_Bomba", "visor_BubbleBumVisor", "visor_Candycane", "visor_Carrot", "visor_chimkin", "visor_clownnose", "visor_Crack", "visor_CucumberVisor", "visor_D2CGoggles", "visor_Dirty", "visor_Dotdot", "visor_doubleeyepatch", "visor_eliksni", "visor_erisBandage", "visor_eyeball", "visor_EyepatchL", "visor_EyepatchR", "visor_fishhook", "visor_Galeforce", "visor_heim", "visor_hl_ah", "visor_hl_bored", "visor_hl_hmph", "visor_hl_marine", "visor_hl_nothoughts", "visor_hl_nudge", "visor_hl_smug", "visor_hl_sweepy", "visor_hl_teehee", "visor_hl_wrong", "visor_IceBeard", "visor_IceCreamChocolateVisor", "visor_IceCreamMintVisor", "visor_IceCreamStrawberryVisor", "visor_IceCreamUbeVisor", "visor_is_beard", "visor_JanitorStache", "visor_jinx", "visor_Krieghaus", "visor_Lava", "visor_LolliBlue", "visor_LolliBrown", "visor_LolliOrange", "visor_lollipopCrew", "visor_lollipopLemon", "visor_lollipopLime", "visor_LolliRed", "visor_marshmallow", "visor_masque_blue", "visor_masque_green", "visor_masque_red", "visor_masque_white", "visor_mira_card_blue", "visor_mira_card_red", "visor_mira_glasses", "visor_mira_mask_black", "visor_mira_mask_blue", "visor_mira_mask_green", "visor_mira_mask_purple", "visor_mira_mask_red", "visor_mira_mask_white", "visor_Mouth", "visor_mummy", "visor_PiercingL", "visor_PiercingR", "visor_PizzaVisor", "visor_pk01_AngeryVisor", "visor_pk01_DumStickerVisor", "visor_pk01_FredVisor", "visor_pk01_HazmatVisor", "visor_pk01_MonoclesVisor", "visor_pk01_PaperMaskVisor", "visor_pk01_PlagueVisor", "visor_pk01_RHMVisor", "visor_pk01_Security1Visor", "visor_Plsno", "visor_polus_ice", "visor_pusheenGorgeousVisor", "visor_pusheenKissyVisor", "visor_pusheenKoolKatVisor", "visor_pusheenOmNomNomVisor", "visor_pusheenSmileVisor", "visor_pusheenYaaaaaayVisor", "visor_Reginald", "visor_Rudolph", "visor_savathun", "visor_Scar", "visor_SciGoggles", "visor_shopglasses", "visor_shuttershadesBlue", "visor_shuttershadesLime", "visor_shuttershadesPink", "visor_shuttershadesPurple", "visor_shuttershadesWhite", "visor_shuttershadesYellow", "visor_SkiGoggleBlack", "visor_SKiGogglesOrange", "visor_SkiGogglesWhite", "visor_SmallGlasses", "visor_SmallGlassesBlue", "visor_SmallGlassesRed", "visor_starfish", "visor_Stealthgoggles", "visor_Stickynote_Cyan", "visor_Stickynote_Green", "visor_Stickynote_Orange", "visor_Stickynote_Pink", "visor_Stickynote_Purple", "visor_Straw", "visor_sunscreenv", "visor_teary", "visor_ToastVisor", "visor_tvColorTest", "visor_vr_Vr-Black", "visor_vr_Vr-White", "visor_w21_carrot", "visor_w21_nutstache", "visor_w21_nye", "visor_w21_santabeard", "visor_wash", "visor_WinstonStache" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetVisor(p, convert_to_string(availableVisors[randi(0, availableVisors.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetVisor(*Game::pLocalPlayer, convert_to_string(availableVisors[randi(0, availableVisors.size() - 1)]), NULL);
                    }
                    if (State.RandomPet) {
                        std::vector availablePets = { "pet_EmptyPet", "pet_Alien", "pet_Bedcrab", "pet_BredPet", "pet_Bush", "pet_Charles", "pet_Charles_Red", "pet_ChewiePet", "pet_clank", "pet_coaltonpet", "pet_Creb", "pet_Crewmate", "pet_Cube", "pet_D2GhostPet", "pet_D2PoukaPet", "pet_D2WormPet", "pet_Doggy", "pet_Ellie", "pet_frankendog", "pet_GuiltySpark", "pet_HamPet", "pet_Hamster", "pet_HolidayHamPet", "pet_Lava", "pet_nuggetPet", "pet_Pip", "pet_poro", "pet_Pusheen", "pet_Robot", "pet_Snow", "pet_Squig", "pet_Stickmin", "pet_Stormy", "pet_test", "pet_UFO", "pet_YuleGoatPet" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetPet(p, convert_to_string(availablePets[randi(0, availablePets.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetPet(*Game::pLocalPlayer, convert_to_string(availablePets[randi(0, availablePets.size() - 1)]), NULL);
                    }
                    if (State.RandomNamePlate) {
                        std::vector availableNamePlates = { "nameplate_NoPlate", "nameplate_airship_Toppat", "nameplate_airship_CCC", "nameplate_airship_Diamond", "nameplate_airship_Emerald", "nameplate_airship_Gems", "nameplate_airship_government", "nameplate_Airship_Hull", "nameplate_airship_Ruby", "nameplate_airship_Sky", "nameplate_Polus-Skyline", "nameplate_Polus-Snowmates", "nameplate_Polus_Colors", "nameplate_Polus_DVD", "nameplate_Polus_Ground", "nameplate_Polus_Lava", "nameplate_Polus_Planet", "nameplate_Polus_Snow", "nameplate_Polus_SpecimenBlue", "nameplate_Polus_SpecimenGreen", "nameplate_Polus_SpecimenPurple", "nameplate_is_yard", "nameplate_is_dig", "nameplate_is_game", "nameplate_is_ghost", "nameplate_is_green", "nameplate_is_sand", "nameplate_is_trees", "nameplate_Mira_Cafeteria", "nameplate_Mira_Glass", "nameplate_Mira_Tiles", "nameplate_Mira_Vines", "nameplate_Mira_Wood", "nameplate_hw_candy", "nameplate_hw_woods", "nameplate_hw_pumpkin" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetNamePlate(p, convert_to_string(availableNamePlates[randi(0, availableNamePlates.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetNamePlate(*Game::pLocalPlayer, convert_to_string(availableNamePlates[randi(0, availableNamePlates.size() - 1)]), NULL);
                    }
                    changeCycleDelay = int(State.CycleTimer * GetFps());
                }
                else changeCycleDelay--;
            }

            if ((IsHost() || !State.SafeMode) && State.ForceColorForEveryone)
            {
                static int forceColorDelay = 0;
                for (auto player : GetAllPlayerControl()) {
                    if (forceColorDelay <= 0) {
                        auto outfit = GetPlayerOutfit(GetPlayerData(player));
                        auto colorId = outfit->fields.ColorId;
                        if (IsInGame() && colorId != State.HostSelectedColorId)
                            State.rpcQueue.push(new RpcForceColor(player, State.HostSelectedColorId));
                        else if (IsInLobby() && colorId != State.HostSelectedColorId)
                            State.lobbyRpcQueue.push(new RpcForceColor(player, State.HostSelectedColorId));
                        forceColorDelay = int(0.5 * GetFps());
                    }
                    else {
                        forceColorDelay--;
                    }
                }
            }

            if ((IsHost() || !State.SafeMode) && State.ForceNameForEveryone) {
                static int forceNameDelay = 0;
                if (forceNameDelay <= 0) {
                    for (auto player : GetAllPlayerControl()) {
                        if (!(State.CustomName && State.ServerSideCustomName && (player == *Game::pLocalPlayer || State.CustomNameForEveryone))) {
                            auto outfit = GetPlayerOutfit(GetPlayerData(player));
                            std::string playerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(player), nullptr));
                            std::string newName = std::format("{}<size=0><{}></size>", State.hostUserName, player->fields.PlayerId);
                            if (playerName == newName) continue;
                            if (IsHost()) {
                                PlayerControl_RpcSetName(player, convert_to_string(newName), NULL);
                                continue;
                            }
                            if (!State.SafeMode)
                                PlayerControl_CmdCheckName(player, convert_to_string(newName), NULL);
                        }
                    }
                    forceNameDelay = int(0.5 * GetFps());
                }
                else {
                    forceNameDelay--;
                }
            }

            static float playerCycleDelay = 0;

            if (State.CycleBetweenPlayers && (IsInGame() || IsInLobby()) && (!State.InMeeting || State.CycleInMeeting) && State.CanChangeOutfit) {
                if (playerCycleDelay <= 0) {
                    std::vector<PlayerControl*> players = {};
                    for (auto player : GetAllPlayerControl()) {
                        if (GetPlayerData(player)->fields.Disconnected || player == *Game::pLocalPlayer)
                            continue; //we don't want to crash or expose ourselves
                        players.push_back(player);
                    }
                    if (players.empty())
                        playerCycleDelay = State.CycleDuration;
                    else if (IsInGame() || IsInLobby()) {
                        int rand = randi(0, (int)players.size() - 1);
                        NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(GetPlayerData(players[rand]));
                        ImpersonateName(GetPlayerData(players[rand]));
                        ImpersonateOutfit(outfit);
                        State.rpcQueue.push(new RpcSetLevel(*Game::pLocalPlayer, GetPlayerData(players[rand])->fields.PlayerLevel));
                        playerCycleDelay = State.CycleDuration;
                        State.activeImpersonation = true;
                    }
                }
                else if (playerCycleDelay > 0)
                    playerCycleDelay--;
                else
                    playerCycleDelay = 0;
            }

            static int attachDelay = 0;
            auto playerToAttach = State.playerToAttach.validate();

            if (State.ActiveAttach && State.playerToAttach.has_value()) {
                if (attachDelay <= 0) {
                    auto pos = GetTrueAdjustedPosition(playerToAttach.get_PlayerControl());
                    if (State.AprilFoolsMode) pos.x -= (randi(0, 1) ? 0.5f : 0.2f);
                    CustomNetworkTransform_RpcSnapTo((*Game::pLocalPlayer)->fields.NetTransform, pos, NULL);
                    attachDelay = int(0.1 * GetFps());
                }
                else attachDelay--;
            }

            // Shift/Ctrl + Right-click Teleport
            static int ctrlRightClickDelay = 0;

            if ((IsInGame() || IsInLobby()) && !State.InMeeting && State.ShiftRightClickTP) {
                ImVec2 mouse = ImGui::GetMousePos();
                Vector2 target = { mouse.x, (DirectX::GetWindowSize().y - mouse.y) };
                bool isValid = target.x != 0.f && target.y != 0.f; // Prevent teleporting to origin
                if (isValid && ImGui::IsKeyDown(VK_SHIFT) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    if (IsInGame()) State.rpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                    if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                }
                else if (isValid && ImGui::IsKeyDown(VK_CONTROL) && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                    if (ctrlRightClickDelay <= 0) {
                        ImVec2 mouse = ImGui::GetMousePos();
                        Vector2 target = { mouse.x, (DirectX::GetWindowSize().y - mouse.y) };
                        if (IsInGame()) State.rpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                        if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                        ctrlRightClickDelay = int(0.1 * GetFps());
                    }
                    else ctrlRightClickDelay--;
                }
            }

            if ((IsInGame() || IsInLobby()) && State.GodMode && ((IsHost() && IsInGame()) || !State.SafeMode)) {
                if (State.protectMonitor.find((*Game::pLocalPlayer)->fields.PlayerId) == State.protectMonitor.end()) {
                    PlayerControl_RpcProtectPlayer(*Game::pLocalPlayer, *Game::pLocalPlayer, GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer))->fields.ColorId, NULL);
                }
            }

            if (IsInGame() || IsInLobby()) {
                if ((*Game::pLocalPlayer)->fields.inMovingPlat)
                    (*Game::pLocalPlayer)->fields.MyPhysics->fields.Speed = 2.5; //remove speed on moving platform to avoid slowing down
                else
                    (*Game::pLocalPlayer)->fields.MyPhysics->fields.Speed = State.MultiplySpeed ? (float)(2.5 * State.PlayerSpeed) : 2.5f;
                (*Game::pLocalPlayer)->fields.MyPhysics->fields.GhostSpeed = State.MultiplySpeed ? (float)(2.5 * State.PlayerSpeed) : 2.5f;
            }
            if (IsInGame() || IsInLobby()) {
                auto localData = GetPlayerData(*Game::pLocalPlayer);
                auto roleType = localData->fields.RoleType;
                bool roleAssigned = (*Game::pLocalPlayer)->fields.roleAssigned;

                if (!localData->fields.IsDead && (State.RealRole == RoleTypes__Enum::GuardianAngel || State.RealRole == RoleTypes__Enum::CrewmateGhost || State.RealRole == RoleTypes__Enum::ImpostorGhost))
                    State.IsRevived = true;
                else
                    State.IsRevived = false;

                std::queue<RPCInterface*>* queue = IsInGame() ? &State.rpcQueue : &State.lobbyRpcQueue;
                if (!IsHost() && (IsInMultiplayerGame() || IsInLobby())) {
                    if (roleType == RoleTypes__Enum::GuardianAngel && State.RealRole != RoleTypes__Enum::GuardianAngel) {
                        queue->push(new SetRole(RoleTypes__Enum::CrewmateGhost)); //prevent being unable to protect
                    }
                }

                if (IsHost() && IsInLobby() && State.AutoStartGame && (600 - State.LobbyTimer) >= State.AutoStartTimer && !autoStartedGame) {
                    autoStartedGame = true;
                    InnerNetClient_SendStartGame(__this, NULL);
                }

                static int sabotageDelay = 0;
                static bool fixSabotage = false;
                if (sabotageDelay <= 0) {
                    if (IsInGame()) {
                        if (State.mapType != Settings::MapType::Fungle && State.DisableLightSwitches) {
                            for (int i = 0; i < 5; ++i) {
                                if (randi(0, 1)) ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Electrical, i, NULL);
                            }
                        }
                        if (fixSabotage) {
                            RepairSabotage(*Game::pLocalPlayer);
                            if (State.SpamDoors) {
                                for (auto door : il2cpp::Array((*Game::pShipStatus)->fields.AllDoors)) {
                                    ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Doors, (uint8_t)(door->fields.Id | 64), NULL);
                                    ShipStatus_UpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Doors, *Game::pLocalPlayer, (uint8_t)(door->fields.Id | 64), NULL);
                                }
                            }
                        }
                        else {
                            if (State.DisableComms) {
                                ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Comms, 128, NULL);
                            }
                            if (State.DisableReactor) {
                                if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle)
                                    ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Reactor, 128, NULL);
                                else if (State.mapType == Settings::MapType::Pb)
                                    ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Laboratory, 128, NULL);
                                else if (State.mapType == Settings::MapType::Airship)
                                    ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::HeliSabotage, 128, NULL);
                            }
                            if ((State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq) && State.DisableOxygen) {
                                ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::LifeSupp, 128, NULL);
                            }
                            if (State.mapType == Settings::MapType::Fungle && State.InfiniteMushroomMixup) {
                                ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::MushroomMixupSabotage, 1, NULL);
                            }
                            if ((State.mapType == Settings::MapType::Pb || State.mapType == Settings::MapType::Airship || State.mapType == Settings::MapType::Fungle)
                                && State.SpamDoors) {
                                for (auto door : il2cpp::Array((*Game::pShipStatus)->fields.AllDoors)) {
                                    ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, door->fields.Room, NULL);
                                }
                            }
                        }
                    }
                    sabotageDelay = int(0.2 * GetFps());
                }
                else sabotageDelay--;
            }

            if (!IsHost() && (IsInGame() || IsInLobby())) {
                State.DisableCallId = false;
                State.DisableKills = false;
                State.DisableMeetings = false;
                State.DisableSabotages = false;
            }
        }

        if (State.BanEveryone || State.KickEveryone) {
            auto allPlayers = GetAllPlayerControl();

            uint32_t localPlayerId = 0;
            if (Game::pLocalPlayer && *Game::pLocalPlayer)
                localPlayerId = (*Game::pLocalPlayer)->fields.PlayerId;

            auto now = std::chrono::steady_clock::now();

            for (auto playerControl : allPlayers) {
                if (!playerControl || playerControl->fields.PlayerId == localPlayerId) continue;

                auto playerData = GetPlayerDataById(playerControl->fields.PlayerId);
                if (!playerData) continue;

                if (State.Ban_IgnoreWhitelist && std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), convert_from_string(playerData->fields.FriendCode)) != State.WhitelistFriendCodes.end()) {
                    continue;
                }

                uint32_t playerId = playerControl->fields.PlayerId;

                if (State.playerPunishTimers.find(playerId) == State.playerPunishTimers.end()) {
                    State.playerPunishTimers[playerId] = now;
                }

                float delaySeconds = State.AutoPunishDelay;
                auto elapsed = std::chrono::duration<float>(now - State.playerPunishTimers[playerId]).count();

                if (elapsed >= delaySeconds) {
                    bool ShouldBan = State.BanEveryone;
                    app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), playerControl->fields._.OwnerId, ShouldBan, NULL);
                    State.playerPunishTimers.erase(playerId);
                }
            }

            std::vector<uint32_t> activeIds;
            for (auto player : allPlayers) {
                if (player) activeIds.push_back(player->fields.PlayerId);
            }

            for (auto it = State.playerPunishTimers.begin(); it != State.playerPunishTimers.end();) {
                if (std::find(activeIds.begin(), activeIds.end(), it->first) == activeIds.end()) {
                    it = State.playerPunishTimers.erase(it);
                }
                else {
                    ++it;
                }
            }
        }
        
        if (State.KickByLockedName) {
            const auto allPlayers = GetAllPlayerControl();

            const std::unordered_set<std::string> BannedNamesSet(State.LockedNames.begin(), State.LockedNames.end());

            for (auto* pc : allPlayers) {
                if (!pc || pc == *Game::pLocalPlayer) continue;

                auto* pd = GetPlayerDataById(pc->fields.PlayerId);
                if (!pd) continue;

                const std::string name = RemoveHtmlTags(convert_from_string(GetPlayerOutfit(GetPlayerData(pc))->fields.PlayerName));
                const std::string puid = convert_from_string(pd->fields.Puid);
                const std::string fc = convert_from_string(pd->fields.FriendCode);

                State.CurrentNames.insert(name);

                if (!BannedNamesSet.contains(name)) continue;

                if (State.Ban_IgnoreWhitelist &&
                    std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), fc) != State.WhitelistFriendCodes.end()) {
                    continue;
                }

                State.CurrentForbiddenNames.insert(name);

                if (State.ForbiddenNames.contains(name)) continue;

                State.ForbiddenNames.insert(name);

                app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), pc->fields._.OwnerId, false, NULL);

                if (auto* notifier = (NotificationPopper*)Game::HudManager.GetInstance()->fields.Notifier) {
                    auto* spriteBackup = new Sprite(*notifier->fields.playerDisconnectSprite);
                    const auto colorBackup = notifier->fields.disconnectColor;

                    notifier->fields.playerDisconnectSprite = notifier->fields.settingsChangeSprite;
                    notifier->fields.disconnectColor = Color(1.0f, 0.0118f, 0.2431f, 1.0f);

                    const std::string kickMsg = std::format("<#FFF><b>{}</color> detected by Name-Checker!</b>", name);
                    NotificationPopper_AddDisconnectMessage(notifier, convert_to_string(kickMsg), NULL);

                    notifier->fields.playerDisconnectSprite = spriteBackup;
                    notifier->fields.disconnectColor = colorBackup;
                }

                if (State.ShowPDataByNC) {
                    const std::string pdataMsg = std::format("<#ff033e><font=\"Barlow-Regular Outline\"><b>Name-Checker ~ Player Data:\n<voffset=-0.5>*</voffset> [<#FFF>{}</color>]\n\n<size=75%>Product User ID: <#FFF>{}</color>\nFriend Code: <#FFF>{}</b></font></size></color>", name, puid.empty() ? "<#F00>NONE</color>" : puid, fc.empty() ? "<#F00>NONE</color>" : fc);
                    ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(pdataMsg), NULL);
                }
            }

            State.ForbiddenNames = std::move(State.CurrentForbiddenNames);
        }

        if (State.KickByWhitelist) {
            const auto allPlayers = GetAllPlayerControl();

            for (auto* pc : allPlayers) {
                if (!pc) continue;

                if (pc->fields.PlayerId == (*Game::pLocalPlayer)->fields.PlayerId) continue;

                auto* pd = GetPlayerDataById(pc->fields.PlayerId);
                if (!pd) continue;

                const std::string name = RemoveHtmlTags(convert_from_string(GetPlayerOutfit(GetPlayerData(pc))->fields.PlayerName));
                const std::string fc = convert_from_string(pd->fields.FriendCode);

                if (std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), fc) != State.WhitelistFriendCodes.end()) {
                    continue;
                }

                InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), pc->fields._.OwnerId, false, NULL);

                if (State.WhitelistNotifications && State.NotifiedFriendCodes.find(fc) == State.NotifiedFriendCodes.end()) {
                    State.NotifiedFriendCodes.insert(fc);

                    std::string pdataMsg = std::format("<font=\"Barlow-Regular Outline\"><b><#FFF>({})</color> <#ff033e>tried to join to your server, but was kicked by a whitelist.</color></b></font></material>\n\n", fc);

                    if (State.ExtraCommands) {
                        pdataMsg += std::format("<font=\"Barlow-Regular Outline\"><b><#0F0>Use the command: <#fff>\"/Add {}\"</color>\nto whitelist player!</color></b></font></material>", fc);
                    }

                    ChatController_AddChatWarning(Game::HudManager.GetInstance()->fields.Chat, convert_to_string(pdataMsg), NULL);
                }
            }
        }
        if (!IsInLobby() && !IsInGame()) {
            State.NotifiedFriendCodes.clear();
        }

        if (State.BanLeavers) {
            auto allPlayers = GetAllPlayerControl();
            std::unordered_set<std::string> currentFriendCodes;

            std::string localFC;
            if (*Game::pLocalPlayer) {
                if (auto* localPD = GetPlayerDataById((*Game::pLocalPlayer)->fields.PlayerId)) {
                    localFC = convert_from_string(localPD->fields.FriendCode);
                }
            }

            for (auto* pc : allPlayers) {
                if (!pc || pc == *Game::pLocalPlayer) continue;

                auto* pd = GetPlayerDataById(pc->fields.PlayerId);
                if (!pd) continue;

                const std::string fc = convert_from_string(pd->fields.FriendCode);
                if (fc == localFC) continue;

                currentFriendCodes.insert(fc);

                if (State.Ban_IgnoreWhitelist && std::ranges::find(State.WhitelistFriendCodes, fc) != State.WhitelistFriendCodes.end()) continue;

                State.activeFriendCodes.insert(fc);

                if (State.joinLeaveCount[fc] > static_cast<int>(State.LeaveCount) - 1) {
                    app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), pc->fields._.OwnerId, true, nullptr);

                    if (State.BL_AutoLeavers && std::ranges::find(State.BlacklistFriendCodes, fc) == State.BlacklistFriendCodes.end()) {
                        State.BlacklistFriendCodes.push_back(fc);
                    }

                    State.activeFriendCodes.erase(fc);
                }
            }

            for (const auto& fc : State.activeFriendCodes) {
                if (fc != localFC && currentFriendCodes.find(fc) == currentFriendCodes.end()) {
                    State.joinLeaveCount[fc]++;
                }
            }

            for (auto it = State.activeFriendCodes.begin(); it != State.activeFriendCodes.end(); ) {
                if (currentFriendCodes.find(*it) == currentFriendCodes.end() && *it != localFC)
                    it = State.activeFriendCodes.erase(it);
                else
                    ++it;
            }

            if (!localFC.empty()) {
                State.joinLeaveCount[localFC] = 0;
                State.activeFriendCodes.erase(localFC);
            }
        }

        if (!IsInLobby() && !IsInGame()) {
            State.joinLeaveCount.clear();
            State.activeFriendCodes.clear();
        }

        if (State.BanWarned || State.KickWarned) {
            auto allPlayers = GetAllPlayerControl();
            std::unordered_set<std::string> currentNotifiedCodes;

            for (auto playerControl : allPlayers) {
                if (!playerControl || playerControl == *Game::pLocalPlayer) continue;

                auto playerData = GetPlayerDataById(playerControl->fields.PlayerId);
                if (!playerData) continue;

                std::string friendCode = convert_from_string(playerData->fields.FriendCode);

                if (State.Ban_IgnoreWhitelist &&
                    std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), friendCode) != State.WhitelistFriendCodes.end()) {
                    continue;
                }

                int warnCount = State.WarnedFriendCodes[friendCode];

                if (warnCount >= State.MaxWarns) {
                    currentNotifiedCodes.insert(friendCode);

                    if (State.NotifiedWarnedPlayers.contains(friendCode)) continue;

                    State.NotifiedWarnedPlayers.insert(friendCode);

                    if (auto* notifier = (NotificationPopper*)Game::HudManager.GetInstance()->fields.Notifier) {

                        auto* spriteBackup = new Sprite(*notifier->fields.playerDisconnectSprite);
                        Color colorBackup = notifier->fields.disconnectColor;

                        notifier->fields.playerDisconnectSprite = notifier->fields.settingsChangeSprite;
                        notifier->fields.disconnectColor = Color(1.0f, 0.0118f, 0.2431f, 1.0f);

                        std::string action = State.BanWarned ? "banned" : "kicked";
                        std::string kickMsg = std::format("<#FFF><b>\"{}\" was {} for receiving {} warns</b>", friendCode, action, State.MaxWarns);
                        NotificationPopper_AddDisconnectMessage(notifier, convert_to_string(kickMsg), NULL);

                        notifier->fields.playerDisconnectSprite = spriteBackup;
                        notifier->fields.disconnectColor = colorBackup;
                    }

                    if (State.BanWarned) {
                        app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), playerControl->fields._.OwnerId, true, NULL);
                    }
                    else if (State.KickWarned) {
                        app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), playerControl->fields._.OwnerId, false, NULL);
                    }
                }
            }
            State.NotifiedWarnedPlayers = std::move(currentNotifiedCodes);
        }

        static bool hasExited = false;
        static bool wasLowFps = false;

        if (State.LeaveDueLFPS) {
            const auto currentTime = std::chrono::steady_clock::now();
            const std::chrono::duration<float> elapsed = currentTime - State.lastFrameTime;
            State.lastFrameTime = currentTime;

            const float fps = 1.0f / elapsed.count();

            if (IsInLobby() || IsInGame()) {
                if (fps < static_cast<float>(State.minFpsThreshold)) {
                    if (!wasLowFps) {
                        State.lowFpsStartTime = currentTime;
                        wasLowFps = true;
                    }
                    else {
                        if (!hasExited && std::chrono::duration<float>(currentTime - State.lowFpsStartTime).count() >= 0.35f) {
                            app::AmongUsClient_ExitGame((*Game::pAmongUsClient), DisconnectReasons__Enum::ExitGame, NULL);
                            hasExited = true;
                        }
                    }
                }
                else {
                    wasLowFps = false;
                }
            }
            else {
                hasExited = false;
                wasLowFps = false;
            }
        }
    }
    catch (Exception* ex) {
        onGameEnd();
        InnerNetClient_DisconnectInternal(__this, DisconnectReasons__Enum::Error, convert_to_string("InnerNetClient_Update exception"), NULL);
        InnerNetClient_EnqueueDisconnect(__this, DisconnectReasons__Enum::Error, convert_to_string("InnerNetClient_Update exception"), NULL);
        LOG_DEBUG("InnerNetClient_Update Exception " + convert_from_string(ex->fields._message));
    }
    catch (...) {
        LOG_ERROR("Exception occurred in InnerNetClient_Update (InnerNetClient)");
    }
    Application_set_targetFrameRate(State.GameFPS > 1 ? State.GameFPS : 60, NULL);
    InnerNetClient_Update(__this, method);

    static int SpamPlatformDelay = 10;
    if (SpamPlatformDelay <= 0) {
        if (State.SpamMovingPlatform) {
            State.rpcQueue.push(new RpcUsePlatform());
            SpamPlatformDelay = 10;
        }
    }
    else {
        SpamPlatformDelay--;
    }


    static int AutoRepairSabotageDelay = 100;
    if (AutoRepairSabotageDelay <= 0) {
        if (State.AutoRepairSabotage) {
            RepairSabotage(*Game::pLocalPlayer);
            AutoRepairSabotageDelay = 100;
        }
    }
    else {
        AutoRepairSabotageDelay--;
    }
}

void dAmongUsClient_OnGameJoined(AmongUsClient* __this, String* gameIdString, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dAmongUsClient_OnGameJoined executed");
    try {
        State.AutoJoinLobby = false;
        if (!State.PanicMode) {
            Log.Debug("Joined lobby " + convert_from_string(gameIdString));
            State.LastLobbyJoined = convert_from_string(gameIdString);
            if (!State.PanicMode) {
                State.PanicMode = true;
                State.TempPanicMode = true;
            }
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in AmongUsClient_OnGameJoined (InnerNetClient)");
    }
    AmongUsClient_OnGameJoined(__this, gameIdString, method);
}

void dAmongUsClient_OnPlayerLeft(AmongUsClient* __this, ClientData* data, DisconnectReasons__Enum reason, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dAmongUsClient_OnPlayerLeft executed");
    try {
        State.BlinkPlayersTab = true;
        if (data->fields.Character) { // Don't use Object_1_IsNotNull().
            auto playerInfo = GetPlayerData(data->fields.Character);

            if (reason == DisconnectReasons__Enum::Banned)
                Log.Debug(ToString(data->fields.Character) + " has been banned by host (" + GetHostUsername() + ").");
            else if (reason == DisconnectReasons__Enum::Kicked)
                Log.Debug(ToString(data->fields.Character) + " has been kicked by host (" + GetHostUsername() + ").");
            else if (reason == DisconnectReasons__Enum::Hacking)
                Log.Debug(ToString(data->fields.Character) + " has been banned for hacking.");
            else if (reason == DisconnectReasons__Enum::Error)
                Log.Debug(ToString(data->fields.Character) + " has been disconnected due to error.");
            else if (reason == DisconnectReasons__Enum::Sanctions)
                Log.Debug(ToString(data->fields.Character) + " has been sanction-banned.");
            else
                Log.Debug(ToString(data->fields.Character) + " has left the game.");

            uint8_t playerId = data->fields.Character->fields.PlayerId;

            if (State.modUsers.find(playerId) != State.modUsers.end())
                State.modUsers.erase(playerId);

            if (auto evtPlayer = GetEventPlayer(playerInfo); evtPlayer) {
                synchronized(Replay::replayEventMutex) {
                    State.liveReplayEvents.emplace_back(std::make_unique<DisconnectEvent>(evtPlayer.value()));
                }
            }
        }
        else {
            //Found this happens on game ending occasionally
            //Log.Info(std::format("Client {} has left the game.", data->fields.Id));
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in AmongUsClient_OnPlayerLeft (InnerNetClient)");
    }
    AmongUsClient_OnPlayerLeft(__this, data, reason, method);
}

void dAmongUsClient_OnPlayerJoined(AmongUsClient* __this, ClientData* data, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dAmongUsClient_OnPlayerJoined executed");
    State.BlinkPlayersTab = true;
    AmongUsClient_OnPlayerJoined(__this, data, method);
}


bool bogusTransformSnap(PlayerSelection& _player, Vector2 newPosition)
{
    const auto& player = _player.validate();
    if (!player.has_value())
        Log.Debug("bogusTransformSnap received invalid player!");
    if (!player.has_value()) return false; //Error getting playercontroller
    //if (player.is_LocalPlayer()) return false;
    if (player.get_PlayerControl()->fields.inVent) return false; //Vent buttons are warps
    if (GameObject_get_layer(app::Component_get_gameObject((Component_1*)player.get_PlayerControl(), NULL), NULL) == LayerMask_NameToLayer(convert_to_string("Ghost"), NULL))
        return false; //For some reason the playercontroller is not marked dead at this point, so we check what layer the player is on
    auto currentPosition = PlayerControl_GetTruePosition(player.get_PlayerControl(), NULL);
    auto distanceToTarget = (int32_t)Vector2_Distance(currentPosition, newPosition, NULL); //rounding off as the smallest kill distance is zero
    std::vector<float> killDistances = { 1.0f, 1.8f, 2.5f }; //proper kill distance check
    auto killDistance = killDistances[std::clamp(GameOptions().GetInt(app::Int32OptionNames__Enum::KillDistance), 0, 2)];
    auto initialSpawnLocation = GetSpawnLocation(player.get_PlayerControl()->fields.PlayerId, (int)il2cpp::List((*Game::pGameData)->fields.AllPlayers).size(), true);
    auto meetingSpawnLocation = GetSpawnLocation(player.get_PlayerControl()->fields.PlayerId, (int)il2cpp::List((*Game::pGameData)->fields.AllPlayers).size(), false);
    if (Equals(initialSpawnLocation, newPosition)) return false;
    if (Equals(meetingSpawnLocation, newPosition)) return false;  //You are warped to your spawn at meetings and start of games
    //if (IsAirshipSpawnLocation(newPosition)) return false;
    if (PlayerIsImpostor(player.get_PlayerData()) && distanceToTarget <= killDistance)
        return false;
    std::ostringstream ss;

    ss << "From " << +currentPosition.x << "," << +currentPosition.y << " to " << +newPosition.x << "," << +newPosition.y << std::endl;
    ss << "Range to target " << +distanceToTarget << ", KillDistance: " << +killDistance << std::endl;
    ss << "Initial Spawn Location " << +initialSpawnLocation.x << "," << +initialSpawnLocation.y << std::endl;
    ss << "Meeting Spawn Location " << +meetingSpawnLocation.x << "," << +meetingSpawnLocation.y << std::endl;
    ss << "-------";
    Log.Debug(ss.str());
    return true; //We have ruled out all possible scenarios.  Off with his head!
}

void dCustomNetworkTransform_SnapTo(CustomNetworkTransform* __this, Vector2 position, uint16_t minSid, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dCustomNetworkTransform_SnapTo executed");
    /*try {//Leave this out until we fix it.
        if (!State.PanicMode) {
            if (!IsInGame()) {
                CustomNetworkTransform_SnapTo(__this, position, minSid, method);
                return;
            }

            for (auto p : GetAllPlayerControl()) {
                if (p->fields.NetTransform == __this) {
                    PlayerSelection pSel = PlayerSelection(p);
                    if (bogusTransformSnap(pSel, position))
                    {
                        synchronized(Replay::replayEventMutex) {
                            State.liveReplayEvents.emplace_back(std::make_unique<CheatDetectedEvent>(GetEventPlayer(GetPlayerData(p)).value(), CHEAT_ACTIONS::CHEAT_TELEPORT));
                        }
                    }
                    break;
                }
            }
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in CustomNetworkTransform_SnapTo (InnerNetClient)");
    }*/
    CustomNetworkTransform_SnapTo(__this, position, minSid, method);
}

void dAmongUsClient_OnGameEnd(AmongUsClient* __this, EndGameResult* endGameResult, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dAmongUsClient_OnGameEnd executed");
    try {
        if (*Game::pLocalPlayer != NULL && GetPlayerData(*Game::pLocalPlayer)->fields.RoleType == RoleTypes__Enum::Shapeshifter)
            RoleManager_SetRole(Game::RoleManager.GetInstance(), *Game::pLocalPlayer, RoleTypes__Enum::Impostor, NULL);
        //fixes game crashing on ending with shapeshifter
        bool impostorWin = false;
        auto reason = endGameResult->fields.GameOverReason;
        switch (reason) {
        case GameOverReason__Enum::HideAndSeek_ImpostorsByKills:
        case GameOverReason__Enum::ImpostorsByKill:
        case GameOverReason__Enum::ImpostorsBySabotage:
        case GameOverReason__Enum::ImpostorsByVote:
        case GameOverReason__Enum::CrewmateDisconnect:
            impostorWin = true;
            break;
        }
        std::string winnersText = "Game Winners: ";
        int count = 0;
        for (auto p : GetAllPlayerData()) {
            if (IsHost() && !State.PanicMode && State.TournamentMode) {
                if (p == NULL) continue;
                auto friendCode = convert_from_string(p->fields.FriendCode);
                if (impostorWin) {
                    if (State.tournamentAliveImpostors == State.tournamentAssignedImpostors && PlayerIsImpostor(p)) {
                        State.tournamentPoints[friendCode] += 2; //AllImpsWin
                        LOG_DEBUG(std::format("Added 2 points to {} for all impostors win", ToString(p)).c_str());
                        State.tournamentWinPoints[friendCode] += 1;
                    }
                    else if (PlayerIsImpostor(p)) {
                        if (State.tournamentAliveImpostors.size() == 1 && !p->fields.IsDead) {
                            State.tournamentPoints[friendCode] += 2; //ImpWin
                            LOG_DEBUG(std::format("Added 2 points to {} for solo win", ToString(p)).c_str());
                            State.tournamentWinPoints[friendCode] += 2;
                        }
                        else {
                            State.tournamentPoints[friendCode] += 1; //ImpWin
                            LOG_DEBUG(std::format("Added 1 point to {} for impostor win", ToString(p)).c_str());
                            State.tournamentWinPoints[friendCode] += 1;
                        }
                    }
                }
                else {
                    if (PlayerIsImpostor(p)) {
                        State.tournamentPoints[friendCode] -= 1; //ImpLose
                        LOG_DEBUG(std::format("Deducted -1 point from {} for impostor loss", ToString(p)).c_str());
                    }
                    else {
                        State.tournamentPoints[friendCode] += 2; //CrewWin
                        LOG_DEBUG(std::format("Added 2 points to {} for crewmate win", ToString(p)).c_str());
                        State.tournamentWinPoints[friendCode] += 1;
                    }
                }
            }
            auto name = convert_from_string(GetPlayerOutfit(p)->fields.PlayerName);
            if ((impostorWin && PlayerIsImpostor(p)) || (!impostorWin && !PlayerIsImpostor(p))) {
                winnersText += name + ", ";
                count++;
            }
        }
        if (count == 0) LOG_DEBUG("No one was a winner in the game.");
        else LOG_DEBUG(winnersText.substr(0, (size_t)winnersText.size() - 2));
        onGameEnd();
    }
    catch (...) {
        LOG_ERROR("Exception occurred in AmongUsClient_OnGameEnd (InnerNetClient)");
    }
    AmongUsClient_OnGameEnd(__this, endGameResult, method);
}

void dInnerNetClient_DisconnectInternal(InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dInnerNetClient_DisconnectInternal executed");
    try {
        // IsInGame() || IsInLobby()
        if (__this->fields.GameState == InnerNetClient_GameStates__Enum::Started
            || __this->fields.GameState == InnerNetClient_GameStates__Enum::Joined
            || __this->fields.NetworkMode == NetworkModes__Enum::FreePlay) {
            onGameEnd();
            State.LastDisconnectReason = reason;
            if (reason == DisconnectReasons__Enum::Banned || reason == DisconnectReasons__Enum::ConnectionLimit || reason == DisconnectReasons__Enum::GameNotFound || reason == DisconnectReasons__Enum::ServerError)
                State.AutoJoinLobby = false;
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in InnerNetClient_DisconnectInternal (InnerNetClient)");
    }
    InnerNetClient_DisconnectInternal(__this, reason, stringReason, method);
}

void dInnerNetClient_EnqueueDisconnect(InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dInnerNetClient_EnqueueDisconnect executed");
    try {
        State.FollowerCam = nullptr;
        onGameEnd(); //removed antiban cuz it glitches the game
    }
    catch (...) {
        LOG_ERROR("Exception occurred in InnerNetClient_EnqueueDisconnect (InnerNetClient)");
    }
    return InnerNetClient_EnqueueDisconnect(__this, reason, stringReason, method);
}

void dGameManager_RpcEndGame(GameManager* __this, GameOverReason__Enum endReason, bool showAd, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dGameManager_RpcEndGame executed");
    if (!State.PanicMode && IsHost() && State.NoGameEnd)
        return;
    GameManager_RpcEndGame(__this, endReason, showAd, method);
}

void dKillOverlay_ShowKillAnimation_1(KillOverlay* __this, NetworkedPlayerInfo* killer, NetworkedPlayerInfo* victim, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dKillOverlay_ShowKillAnimation_1 executed");
    try {
        if (!State.PanicMode && State.DisableKillAnimation)
            return;
    }
    catch (...) {
        Log.Debug("Exception occurred in KillOverlay_ShowKillAnimation_1 (InnerNetClient)");
    }
    return KillOverlay_ShowKillAnimation_1(__this, killer, victim, method);
}

float dLogicOptions_GetKillDistance(LogicOptions* __this, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dLogicOptions_GetKillDistance executed");
    try {
        if (!State.PanicMode) {
            State.GameKillDistance = LogicOptions_GetKillDistance(__this, method);
            if (State.InfiniteKillRange)
                return FLT_MAX;
            if (State.ModifyKillDistance)
                return State.KillDistance;
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in LogicOptions_GetKillDistance (InnerNetClient)");
    }
    return LogicOptions_GetKillDistance(__this, method);
}

void dLadder_SetDestinationCooldown(Ladder* __this, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dLadder_SetDestinationCooldown executed");
    try {
        if (!State.PanicMode && State.NoAbilityCD) {
            __this->fields._CoolDown_k__BackingField = 0.f;
            return;
        }
    }
    catch (...) {
        Log.Debug("Exception occurred in Ladder_SetDestinationCooldown (InnerNetClient)");
    }
    return Ladder_SetDestinationCooldown(__this, method);
}

void dZiplineConsole_SetDestinationCooldown(ZiplineConsole* __this, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dZiplineConsole_SetDestinationCooldown executed");
    try {
        if (!State.PanicMode && State.NoAbilityCD) {
            __this->fields._CoolDown_k__BackingField = 0.f;
            return;
        }
    }
    catch (...) {
        Log.Debug("Exception occurred in ZiplineConsole_SetDestinationCooldown (InnerNetClient)");
    }
    return ZiplineConsole_SetDestinationCooldown(__this, method);
}

void dVoteBanSystem_AddVote(VoteBanSystem* __this, int32_t srcClient, int32_t clientId, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dVoteBanSystem_AddVote executed");
    try {
        if (clientId == (*Game::pLocalPlayer)->fields._.OwnerId)
            State.VoteKicks++;
        PlayerControl* sourcePlayer = *Game::pLocalPlayer;
        PlayerControl* affectedPlayer = *Game::pLocalPlayer;
        for (auto p : GetAllPlayerControl()) {
            if (p->fields._.OwnerId == srcClient) sourcePlayer = p;
            if (p->fields._.OwnerId == clientId) affectedPlayer = p;
        }
        if (IsHost()) {
            if (affectedPlayer == *Game::pLocalPlayer) return; //anti kick as host
            if (sourcePlayer == *Game::pLocalPlayer) {
                InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), clientId, false, NULL);
                return;
            }
            if (State.DisableAllVotekicks) return;
        }
        std::string sourceplayerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(sourcePlayer), nullptr));
        std::string affectedplayerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(affectedPlayer), nullptr));
        LOG_DEBUG(sourceplayerName + " attempted to votekick " + affectedplayerName);
    }
    catch (...) {
        LOG_ERROR("Exception occurred in VoteBanSystem_AddVote (InnerNetClient)");
    }
    return VoteBanSystem_AddVote(__this, srcClient, clientId, method);
}

/*void* dAmongUsClient_CoStartGameHost(AmongUsClient* __this, MethodInfo* method) {
    //this might flip the skeld
    return AmongUsClient_CoStartGameHost(__this, method);
}*/

void dDisconnectPopup_DoShow(DisconnectPopup* __this, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dDisconnectPopup_DoShow executed");
    DisconnectPopup_DoShow(__this, method);
    if (!State.PanicMode) {
        switch (State.LastDisconnectReason) {
        case DisconnectReasons__Enum::Hacking: {
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("You were banned for hacking.\n\n{}{}",
                    State.AutoCopyLobbyCode ? "Lobby Code has been copied to the clipboard." : "Please stop.",
                    State.SafeMode ? "" : "\n\nDisabling safe mode isn't recommended on official servers!")), NULL);
        }
        break;
        case DisconnectReasons__Enum::Kicked: {
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("You were kicked from the lobby.\n\n{}",
                    State.AutoCopyLobbyCode ? "Lobby Code has been copied to the clipboard." : "You can rejoin the lobby if it hasn't started.")), NULL);
        }
        break;
        case DisconnectReasons__Enum::Banned: {
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("You were banned from the lobby.\n\n{}",
                    State.AutoCopyLobbyCode ? "Lobby Code has been copied to the clipboard." : "You can rejoin the lobby by changing your IP address.")), NULL);
        }
        break;
        default: {
            std::string prevText = convert_from_string(TMP_Text_get_text((TMP_Text*)__this->fields._textArea, NULL));
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("{}{}", prevText,
                    State.AutoCopyLobbyCode ? "\nLobby Code has been copied to the clipboard." : "")), NULL);
        }
        break;
        }
        if (State.AutoCopyLobbyCode) ClipboardHelper_PutClipboardString(convert_to_string(State.LastLobbyJoined), NULL);
    }
}

bool dGameManager_DidImpostorsWin(GameManager* __this, GameOverReason__Enum reason, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dGameManager_DidImpostorsWin executed");
    return GameManager_DidImpostorsWin(__this, reason, method);
}
