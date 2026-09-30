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
#include "toasts.hpp"
#include "console.hpp"
#include <chrono>
#include "achievements.hpp"

using namespace std::string_view_literals;

static bool autoStartedGame = false;
extern bool editingAutoStartPlayerCount;

static std::string strToLower(std::string str) {
    std::string new_str = "";
    for (auto i : str) {
        new_str += char(std::tolower(i));
    }
    return new_str;
}

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

const ptrdiff_t GetRoleCount(RoleType role, bool excludeSelf = false)
{
    if (excludeSelf) {
        std::array<RoleType, Game::MAX_PLAYERS> assignedRolesCopy = {};
        std::copy(std::begin(State.assignedRoles), std::end(State.assignedRoles), std::begin(assignedRolesCopy));
        if (*Game::pLocalPlayer != NULL) assignedRolesCopy[(*Game::pLocalPlayer)->fields.PlayerId] = RoleType::Random;
        return std::count_if(assignedRolesCopy.cbegin(), assignedRolesCopy.cend(), [role](RoleType i) {return i == role; });
    }
    return std::count_if(State.assignedRoles.cbegin(), State.assignedRoles.cend(), [role](RoleType i) {return i == role; });
}

static void onGameEnd() {
    try {
        LOG_DEBUG("Reset All");
        Replay::Reset();
        State.liveReplayEvents.clear();
        State.modUsers.clear();
        State.activeImpersonation = false;
        State.FollowerCam = nullptr;
        State.shadowCollab = nullptr;
        //State.EnableZoom = false;
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
        State.validDeadBodyIds.clear();
        State.ventTpSeqIds.clear();
        State.SpamVentTpEveryoneRandom = false;
        State.SpamVentTpEveryone = false;
        State.spamRandomVentTpPlayers.clear();
        State.spamZiplinePlayers.clear();
        State.spamVentTpPlayers.clear();
        State.VoteKicks = 0;
        State.OutfitCooldown = GetFps();
        State.CanChangeOutfit = false;
        State.GameLoaded = false;
        State.RealRole = RoleTypes__Enum::Crewmate;
        State.mapType = Settings::MapType::Ship;
        State.SpeedrunTimer = 0.f;
        State.GameModeDurationTimer = 0.f;
        State.GameModeDurationOver = false;
        autoStartedGame = false;
        State.ChatFocused = false;
        State.MIG_ThemeChanged = true;
        State.DisableHud = false;
        State.ControlPet = false;
        State.petPos = { NULL, NULL };
        State.playerToAttach = {};
        State.DisableControlPetHand = false;
        State.ChatSpamMode = 0;

        State.VoteOffPlayerId = Game::HasNotVoted;

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
        State.checkedPlayerIds.clear();
        State.SpeedrunOver = false;
        State.JoinedLobby = false;
        State.SpamZiplineEveryone = false;
        State.AntiExploit_IsTeleportingSelf = false;
        State.AntiExploit_IsClimbingZipline = false;

        State.ColorCycledPlayers.clear();
        State.VoteImmunePlayers.clear();

        drawing_t& instance = Esp::GetDrawing();
        synchronized(instance.m_DrawingMutex) {
            instance.m_Players = {};
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in onGameEnd (InnerNetClient)");
    }
}

void dInnerNetClient_Update(InnerNetClient* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dInnerNetClient_Update executed", false);
    try {
        if (State.unlockAllAchievements) {
            Achievements::UnlockAll();
            State.unlockAllAchievements = false;
        }

        if (!State.PanicMode) {
            if (IsHost() && !State.Mod_PendingRulesMessages.empty()) {
                State.Mod_PendingRulesDelay -= Time_get_deltaTime(NULL);
                if (State.Mod_PendingRulesDelay <= 0.f) {
                    std::string nextRulesMsg = State.Mod_PendingRulesMessages.front();
                    State.Mod_PendingRulesMessages.pop();
                    PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(nextRulesMsg), NULL);
                    State.Mod_PendingRulesDelay = 2.0f;
                }
            }
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

            if (!State.PanicMode && State.ModDetection && (IsInLobby()/* || State.BroadcastedMod == 1*/)) {
                uint8_t rpcCall = (uint8_t)420;
                /*switch (State.BroadcastedMod) {
                case 1:
                    rpcCall = (uint8_t)42069;
                    break;
                case 2:
                    rpcCall = (uint8_t)250;
                    break;
                }*/
                if (State.rpcCooldown <= 0) {
                    //SickoMenu users can detect this rpc
                    MessageWriter* writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId, rpcCall, SendOption__Enum::Reliable, -1, NULL);
                    MessageWriter_WriteString(writer, convert_to_string(State.SickoVersion), NULL);
                    InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
                    State.rpcCooldown = int(0.5 * GetFps());
                }
                else {
                    State.rpcCooldown--;
                }
            }

            if (!IsInGame()) {
                State.InMeeting = false;
                State.DisableLights = false;
                State.DisableLightSwitches = false;
                State.DisableComms = false;
                State.DisableReactor = false;
                State.DisableOxygen = false;
                State.InfiniteMushroomMixup = false;
                State.AutoRepairSabotage = false;
                State.CloseAllDoors = false;
                State.SpamReport = false;
                State.DisableVents = false;
                State.SpamMovingPlatform = false;

                if (!IsInLobby()) {
                    State.selectedPlayers = {};
                    //State.EnableZoom = false; //intended as we don't want stuff like the taskbar and danger meter disappearing on game start
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

                if (IsHost() && State.GameLoaded && State.GameMode != 0 && !State.GameModeDurationOver) {
                    State.GameModeDurationTimer += Time_get_deltaTime(NULL);
                    if (State.GameModeDurationTimer >= (float)State.GameModeDuration) {
                        State.GameModeDurationOver = true; 
                        GameManager_RpcEndGame(GameManager__TypeInfo->static_fields->_Instance_k__BackingField, GameOverReason__Enum::CrewmatesByTask, false, NULL);
                    }
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
                auto outfit = GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer));
                if ((IsColorAvailable(State.SelectedColorId) || !State.SafeMode) && outfit != NULL &&
                    outfit->fields.ColorId != State.SelectedColorId) {
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
                        State.originalPet = outfit->fields.PetId;
                        State.originalSkin = outfit->fields.SkinId;
                        State.originalHat = outfit->fields.HatId;
                        State.originalVisor = outfit->fields.VisorId;
                        State.originalNamePlate = outfit->fields.NamePlateId;
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

            if ((IsInGame() || IsInLobby()) && GameOptions().GetGameMode() == GameModes__Enum::Normal && GetPlayerData(*Game::pLocalPlayer) != NULL) {
                if ((IsHost() || !State.SafeMode) && State.Impostor_NoKillCooldown) {
                    if (GameLogicOptions().GetKillCooldown() > 0)
                        (*Game::pLocalPlayer)->fields.killTimer = 0.f;
                    else
                        GameLogicOptions().SetFloat(app::FloatOptionNames__Enum::KillCooldown, 0.0042069f); //force cooldown > 0 as ur unable to kill otherwise
                }
                if (IsInGame() && State.InfiniteMeetings) {
                    (*Game::pLocalPlayer)->fields.RemainingEmergencies = 69420;
                    //if (GameOptions().HasOptions())
                        //(*Game::pShipStatus)->fields.EmergencyCooldown = (float)GameOptions().GetInt(app::Int32OptionNames__Enum::EmergencyCooldown);
                }
            }
            if ((IsInGame() || IsInLobby()) && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek && State.Impostor_NoKillCooldown) {
                auto localData = GetPlayerData(*Game::pLocalPlayer);
                app::RoleBehaviour* playerRole = localData->fields.Role;
                app::RoleTypes__Enum role = playerRole != nullptr ? (playerRole)->fields.Role : app::RoleTypes__Enum::Crewmate;
                if (IsHost() || !State.SafeMode) (*Game::pLocalPlayer)->fields.killTimer = 0;
            }

            static int weaponsDelay = 0;

            if (weaponsDelay <= 0 && IsInGame()) {
                if (State.PlayWeaponsAnimation == true) {
                    State.rpcQueue.push(new RpcPlayAnimation(6));
                    weaponsDelay = GetFps(); //Should be approximately 1 second
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
            }

            if (State.CycleDuration <= 10) {
                State.CycleDuration = 10;
            }

            // Resolve missing host name when joining through a code / invite using GetHostUsername which is known to work
            if (!State.LobbyHistory.empty() && State.LobbyHistory.front().HostName.empty() && IsInLobby()) {
                std::string host = GetHostUsername();
                if (!host.empty())
                    State.LobbyHistory.front().HostName = RemoveHtmlTags(host);
            }

            if (State.VotekickRejoinPending) {
                State.VotekickRejoinDelay -= Time_get_deltaTime(NULL);
                if (State.VotekickRejoinDelay <= 0.f) {
                    State.VotekickRejoinPending = false;
                    State.JoinLobbyCode = State.VotekickRejoinLobbyCode;
                    State.JoinLobby = true;
                }
            }

            // static int joinDelay = 0;
            // if (joinDelay > 0) joinDelay--;
            if (State.JoinLobby/* && joinDelay <= 0*/) {
                AmongUsClient_ExitGame(*Game::pAmongUsClient, DisconnectReasons__Enum::ExitGame, NULL);
                void* routine = AmongUsClient_CoFindGameInfoFromCodeAndJoin(*Game::pAmongUsClient,
                    GameCode_GameNameToInt(convert_to_string(State.JoinLobbyCode), NULL),
                    NULL);
                if (routine != NULL)
                    MonoBehaviour_StartCoroutine((MonoBehaviour*)*Game::pAmongUsClient, routine, NULL);
                State.JoinLobby = false;
                // joinDelay = 100;
            }

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

            if (State.CrashSpamReport && IsInGame() && !State.GameLoaded) {
                State.rpcQueue.push(new RpcReportBody({}));
            }

            static int nameChangeCycleDelay = 0; //If we spam too many name changes, we're banned
            if (nameChangeCycleDelay <= 0 && State.SetName && !State.activeImpersonation && !State.ServerSideCustomName && !State.SafeMode) {
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
            else if (!State.SafeMode) {
                nameChangeCycleDelay--;
            }

            static int nameCtr = 1;

            static int cycleNameDelay = 0; //If we spam too many name changes, we're banned
            static int colorChangeCycleDelay = 0; //If we spam too many color changes, we're banned?
            static int changeCycleDelay = 0; //controls the actual cosmetic cycler

            if (State.Cycler)
                State.CycleBetweenPlayers = false;
            if (State.CycleBetweenPlayers)
                State.Cycler = false;

            if ((!State.InMeeting || State.CycleInMeeting) && State.CanChangeOutfit) {
                if (State.Cycler && !State.SafeMode && State.CycleName && cycleNameDelay <= 0) {
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
                else if (cycleNameDelay > 0) cycleNameDelay--;

                if (colorChangeCycleDelay <= 0 && ((State.Cycler && State.RandomColor) || State.ColorCycledPlayers.size() != 0)) {
                    if ((IsHost() || !State.SafeMode) && (State.CycleForEveryone || State.ColorCycledPlayers.size() != 0)) {
                        for (auto p : GetAllPlayerControl()) {
                            bool isColorCycling = std::find(State.ColorCycledPlayers.begin(), State.ColorCycledPlayers.end(), p->fields.PlayerId) != State.ColorCycledPlayers.end();
                            if (!State.CycleForEveryone && !isColorCycling &&
                                !(State.Cycler && State.RandomColor && p == *Game::pLocalPlayer)) continue;
                            if (p == *Game::pLocalPlayer && State.activeImpersonation) continue;

                            PlayerControl_RpcSetColor(p, GetRandomColorId(), NULL);
                        }
                    }
                    else PlayerControl_CmdCheckColor(*Game::pLocalPlayer, GetRandomColorId(), NULL);
                    colorChangeCycleDelay = int(State.CycleTimer * GetFps()); //idk how long this is
                }
                else if (colorChangeCycleDelay > 0) colorChangeCycleDelay--;

                if (State.Cycler && changeCycleDelay <= 0 && !State.activeImpersonation && (State.RandomHat || State.RandomSkin || State.RandomVisor || State.RandomPet || State.RandomNamePlate)) {
                    if (State.RandomHat) {
                        std::vector availableHats = { "hat_NoHat", "hat_bday_guard", "hat_cosmic_alienAntenna", "hat_cosmic_hood", "hat_cosmic_lure", "hat_cosmic_cosmonaut", "hat_cosmic_meteor", "hat_cosmic_moon", "hat_cosmic_rings", "hat_cosmic_crash", "hat_cosmic_rocket", "hat_cosmic_satellite", "hat_cosmic_starAntenna", "hat_cosmic_star", "hat_cosmic_sun", "hat_cosmic_telescope", "hat_parasite_Blue", "hat_parasite_Cyan", "hat_parasite_Green", "hat_parasite_Lime", "hat_parasite_Purple", "hat_parasite_Red", "hat_parasite_Cook", "hat_hanami_blossom", "hat_hanami_pigtails", "hat_hanami_dango", "hat_hanami_crown", "hat_hanami_hachimaki", "hat_hanami_matcha", "hat_hanami_mochi", "hat_phoenix_wright", "hat_2026nye", "hat_stardew_abigail", "hat_stardew_grandpa", "hat_stardew_lewis", "hat_stardew_linus", "hat_stardew_mrqi", "hat_stardew_sebastian", "hat_stardew_straw", "hat_stardew_shorts", "hat_stardew_melon", "hat_stardew_parsnip", "hat_stardew_chickenWhite", "hat_stardew_chickenBlue", "hat_stardew_chickenVoid", "hat_stardew_egg", "hat_Paimon", "hat_racing_fungle", "hat_racing_skeld", "hat_racing_mira", "hat_racing_polus", "hat_racing_airship", "hat_racing_bald", "hat_bsb2_watermelon", "hat_bsb2_beretBlack", "hat_bsb2_beretBlue", "hat_bsb2_beretPink", "hat_bsb2_bowPink", "hat_bsb2_bowRed", "hat_bday_cake", "hat_kamurocho_cinderella", "hat_kamurocho_ichiban", "hat_kamurocho_kazuma", "hat_kamurocho_majima", "hat_kamurocho_helmet", "hat_kamurocho_pirate", "hat_kamurocho_ono", "hat_NewYear2025", "hat_paws_panda", "hat_claws_spaceDog", "hat_claws_frog", "hat_paws_spaceDog", "hat_paws_fish", "hat_claws_moose", "hat_claws_dragonRed", "hat_paws_raccoon", "hat_claws_bullHorns", "hat_paws_turtle", "hat_paws_foxGrey", "hat_claws_foxOrange", "hat_paws_antlers", "hat_claws_squid", "hat_claws_kuduHorns", "hat_paws_opossum", "hat_claws_buffaloHorns", "hat_claws_hippo", "hat_Edgeworth", "hat_artagan", "hat_chetney", "hat_fcg", "hat_fearne", "hat_jester", "hat_laudna", "hat_molly", "hat_nott", "hat_orthax", "hat_scanlan", "hat_sprinkle", "hat_vax", "hat_fluffyHat", "hat_topHatMinimate", "hat_triplePartyHat", "hat_bb1_bucketHatBee", "hat_bb1_bucketHatBlack", "hat_bb1_bucketHatCamo", "hat_bb1_bucketHatFlowers", "hat_bb1_bucketHatWhite", "hat_bb1_bunnyBlack", "hat_bb1_sunHatGreen", "hat_bb1_sunHatYellow", "hat_bb1_lilShroomRed", "hat_bb1_lilShroomBlue", "hat_bb1_lilShroomGlowing", "hat_lny_dancerTail", "hat_lny_dancerBody", "hat_lny_dancerHead", "hat_lny_lantern", "hat_lny_soupSpoon", "hat_lny_cat", "hat_lny_dog", "hat_lny_dragon", "hat_lny_goat", "hat_lny_horse", "hat_lny_monkey", "hat_lny_ox", "hat_lny_pig", "hat_lny_rabbit", "hat_lny_rat", "hat_lny_rooster", "hat_lny_tiger", "hat_NewYear2024", "hat_bowkid", "hat_conductor", "hat_hatkid", "hat_mustachegirl", "hat_alienHominid", "hat_castleCrasher", "hat_hattyHattington", "hat_king", "hat_badeline", "hat_bird", "hat_madeline", "hat_theohair", "hat_cadence", "hat_nocturna", "hat_shopkeeper", "hat_skull", "hat_boneUndertale", "hat_duck", "hat_floweyBad", "hat_floweyGood", "hat_spear", "hat_undyne", "hat_bell", "hat_gardener", "hat_goosefloaty", "hat_mushmuffsHat", "hat_shiitakeHat", "hat_shrapnelHat", "hat_anchor", "hat_antenna", "hat_beachball", "hat_bucket", "hat_bucketHat", "hat_fishingHat", "hat_fungleFlower", "hat_killerplant", "hat_lilShroom", "hat_mushbuns", "hat_mushroomBeret", "hat_mysteryBones", "hat_pickaxe", "hat_sharkfin", "hat_shovel", "hat_bearyCold", "hat_pusheenGreyHat", "hat_pusheenMintHat", "hat_pusheenPinkHat", "hat_pusheenPurpleHat", "hat_PusheenicornHat", "hat_pusheenSitHat", "hat_pusheenSleepHat", "hat_SlothHat", "hat_starBalloon", "hat_bandanaWBY", "hat_fishCap", "hat_rabbitEars", "hat_caiatl", "hat_chalice", "hat_erisMorn", "hat_hunter", "hat_maraSov", "hat_osiris", "hat_pyramid", "hat_saint14", "hat_savathun", "hat_shaxx", "hat_starhorse", "hat_titan", "hat_warlock", "hat_NewYear2023", "hat_Igloo", "hat_Present", "hat_Scrudge", "hat_Snowman", "hat_StarTopper", "hat_babybean", "hat_cashHat", "hat_crownBean", "hat_crownDouble", "hat_crownTall", "hat_mareLwyd", "hat_schnapp", "hat_Sorry", "hat_tinFoil", "hat_wigJudge", "hat_wigTall", "hat_hl_fubuki", "hat_hl_gura", "hat_hl_korone", "hat_hl_marine", "hat_hl_mio", "hat_hl_moona", "hat_hl_okayu", "hat_hl_pekora", "hat_hl_risu", "hat_hl_watson", "hat_Baguette", "hat_BreadLoaf", "hat_Butter", "hat_OrangeHat", "hat_PancakeStack", "hat_Pineapple", "hat_PizzaSliceHat", "hat_StrawberryLeavesHat", "hat_ToastButterHat", "hat_croissant", "hat_IceCreamVanilla", "hat_IceCreamUbe", "hat_IceCreamMatcha", "hat_IceCreamMint", "hat_IceCreamNeo", "hat_IceCreamStrawberry", "hat_sausage", "hat_screamghostface", "hat_halospartan", "hat_ratchet", "hat_w21_gingerbread", "hat_w21_holly", "hat_w21_krampus", "hat_w21_log", "hat_w21_mistletoe", "hat_w21_mittens", "hat_w21_nutcracker", "hat_w21_pinecone", "hat_w21_snowflake", "hat_w21_snowman", "hat_w21_winterpuff", "hat_caitlin", "hat_clagger", "hat_comper", "hat_enforcer", "hat_heim", "hat_jayce", "hat_jinx", "hat_vi", "hat_arrowhead", "hat_axe", "hat_bone", "hat_candycorn", "hat_clown_purple", "hat_fairywings", "hat_fishhed", "hat_frankenbolts", "hat_frankenbride", "hat_glowstick", "hat_glowstickCyan", "hat_glowstickOrange", "hat_glowstickPink", "hat_glowstickPurple", "hat_glowstickYellow", "hat_tombstone", "hat_mummy", "hat_Basketball", "hat_Bowlingball", "hat_Dodgeball", "hat_Voleyball", "hat_Soccer", "hat_Deitied", "hat_DrillMetal", "hat_DrillStone", "hat_DrillWood", "hat_Janitor", "hat_Pot", "hat_CuppaJoe", "hat_HardtopHat", "hat_Prototype", "hat_Records", "hat_Rupert", "hat_ThomasC", "hat_ToppatHair", "hat_WilfordIV", "hat_Winston", "hat_pk05_Burthat", "hat_pk05_cheesetoppat", "hat_pk05_davehat", "hat_pk05_Ellie", "hat_pk05_Ellryhat", "hat_pk05_GeoffreyToppat", "hat_pk05_HenryToppat", "hat_pk05_EllieToppat", "hat_pk05_Macbethhat", "hat_pk05_RHM", "hat_pk05_Svenhat", "hat_mira_bush", "hat_mira_case", "hat_mira_cloud", "hat_mira_flower", "hat_mira_flower_red", "hat_mira_gem", "hat_mira_leaf", "hat_mira_milk", "hat_pk03_Headphones", "hat_GovtHeadset", "hat_mira_headset_blue", "hat_mira_headset_pink", "hat_mira_headset_yellow", "hat_pk03_Security1", "hat_AbominalHat", "hat_EarmuffGreen", "hat_EarmuffsPink", "hat_EarmuffsYellow", "hat_EarnmuffBlue", "hat_RockLava", "hat_RockIce", "hat_SnowbeanieRed", "hat_SnowBeaniePurple", "hat_SnowbeanieGreen", "hat_SnowbeanieOrange", "hat_WinterHelmet", "hat_pk04_Archae", "hat_pk04_MinerCap", "hat_MinerYellow", "hat_MinerBlack", "hat_pk04_WinterHat", "hat_WinterYellow", "hat_WinterRed", "hat_WinterGreen", "hat_pkHW01_BatWings", "hat_bat_crewcolor", "hat_bat_green", "hat_bat_ice", "hat_pkHW01_CatEyes", "hat_cat_grey", "hat_cat_orange", "hat_cat_pink", "hat_cat_snow", "hat_pkHW01_Horns", "hat_devilhorns_yellow", "hat_devilhorns_black", "hat_devilhorns_crewcolor", "hat_devilhorns_green", "hat_devilhorns_murky", "hat_devilhorns_white", "hat_pkHW01_Machete", "hat_pkHW01_Mohawk", "hat_mohawk_rainbow", "hat_mohawk_bubblegum", "hat_mohawk_bumblebee", "hat_mohawk_purple_green", "hat_pkHW01_Pirate", "hat_pkHW01_PlagueHat", "hat_pkHW01_Pumpkin", "hat_pkHW01_ScaryBag", "hat_papermask", "hat_pkHW01_Witch", "hat_witch_white", "hat_witch_green", "hat_witch_murky", "hat_witch_pink", "hat_pkHW01_Wolf", "hat_wolf_grey", "hat_wolf_murky", "hat_pk06_Candycanes", "hat_w21_candycane_mint", "hat_w21_candycane_blue", "hat_w21_candycane_bubble", "hat_w21_candycane_chocolate", "hat_pk06_ElfHat", "hat_w21_elf_swe", "hat_w21_elf_pink", "hat_pk06_Lights", "hat_w21_lights_white", "hat_w21_lights_yellow", "hat_pk06_Present", "hat_w21_present_whiteblue", "hat_w21_present_evil", "hat_w21_present_greenyellow", "hat_w21_present_redwhite", "hat_pk06_Reindeer", "hat_pk06_Santa", "hat_w21_santa_yellow", "hat_w21_santa_evil", "hat_w21_santa_green", "hat_w21_santa_mint", "hat_w21_santa_pink", "hat_w21_santa_white", "hat_pk06_Snowman", "hat_w21_snowman_swe", "hat_w21_snowman_evil", "hat_w21_snowman_greenred", "hat_w21_snowman_redgreen", "hat_pk06_tree", "hat_astronaut", "hat_Astronaut-Blue", "hat_Astronaut-Cyan", "hat_Astronaut-Orange", "hat_brainslug", "hat_headslug_White", "hat_headslug_Yellow", "hat_headslug_Red", "hat_headslug_Purple", "hat_bushhat", "hat_Chocolate", "hat_chocolateVanillaStrawb", "hat_chocolateCandy", "hat_chocolateMatcha", "hat_doubletophat", "hat_flowerpot", "hat_goggles", "hat_Goggles_Chrome", "hat_Goggles_Black", "hat_hardhat", "hat_Hardhat_White", "hat_Hardhat_black", "hat_Hardhat_Blue", "hat_Hardhat_Green", "hat_Hardhat_Orange", "hat_Hardhat_Pink", "hat_Hardhat_Purple", "hat_Hardhat_Red", "hat_Heart", "hat_military", "hat_MilitaryWinter", "hat_GovtDesert", "hat_police", "hat_paperhat", "hat_Paperhat_Pink", "hat_Paperhat_Yellow", "hat_Paperhat_Lightblue", "hat_Paperhat_Cyan", "hat_Paperhat_Blue", "hat_Paperhat_Black", "hat_partyhat", "hats_newyears2018", "hat_pk01_BaseballCap", "hat_baseball_White", "hat_baseball_Yellow", "hat_baseball_Red", "hat_baseball_Purple", "hat_baseball_Pink", "hat_baseball_Orange", "hat_baseball_Lilac", "hat_baseball_LightGreen", "hat_baseball_Lightblue", "hat_baseball_Green", "hat_baseball_Black", "hat_pk02_Crown", "hat_pk02_Eyebrows", "hat_pk02_HaloHat", "hat_pk02_HeroCap", "hat_Herohood_Yellow", "hat_Herohood_Black", "hat_Herohood_Blue", "hat_Herohood_Pink", "hat_Herohood_Purple", "hat_Herohood_Red", "hat_pk05_Wizardhat", "hat_pk02_PipCap", "hat_pk02_PlungerHat", "hat_Plunger_Blue", "hat_Plunger_Yellow", "hat_pk02_ScubaHat", "hat_pk02_StickminHat", "hat_pk02_StrawHat", "hat_pk02_TenGallonHat", "hat_TenGallon_Black", "hat_TenGallon_White", "hat_pk02_ThirdEyeHat", "hat_pk02_ToiletPaperHat", "hat_pk02_Toppat", "hat_pk03_Fedora", "hat_pk03_Goggles", "hat_pk03_StrapHat", "hat_pk03_Traffic", "hat_traffic_purple", "hat_Traffic_Blue", "hat_Traffic_Red", "hat_Traffic_Yellow", "hat_pk04_Antenna", "hat_Antenna_Black", "hat_pk04_Balloon", "hat_pk04_Banana", "hat_BananaGreen", "hat_BananaPurple", "hat_pk04_Bandana", "hat_Bandana_White", "hat_Bandana_Yellow", "hat_Bandana_Red", "hat_Bandana_Pink", "hat_Bandana_Green", "hat_Bandana_Blue", "hat_pk04_Beanie", "hat_Beanie_Black", "hat_Beanie_Blue", "hat_Beanie_Green", "hat_Beanie_Lightblue", "hat_Beanie_LightGreen", "hat_Beanie_LightPurple", "hat_Beanie_Pink", "hat_Beanie_Purple", "hat_Beanie_White", "hat_Beanie_Yellow", "hat_pk04_Bear", "hat_pk04_BirdNest", "hat_pk04_Chef", "hat_ChefWhiteBlue", "hat_pk04_DoRag", "hat_Dorag_Yellow", "hat_Dorag_Black", "hat_Dorag_Desert", "hat_Dorag_Jungle", "hat_Dorag_Purple", "hat_Dorag_Sky", "hat_Dorag_Snow", "hat_pk04_Fez", "hat_pk04_GeneralHat", "hat_captain", "hat_pk04_HunterCap", "hat_pk04_JungleHat", "hat_pk04_MiniCrewmate", "hat_pk04_Pompadour", "hat_pk04_RamHorns", "hat_Ramhorn_Black", "hat_Ramhorn_Red", "hat_Ramhorn_White", "hat_pk04_Slippery", "hat_mira_sign_blue", "hat_pk04_Snowman", "hat_pk04_Vagabond", "hat_pk05_Cheese", "hat_cheeseSwiss", "hat_cheeseBleu", "hat_cheeseMoldy", "hat_pk05_Cherry", "hat_cherryPink", "hat_cherryOrange", "hat_pk05_Egg", "hat_eggYellow", "hat_eggGreen", "hat_pk05_Fedora", "hat_pk05_Flamingo", "hat_pk05_FlowerPin", "hat_pk05_Helmet", "hat_pk05_Plant", "hat_Ponytail", "hat_Rubberglove", "hat_russian", "hat_stethescope", "hat_Doc_White", "hat_Doc_black", "hat_Doc_Orange", "hat_Doc_Purple", "hat_Doc_Red", "hat_tophat", "hat_towelwizard", "hat_Unicorn", "hat_viking", "hat_Visor", "hat_wallcap", "hat_pk04_CCC", "hat_whitetophat", "hat_Zipper", "hat_Starless" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetHat(p, convert_to_string(availableHats[randi(0, (int)availableHats.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetHat(*Game::pLocalPlayer, convert_to_string(availableHats[randi(0, (int)availableHats.size() - 1)]), NULL);
                    }
                    if (State.RandomSkin) {
                        std::vector availableSkins = { "skin_None", "skin_cosmic_rocket", "skin_parasite_Black", "skin_parasite_Blue", "skin_parasite_Brown", "skin_parasite_Cyan", "skin_parasite_Green", "skin_parasite_Lime", "skin_parasite_Orange", "skin_parasite_Purple", "skin_parasite_Red", "skin_parasite_White", "skin_parasite_Yellow", "skin_hanami_cardigan", "skin_hanami_kimono", "skin_hanami_matsuri", "skin_phoenix_wright", "skin_stardew_abigail", "skin_stardew_lewis", "skin_stardew_linus", "skin_stardew_mrqi", "skin_stardew_sebastian", "skin_stardew_overalls", "skin_Genshin_Skin_Paimon", "skin_racing_fungle", "skin_racing_skeld", "skin_racing_mira", "skin_racing_polus", "skin_racing_airship", "skin_racing_jim", "skin_bsb2_pompous", "skin_bsb2_powdered", "skin_bsb2_scarfYellowGreen", "skin_bsb2_scarfSepia", "skin_kamurocho_cinderella", "skin_kamurocho_akiyama", "skin_kamurocho_tropical", "skin_kamurocho_ichiban", "skin_kamurocho_kazuma", "skin_kamurocho_pirate", "skin_kamurocho_majima", "skin_kamurocho_saejima", "skin_kamurocho_zhao", "skin_paws_opossum", "skin_paws_parrot", "skin_paws_raccoon", "skin_claws_stripes", "skin_paws_foxGrey", "skin_claws_squid", "skin_claws_dragonRed", "skin_claws_frogSuit", "skin_claws_hoofed", "skin_claws_foxOrange", "skin_Edgeworth", "skin_artagan", "skin_Chetney", "skin_Fearne", "skin_Jester", "skin_Laudna", "skin_Molly", "skin_Nott", "skin_Vax", "skin_bb1_rainbowTube", "skin_bb1_fungleDress", "skin_lny_dragonDancer", "skin_BowKidskin", "skin_Conductorskin", "skin_HatKidSkinskin", "skin_MoustacheKidSkinskin", "skin_CrusaderSkinskin", "skin_HattySkinskin", "skin_KingSkinskin", "skin_BadelineSkinskin", "skin_MadelineSkinskin", "skin_Theoskin", "skin_CadenceSkinskin", "skin_FrederickSkinskin", "skin_Nocturnaskin", "skin_Papyrusskin", "skin_Sanskin", "skin_Undyneskin", "skin_GardenerSkin1skin", "skin_Wimpskin", "skin_Skins14_5skin", "skin_FishingSkinskin", "skin_FishSkinskin", "skin_InnerTubeSkinskin", "skin_LifeVestSkinskin", "skin_PusheenGreyskin", "skin_PusheenMintskin", "skin_PusheenPinkskin", "skin_PusheenPurpleskin", "skin_Pusheenicornskin", "skin_Slothskin", "skin_D2Hunter", "skin_D2Osiris", "skin_D2Saint14", "skin_D2Shaxx", "skin_D2Titan", "skin_D2Warlock", "skin_greedygrampaskin", "skin_presentskin", "skin_scarfskin", "skin_uglysweaterskin", "skin_benoit", "skin_Box1skin", "skin_BubbleWrapskin", "skin_Burlapskin", "skin_BushSign1skin", "skin_Horse1skin", "skin_Sack1skin", "skin_hl_fubuki", "skin_hl_gura", "skin_hl_korone", "skin_hl_marine", "skin_hl_mio", "skin_hl_moona", "skin_hl_okayu", "skin_hl_pekora", "skin_hl_risu", "skin_hl_watson", "skin_Bananaskin", "skin_BlueApronskin", "skin_ApronGreen", "skin_PinkApronskin", "skin_YellowApronskin", "skin_BlueSuspskin", "skin_OrangeSuspskin", "skin_PinkSuspskin", "skin_YellowSuspskin", "skin_ChefBlackskin", "skin_ChefBlue", "skin_ChefRed", "skin_Hotdogskin", "skin_screamghostface", "skin_halospartan", "skin_ratchet", "skin_w21_deer", "skin_w21_elf", "skin_w21_msclaus", "skin_w21_nutcracker", "skin_w21_santa", "skin_w21_snowmate", "skin_w21_tree", "skin_caitlin", "skin_enforcer", "skin_heim", "skin_jayce", "skin_jinx", "skin_vi", "skin_clown", "skin_fairy", "skin_fishmonger", "skin_pumpkin", "skin_vampire", "skin_witch", "skin_mummy", "skin_D2Cskin", "skin_Janitorskin", "skin_SportsBlueskin", "skin_SportsRedskin", "skin_Bling", "skin_General", "skin_ToppatSuitFem", "skin_ToppatVest", "skin_CCC", "skin_prisoner", "skin_PrisonerTanskin", "skin_PrisonerBlue", "skin_rhm", "skin_Bushskin", "skin_BusinessFemskin", "skin_BusinessFem-Tanskin", "skin_BusinessFem-Aquaskin", "skin_Hazmat", "skin_Hazmat-Blackskin", "skin_Hazmat-Blueskin", "skin_Hazmat-Greenskin", "skin_Hazmat-Pinkskin", "skin_Hazmat-Redskin", "skin_Hazmat-Whiteskin", "skin_Security", "skin_Tarmac", "skin_Abominalskin", "skin_RockLavaskin", "skin_RockIceskin", "skin_Sweaterskin", "skin_SweaterPinkskin", "skin_SweaterBlueskin", "skin_SweaterYellowskin", "skin_Archae", "skin_Miner", "skin_MinerBlackskin", "skin_Winter", "skin_JacketYellowskin", "skin_JacketGreenskin", "skin_JacketPurpleskin", "skin_Astro", "skin_Astronaut-Blueskin", "skin_Astronaut-Cyanskin", "skin_Astronaut-Orangeskin", "skin_Capt", "skin_Mech", "skin_MechanicRed", "skin_Military", "skin_MilitaryDesert", "skin_MilitarySnowskin", "skin_Police", "skin_Science", "skin_Scientist-Blueskin", "skin_Scientist-Darkskin", "skin_SuitB", "skin_SuitW", "skin_Skin_SuitRedskin", "skin_Wall" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetSkin(p, convert_to_string(availableSkins[randi(0, (int)availableSkins.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetSkin(*Game::pLocalPlayer, convert_to_string(availableSkins[randi(0, (int)availableSkins.size() - 1)]), NULL);
                    }
                    if (State.RandomVisor) {
                        std::vector availableVisors = { "visor_EmptyVisor", "visor_cosmic_alien", "visor_cosmic_infinity", "visor_cosmic_nebula", "visor_cosmic_stars", "visor_cosmic_sunglasses", "visor_cosmic_threeEyes", "visor_parasite_Black", "visor_parasite_Lime", "visor_hanami_petal", "visor_stardew_grandpa", "visor_stardew_lewis", "visor_stardew_linus", "visor_stardew_mrqi", "visor_racing_goggles", "visor_bsb2_bandage", "visor_bsb2_noteSad", "visor_bsb2_noteSmile", "visor_bsb2_heartGlassesRed", "visor_bsb2_starGlassesPurple", "visor_kamurocho_eyepatch", "visor_kamurocho_glasses", "visor_slothMask", "visor_paws_sacabambaspis", "visor_paws_parrotBeak", "visor_paws_bone", "visor_paws_carrot", "visor_paws_raccoon", "visor_claws_knife", "visor_claws_smallMuzzle", "visor_claws_whiskers", "visor_claws_bullRing", "visor_artagan", "visor_chetney", "visor_nott", "visor_happyMouthNote", "visor_henry", "visor_thatFace", "visor_lny_dragon", "visor_lny_pig", "visor_lny_rat", "visor_lny_snake", "visor_lny_tiger", "visor_mustachegirl", "visor_alienHominid", "visor_hattyHattington", "visor_king", "visor_theobeard", "visor_shopkeeper", "visor_gardenernose", "visor_goose", "visor_wimpglasses", "visor_animesunglassesVisor", "visor_heartsunglassesgoldVisor", "visor_mushroomeyesVisor", "visor_starsunglassesmintVisor", "visor_doubleeyepatch", "visor_fishhook", "visor_marshmallow", "visor_starfish", "visor_sunscreenv", "visor_pusheenGorgeousVisor", "visor_pusheenKissyVisor", "visor_pusheenKoolKatVisor", "visor_pusheenOmNomNomVisor", "visor_pusheenSmileVisor", "visor_pusheenYaaaaaayVisor", "visor_shuttershadesBlue", "visor_shuttershadesLime", "visor_shuttershadesPink", "visor_shuttershadesPurple", "visor_shuttershadesWhite", "visor_shuttershadesYellow", "visor_chimkin", "visor_eliksni", "visor_erisBandage", "visor_savathun", "visor_Candycane", "visor_IceBeard", "visor_Rudolph", "visor_anime", "visor_beautyMark", "visor_Plsno", "visor_Stealthgoggles", "visor_teary", "visor_tvColorTest", "visor_wash", "visor_vr_Vr-Black", "visor_vr_Vr-White", "visor_hl_ah", "visor_hl_bored", "visor_hl_hmph", "visor_hl_marine", "visor_hl_nothoughts", "visor_hl_nudge", "visor_hl_smug", "visor_hl_sweepy", "visor_hl_teehee", "visor_hl_wrong", "visor_BaconVisor", "visor_BananaVisor", "visor_BubbleBumVisor", "visor_CucumberVisor", "visor_IceCreamChocolateVisor", "visor_IceCreamMintVisor", "visor_IceCreamStrawberryVisor", "visor_IceCreamUbeVisor", "visor_PizzaVisor", "visor_ToastVisor", "visor_w21_carrot", "visor_w21_nutstache", "visor_w21_santabeard", "visor_w21_nye", "visor_heim", "visor_jinx", "visor_clownnose", "visor_eyeball", "visor_masque_blue", "visor_masque_white", "visor_masque_red", "visor_masque_green", "visor_mummy", "visor_D2CGoggles", "visor_is_beard", "visor_JanitorStache", "visor_Mouth", "visor_shopglasses", "visor_BillyG", "visor_Krieghaus", "visor_Reginald", "visor_Scar", "visor_WinstonStache", "visor_pk01_MonoclesVisor", "visor_pk01_RHMVisor", "visor_Galeforce", "visor_mira_card_blue", "visor_mira_card_red", "visor_mira_glasses", "visor_pk01_HazmatVisor", "visor_mira_mask_red", "visor_mira_mask_white", "visor_mira_mask_purple", "visor_mira_mask_green", "visor_mira_mask_blue", "visor_mira_mask_black", "visor_pk01_Security1Visor", "visor_Lava", "visor_polus_ice", "visor_SkiGoggleBlack", "visor_SKiGogglesOrange", "visor_SkiGogglesWhite", "visor_pk01_FredVisor", "visor_pk01_PaperMaskVisor", "visor_pk01_PlagueVisor", "hat_geoff", "visor_Blush", "visor_Bomba", "visor_Carrot", "visor_Crack", "visor_Dirty", "visor_Dotdot", "visor_EyepatchL", "visor_EyepatchR", "visor_LolliRed", "visor_LolliBlue", "visor_LolliOrange", "visor_LolliBrown", "visor_lollipopLemon", "visor_lollipopLime", "visor_lollipopCrew", "visor_PiercingL", "visor_PiercingR", "visor_pk01_AngeryVisor", "visor_pk01_DumStickerVisor", "visor_Stickynote_Purple", "visor_Stickynote_Cyan", "visor_Stickynote_Green", "visor_Stickynote_Orange", "visor_Stickynote_Pink", "visor_SciGoggles", "visor_SmallGlasses", "visor_SmallGlassesBlue", "visor_SmallGlassesRed", "visor_Straw" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetVisor(p, convert_to_string(availableVisors[randi(0, (int)availableVisors.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetVisor(*Game::pLocalPlayer, convert_to_string(availableVisors[randi(0, (int)availableVisors.size() - 1)]), NULL);
                    }
                    if (State.RandomPet) {
                        std::vector availablePets = { "pet_EmptyPet", "pet_cosmic_cat", "pet_parasite_Stressball", "pet_stardew_junimo", "pet_stardew_krobus", "pet_racing_beanCar", "pet_kamurocho_nancy", "pet_kamurocho_car", "pet_claws_spaceCat", "pet_Pate", "pet_Mister", "pet_lny_dragon", "pet_Crow", "pet_Rammy", "pet_Strawb", "pet_DancingSkeletonPet", "pet_napstamate", "pet_GoosePet", "pet_Creb", "pet_Pip", "pet_Pusheen", "pet_Stormy", "pet_D2GhostPet", "pet_D2PoukaPet", "pet_D2WormPet", "pet_coaltonpet", "pet_HolidayHamPet", "pet_nuggetPet", "pet_YuleGoatPet", "pet_BredPet", "pet_HamPet", "pet_GuiltySpark", "pet_clank", "pet_poro", "pet_Cube", "pet_Charles", "pet_Charles_Red", "pet_Bush", "pet_Lava", "pet_Snow", "pet_Alien", "pet_UFO", "pet_Bedcrab", "pet_Squig", "pet_Crewmate", "pet_ChewiePet", "pet_Robot", "pet_Doggy", "pet_frankendog", "pet_Hamster", "pet_Stickmin", "pet_Ellie", "pet_test" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetPet(p, convert_to_string(availablePets[randi(0, (int)availablePets.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetPet(*Game::pLocalPlayer, convert_to_string(availablePets[randi(0, (int)availablePets.size() - 1)]), NULL);
                    }
                    if (State.RandomNamePlate) {
                        std::vector availableNamePlates = { "nameplate_NoPlate", "nameplate_cosmic_launch", "nameplate_cosmic_nebula", "nameplate_cosmic_rings", "nameplate_parasite_Impostor", "nameplate_hanami_viewing", "nameplate_hanami_furoshiki", "nameplate_stardew_jellies", "nameplate_stardew_night", "nameplate_stardew_title", "nameplate_paimonstars", "nameplate_racing_beanCar", "nameplate_bsb2_error", "nameplate_bsb2_frame", "nameplate_bsb2_breach", "nameplate_bsb2_notes", "nameplate_kamurocho_hero", "nameplate_kamurocho_dragon", "nameplate_kamurocho_welcome", "nameplate_kamurocho_shimano", "nameplate_kamurocho_neon", "nameplate_kamurocho_nights", "nameplate_paws_fur", "nameplate_paws_jaguar", "nameplate_paws_feathers", "nameplate_claws_spaceCat", "nameplate_paws_scales", "nameplate_claws_spaceDog", "nameplate_claws_tigerPrint", "nameplate_claws_lilypad", "nameplate_cupcake", "nameplate_eyes", "nameplate_moons", "nameplate_tea", "nameplate_crewmatesBlue", "nameplate_crewmatesRed", "nameplate_horseHeaven", "nameplate_horsemateField", "nameplate_bb1_disco", "nameplate_bb1_hackerman", "nameplate_bb1_ram", "nameplate_bb1_rave", "nameplate_bb1_zen", "nameplate_lny_boar", "nameplate_lny_cat", "nameplate_lny_dog", "nameplate_lny_dragon", "nameplate_lny_goat", "nameplate_lny_horse", "nameplate_lny_monkey", "nameplate_lny_ox", "nameplate_lny_rabbit", "nameplate_lny_rat", "nameplate_lny_rooster", "nameplate_lny_snake", "nameplate_lny_tiger", "nameplate_lny_beanDragon", "nameplate_lny_clouds", "nameplate_lny_incense", "nameplate_lny_lanterns", "nameplate_lny_redPackets", "nameplate_lny_goldCrewmate", "nameplate_lny_goldImpostor", "nameplate_hourglass", "nameplate_shadows", "nameplate_battlefield", "nameplate_hominid", "nameplate_Celeste", "nameplate_flyingStrawberry", "nameplate_dungeonFloor", "nameplate_torch", "nameplate_fight", "nameplate_flowers", "nameplate_honk", "nameplate_knife", "nameplate_binocularsNameplate", "nameplate_DeadSunsetNameplate", "nameplate_cliffs", "nameplate_grill", "nameplate_plant", "nameplate_sandcastle", "nameplate_zipline", "nameplate_pusheen_01", "nameplate_pusheen_02", "nameplate_pusheen_03", "nameplate_pusheen_04", "nameplate_flagAro", "nameplate_flagMlm", "nameplate_hunter", "nameplate_lightfall", "nameplate_titan", "nameplate_warlock", "nameplate_candyCanePlate", "nameplate_SnowmiesPlate", "nameplate_winterForestPlate", "nameplate_WrappingPaperPlate", "nameplate_ballPit", "nameplate_cafeteria", "nameplate_deadbodyfound", "nameplate_ejected", "nameplate_flagAce", "nameplate_flagAgend", "nameplate_flagBi", "nameplate_flagGendF", "nameplate_flagGendQ", "nameplate_flagLesbian", "nameplate_flagNonbinary", "nameplate_flagPan", "nameplate_flagPride", "nameplate_flagRainbow", "nameplate_flagTrans", "nameplate_flashlight", "nameplate_impostor", "nameplate_ninjas", "nameplate_polus", "nameplate_reactor", "nameplate_ripple", "nameplate_seeker", "nameplate_shhh", "nameplate_BlimeyPlate", "nameplate_BreadPlate", "nameplate_EggPlate", "nameplate_PinkPlate", "nameplate_PizzaPlate", "nameplate_PlatePlate", "nameplate_Croissant", "nameplate_Lemon", "nameplate_Orange", "nameplate_w21_fireplace", "nameplate_w21_snow", "nameplate_w21_tree", "nameplate_hw_candy", "nameplate_hw_pumpkin", "nameplate_hw_woods", "nameplate_is_dig", "nameplate_is_game", "nameplate_is_ghost", "nameplate_is_green", "nameplate_is_sand", "nameplate_is_trees", "nameplate_is_yard", "nameplate_airship_CCC", "nameplate_airship_Diamond", "nameplate_airship_Emerald", "nameplate_airship_Gems", "nameplate_airship_government", "nameplate_Airship_Hull", "nameplate_airship_Ruby", "nameplate_airship_Sky", "nameplate_airship_Toppat", "nameplate_Mira_Cafeteria", "nameplate_Mira_Glass", "nameplate_Mira_Tiles", "nameplate_Mira_Vines", "nameplate_Mira_Wood", "nameplate_Polus_Colors", "nameplate_Polus_DVD", "nameplate_Polus_Ground", "nameplate_Polus_Lava", "nameplate_Polus_Planet", "nameplate_Polus_Snow", "nameplate_Polus_SpecimenBlue", "nameplate_Polus_SpecimenGreen", "nameplate_Polus_SpecimenPurple", "nameplate_Polus-Skyline", "nameplate_Polus-Snowmates" };
                        if (!State.SafeMode && State.CycleForEveryone) {
                            for (auto p : GetAllPlayerControl()) {
                                PlayerControl_RpcSetNamePlate(p, convert_to_string(availableNamePlates[randi(0, (int)availableNamePlates.size() - 1)]), NULL);
                            }
                        }
                        else PlayerControl_RpcSetNamePlate(*Game::pLocalPlayer, convert_to_string(availableNamePlates[randi(0, (int)availableNamePlates.size() - 1)]), NULL);
                    }
                    changeCycleDelay = int(State.CycleTimer * GetFps());
                }
                else if (changeCycleDelay > 0) changeCycleDelay--;
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

            if ((/*IsHost() || */!State.SafeMode) && State.ForceNameForEveryone) {
                static int forceNameDelay = 0;
                if (forceNameDelay <= 0) {
                    for (auto player : GetAllPlayerControl()) {
                        if (player == *Game::pLocalPlayer && State.SetName) continue;
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
                else if (!State.SafeMode) {
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
                        if (!State.SafeMode) ImpersonateName(GetPlayerData(players[rand]));
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
                float xOffset = (DirectX::GetWindowSize(true).x - DirectX::GetWindowSize(false).x) / 2.f;
                float yOffset = (DirectX::GetWindowSize(true).y - DirectX::GetWindowSize(false).y) / 2.f;
                Vector2 target = { mouse.x + xOffset, (DirectX::GetWindowSize(true).y - mouse.y - yOffset) };
                bool isValid = target.x != 0.f && target.y != 0.f; // Prevent teleporting to origin
                if (isValid && ImGui::IsKeyDown(VK_SHIFT) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    if (State.ControlPet) {
                        State.petPos = ScreenToWorld(target);
                    }
                    else {
                        if (IsInGame()) State.rpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                        if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                    }
                }
                else if (isValid && ImGui::IsKeyDown(VK_CONTROL) && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                    if (ctrlRightClickDelay <= 0) {
                        if (State.ControlPet) {
                            State.petPos = ScreenToWorld(target);
                        }
                        else {
                            if (IsInGame()) State.rpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                            if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSnapTo(ScreenToWorld(target)));
                        }
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
                float aliveSpeed = 2.5f, ghostSpeed = 3.f;
                if ((*Game::pLocalPlayer)->fields.inMovingPlat)
                    (*Game::pLocalPlayer)->fields.MyPhysics->fields.Speed = aliveSpeed; //remove speed on moving platform to avoid slowing down
                else
                    (*Game::pLocalPlayer)->fields.MyPhysics->fields.Speed = State.MultiplySpeed ? (float)(aliveSpeed * State.PlayerSpeed) : aliveSpeed;
                (*Game::pLocalPlayer)->fields.MyPhysics->fields.GhostSpeed = State.MultiplySpeed ? (float)(ghostSpeed * State.PlayerSpeed) : ghostSpeed;
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
                static float slackerTimer = 0.f;
                if (!IsInGame()) slackerTimer = 0.f;
                if (!IsInGame() && !IsInLobby()) {
                    State.VoteImmunePlayers.clear();
                }
                
                if (IsHost() && IsInLobby() && State.AutoStartGame && (600 - State.LobbyTimer) >= State.AutoStartTimer && !autoStartedGame) {
                    autoStartedGame = true;
                    InnerNetClient_SendStartGame(__this, NULL);
                }



                if (IsHost() && IsInGame() && State.AutoKickSlackers) {
                    slackerTimer += Time_get_deltaTime(NULL);
                    if (slackerTimer >= (float)State.AutoKickSlackersGrace) {
                        for (auto pc : GetAllPlayerControl()) {
                            if (pc == nullptr || pc == *Game::pLocalPlayer) continue;
                            auto pd = GetPlayerData(pc);
                            if (pd == nullptr || pd->fields.Disconnected) continue;
                            if (PlayerIsImpostor(pd)) continue;
                            auto tasks = GetNormalPlayerTasks(pc);
                            if (tasks.empty()) continue;
                            int total = (int)tasks.size();
                            int completed = 0;
                            for (auto task : tasks) {
                                if (task != nullptr && NormalPlayerTask_get_IsComplete(task, NULL))
                                    completed++;
                            }
                            int pct = (total > 0) ? (completed * 100 / total) : 100;
                            if (pct < State.AutoKickSlackersThreshold) {
                                std::string fc = convert_from_string(pd->fields.FriendCode);
                                bool whitelisted = std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), fc) != State.WhitelistFriendCodes.end();
                                if (!whitelisted || !State.AutoKickSlackersIgnoreWhitelist) {
                                    std::string playerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(pd, NULL));
                                    LOG_DEBUG("Task Enforcer: kicking " + playerName + " (" + std::to_string(pct) + "% tasks)");
                                    std::string msg = std::format("{} was kicked by Task Enforcer ({}/{}% tasks)", playerName, pct, State.AutoKickSlackersThreshold);
                                    InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), pc->fields._.OwnerId, false, NULL);
                                    
                                    Toasts::AddToast("Task Enforcer", msg, ImVec4(0.f, 1.f, 0.f, 1.f));
                                }
                            }
                        }
                        slackerTimer = 0.f;
                    }
                }

                if (IsInGame() && (*Game::pShipStatus) != NULL) {
                    const std::vector<const char*> SHIPVENTS = { "Admin", "Hallway", "Cafeteria", "Electrical", "Upper Engine", "Security", "Medbay", "Weapons", "Lower Reactor", "Lower Engine", "Shields", "Upper Reactor", "Upper Navigation", "Lower Navigation" };
                    const std::vector<const char*> HQVENTS = { "Balcony", "Cafeteria", "Reactor", "Laboratory", "Office", "Admin", "Greenhouse", "Medbay", "Decontamination", "Locker Room", "Launchpad" };
                    const std::vector<const char*> PBVENTS = { "Security", "Electrical", "O2", "Communications", "Office", "Admin", "Laboratory", "Lava Pool", "Storage", "Right Seismic", "Left Seismic", "Outside Admin" };
                    const std::vector<const char*> AIRSHIPVENTS = { "Vault", "Cockpit", "Viewing Deck", "Engine", "Kitchen", "Lower Main Hall", "Upper Main Hall", "Right Gap Room", "Left Gap Room", "Showers", "Records", "Cargo Bay" };
                    const std::vector<const char*> FUNGLEVENTS = { "Communications", "Kitchen", "Lookout", "Outside Dorm", "Laboratory", "Reactor", "Jungle (Laboratory)", "Jungle (Greenhouse)", "Splash Zone", "Cafeteria" };

                    std::vector<const char*> allVents;
                    switch (State.mapType) {
                    case Settings::MapType::Ship:
                        allVents = SHIPVENTS;
                        break;
                    case Settings::MapType::Hq:
                        allVents = HQVENTS;
                        break;
                    case Settings::MapType::Pb:
                        allVents = PBVENTS;
                        break;
                    case Settings::MapType::Airship:
                        allVents = AIRSHIPVENTS;
                        break;
                    case Settings::MapType::Fungle:
                        allVents = FUNGLEVENTS;
                        break;
                    }
                    int hqOffset = (int)(State.mapType == Settings::MapType::Hq);

                    static float ventTpDelay = 0.f;
                    if (ventTpDelay <= 0.f) {
                        if (State.SpamVentTpEveryoneRandom || State.spamRandomVentTpPlayers.size() != 0) {
                            for (auto p : GetAllPlayerControl()) {
                                if (State.IgnoreVentTpSelf && p == *Game::pLocalPlayer) continue;
                                if (!State.SpamVentTpEveryoneRandom) {
                                    auto it = std::find(State.spamRandomVentTpPlayers.begin(), State.spamRandomVentTpPlayers.end(), p->fields.PlayerId);
                                    if (it == State.spamRandomVentTpPlayers.end()) continue;
                                }

                                int ventId = randi(0 + hqOffset, (int)allVents.size() - 1 + hqOffset);

                                if (IsHost() || !State.SafeMode)
                                    PlayerPhysics_RpcBootFromVent(p->fields.MyPhysics, ventId, NULL);
                                else
                                    SendBootVentNonHost(p, ventId);

                                if (p == *Game::pLocalPlayer) State.AntiExploit_IsTeleportingSelf = true;
                            }
                        }

                        else if (State.SpamVentTpEveryone || !State.spamVentTpPlayers.empty()) {
                            for (auto p : GetAllPlayerControl()) {
                                if (p == NULL) continue;
                                if (State.IgnoreVentTpSelf && p == *Game::pLocalPlayer) continue;

                                bool isSpamVentedSeparately = State.spamVentTpPlayers.find(p->fields.PlayerId) != State.spamVentTpPlayers.end();

                                if (!State.SpamVentTpEveryone && !isSpamVentedSeparately) continue;

                                int ventId = isSpamVentedSeparately ? State.spamVentTpPlayers.at(p->fields.PlayerId) : State.SelectedVentId;

                                if (IsHost() || !State.SafeMode)
                                    PlayerPhysics_RpcBootFromVent(p->fields.MyPhysics, ventId, NULL);
                                else
                                    SendBootVentNonHost(p, ventId);

                                if (p == *Game::pLocalPlayer) State.AntiExploit_IsTeleportingSelf = true;
                            }
                        }
                        ventTpDelay = 0.75f;
                    }
                    else ventTpDelay -= Time_get_deltaTime(NULL);
                }

                if (IsInGame() && *Game::pShipStatus != NULL && State.mapType == Settings::MapType::Fungle) {
                    static float ziplineClimbDelay = 0.f;
                    static bool ziplineTop = false;

                    if (State.SpamZiplineEveryone || !State.spamZiplinePlayers.empty()) {
                        if (ziplineClimbDelay <= 0.f) {
                            for (auto p : GetAllPlayerControl()) {
                                if (p == NULL) continue;
                                if (State.IgnoreZiplineSelf && p == *Game::pLocalPlayer) continue;
                                
                                if (!State.SpamZiplineEveryone) {
                                    auto it = std::find(State.spamZiplinePlayers.begin(), State.spamZiplinePlayers.end(), p->fields.PlayerId);
                                    if (it == State.spamZiplinePlayers.end()) continue;
                                }

                                auto ziplineBehaviour = (ZiplineBehaviour*)((FungleShipStatus*)(*Game::pShipStatus))->fields._Zipline_k__BackingField;
                                if (ziplineBehaviour == NULL) continue;

                                PlayerControl_RpcUseZipline(p, p, ziplineBehaviour, ziplineTop, NULL);
                            }
                            ziplineTop = !ziplineTop;
                            ziplineClimbDelay = 2.5f;
                        }
                        else ziplineClimbDelay -= Time_get_deltaTime(NULL);
                    }
                    else ziplineClimbDelay = 0.f;
                }

                /*if (IsHost() && State.AutoStartGamePlayers && IsInLobby() && !editingAutoStartPlayerCount && !autoStartedGame) {  //this makes sure they dont start the game by mistake, if they are typing a 2 digit number eg 12
                    int playerCount = 0;
                    for (auto p : GetAllPlayerControl()) {
                        if (!p->fields.isNew) playerCount++;
                    }
                    if (playerCount >= State.AutoStartPlayerCount) {
                        autoStartedGame = true;
                        AmongUsClient_KickNotJoinedPlayers(*Game::pAmongUsClient, NULL);
                        InnerNetClient_SendStartGame((InnerNetClient*)(*Game::pAmongUsClient), NULL);
                    }
                }*/

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

                if (playerData->fields.ClientId == (*Game::pAmongUsClient)->fields._.ClientId) continue;
                // IS skill issues moment

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

                const std::string name = strToLower(RemoveHtmlTags(convert_from_string(GetPlayerOutfit(GetPlayerData(pc))->fields.PlayerName)));
                const std::string puid = convert_from_string(pd->fields.Puid);
                const std::string fc = convert_from_string(pd->fields.FriendCode);

                State.CurrentNames.insert(name);

                if (!BannedNamesSet.contains(name)) continue;

                if (State.Ban_IgnoreWhitelist &&
                    std::find(State.WhitelistFriendCodes.begin(), State.WhitelistFriendCodes.end(), fc) != State.WhitelistFriendCodes.end()) {
                    continue;
                }

                if (pd->fields.ClientId == (*Game::pAmongUsClient)->fields._.ClientId) continue; // Don't kick yourself

                State.CurrentForbiddenNames.insert(name);

                if (State.ForbiddenNames.contains(name)) continue;

                State.ForbiddenNames.insert(name);

                app::InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), pc->fields._.OwnerId, false, NULL);

                const std::string kickMsg = std::format("{} was detected by Name-Checker!", name);
                Toasts::AddToast("Name-Checker", kickMsg, ImVec4(0.f, 1.f, 0.f, 1.f));

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

                    std::string action = State.BanWarned ? "banned" : "kicked";
                    std::string kickMsg = std::format("{} was {} for receiving {} warns", friendCode, action, State.MaxWarns);
                    Toasts::AddToast(State.BanWarned ? "Ban by Warns" : "Kick by Warns", kickMsg, ImVec4(1.f, 0.f, 0.f, 1.f));

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

        if (State.TempBanEnabled) {
            static auto lastCheck = std::chrono::system_clock::now();
            auto now = std::chrono::system_clock::now();

            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCheck).count() >= 100) /* <- trigger threshold (0,1 sec) */ {
                for (auto it = State.TempBannedFCs.begin(); it != State.TempBannedFCs.end();) {
                    if (now >= it->second) {
                        it = State.TempBannedFCs.erase(it);
                        State.Save();
                    }
                    else {
                        if (IsInGame() || IsInLobby()) {
                            for (auto p : GetAllPlayerControl()) {
                                if (convert_from_string(p->fields.FriendCode) == it->first) {
                                    // Re-ban temp-banned:
                                    if (IsInGame()) {
                                        State.rpcQueue.push(new PunishPlayer(p, false));
                                    }
                                    if (IsInLobby()) {
                                        State.lobbyRpcQueue.push(new PunishPlayer(p, false));
                                    }
                                }
                            }
                        }
                        ++it;
                    }
                }
                lastCheck = now;
            }
        }

        if (IsInLobby() && IsHost() && GameOptionsManager_get_HasOptions(GameOptionsManager_get_Instance(NULL), NULL) && *Game::pLocalPlayer != NULL) {
            GameOptions options;
            if (State.AutoHostRole) {
                int index = (*Game::pLocalPlayer)->fields.PlayerId;
                auto assignedRole = State.assignedRoles[index];
                if (assignedRole != State.HostRoleToSet) {
                    int totalEngineers = (int)GetRoleCount(RoleType::Engineer, true) + (State.HostRoleToSet == RoleType::Engineer);
                    int totalScientists = (int)GetRoleCount(RoleType::Scientist, true) + (State.HostRoleToSet == RoleType::Scientist);
                    int totalTrackers = (int)GetRoleCount(RoleType::Tracker, true) + (State.HostRoleToSet == RoleType::Tracker);
                    int totalNoisemakers = (int)GetRoleCount(RoleType::Noisemaker, true) + (State.HostRoleToSet == RoleType::Noisemaker);
                    int totalDetectives = (int)GetRoleCount(RoleType::Detective, true) + (State.HostRoleToSet == RoleType::Detective);
                    int totalJudges = (int)GetRoleCount(RoleType::Judge, true) + (State.HostRoleToSet == RoleType::Judge);
                    int totalShapeshifters = (int)GetRoleCount(RoleType::Shapeshifter, true) + (State.HostRoleToSet == RoleType::Shapeshifter);
                    int totalPhantoms = (int)GetRoleCount(RoleType::Phantom, true) + (State.HostRoleToSet == RoleType::Phantom);
                    int totalVipers = (int)GetRoleCount(RoleType::Viper, true) + (State.HostRoleToSet == RoleType::Viper);
                    int totalImpostors = (int)GetRoleCount(RoleType::Impostor, true) + (State.HostRoleToSet == RoleType::Impostor);
                    int totalCrewmates = (int)GetRoleCount(RoleType::Crewmate, true) + (State.HostRoleToSet == RoleType::Crewmate);

                    int maxImpostors = GetMaxImpostorAmount((int)GetAllPlayerData().size());
                    int sumOfImpostorRoles = totalImpostors + totalShapeshifters + totalPhantoms + totalVipers;
                    int sumOfCrewmateRoles = totalEngineers + totalScientists + totalTrackers + totalNoisemakers + totalDetectives + totalJudges + totalCrewmates;

                    if (State.HostRoleToSet == RoleType::Impostor || State.HostRoleToSet == RoleType::Shapeshifter || State.HostRoleToSet == RoleType::Phantom || State.HostRoleToSet == RoleType::Viper) {
                        if (sumOfImpostorRoles <= maxImpostors) {
                            if (options.GetGameMode() == GameModes__Enum::HideNSeek) State.HostRoleToSet = RoleType::Impostor;
                            State.assignedRoles[index] = State.HostRoleToSet;
                        }
                    }
                    else {
                        if (sumOfCrewmateRoles <= (int)GetAllPlayerData().size() - maxImpostors) {
                            if (options.GetGameMode() == GameModes__Enum::HideNSeek) State.HostRoleToSet = RoleType::Engineer;
                            State.assignedRoles[index] = State.HostRoleToSet;
                        }
                    }
                }
            }
        }

        if (State.murderLoop) {
            auto selectedPlayer = State.selectedPlayer.validate();
            if (State.murderDelay <= 0) {
                if (State.murderCount > 0 && selectedPlayer.has_value() && !selectedPlayer.get_PlayerData()->fields.Disconnected) {
                    if (IsInGame()) {
                        State.rpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, selectedPlayer.get_PlayerControl(), 1, false));
                    }
                    else if (IsInLobby()) {
                        State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, selectedPlayer.get_PlayerControl(), 1, false));
                    }
                    State.murderDelay = GetFps() / 12;
                    State.murderCount--;
                }
                else {
                    State.murderLoop = false;
                    State.murderCount = 0;
                }
            }
            else State.murderDelay--;
        }

        if (State.farmLoop) {
            if (State.farmDelay <= 0 && *Game::pLocalPlayer != NULL) {
                if (State.farmCount > 0) {                    
                    uint8_t gameDataTag = 5, rpcFlag = 2;

                    auto writer = MessageWriter_Get(SendOption__Enum::Reliable, NULL);
                    MessageWriter_StartMessage(writer, gameDataTag, NULL);
                    MessageWriter_WriteInt32(writer, (*Game::pAmongUsClient)->fields._.GameId, NULL);

                    int maxPackedRpcs = 10 + GameOptions().GetInt(Int32OptionNames__Enum::MaxPlayers) * 2;

                    for (int i = 0; i < maxPackedRpcs; ++i) {
                        MessageWriter_StartMessage(writer, rpcFlag, NULL);
                        MessageWriter_WritePacked(writer, (*Game::pLocalPlayer)->fields._.NetId, NULL);
                        MessageWriter_WriteByte(writer, (uint8_t)RpcCalls__Enum::MurderPlayer, NULL);
                        MessageExtensions_WriteNetObject(writer, (InnerNetObject*)(*Game::pLocalPlayer), NULL);
                        MessageWriter_WriteInt32(writer, (int32_t)MurderResultFlags__Enum::Succeeded, NULL);
                        MessageWriter_EndMessage(writer, NULL);
                    }

                    MessageWriter_EndMessage(writer, NULL);
                    InnerNetClient_SendOrDisconnect((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
                    MessageWriter_Recycle(writer, NULL);

                    State.farmDelay = GetFps() / 15;
                    State.farmCount--;
                }
                else {
                    State.farmLoop = false;
                    State.farmCount = 0;
                }
            }
            else State.farmDelay--;
        }

        if (State.suicideLoop) {
            auto selectedPlayer = State.selectedPlayer.validate();
            if (State.suicideDelay <= 0) {
                if (State.suicideCount > 0 && selectedPlayer.has_value() && !selectedPlayer.get_PlayerData()->fields.Disconnected) {
                    if (IsInGame()) {
                        State.rpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), selectedPlayer.get_PlayerControl()));
                    }
                    else if (IsInLobby()) {
                        State.lobbyRpcQueue.push(new RpcMurderPlayer(selectedPlayer.get_PlayerControl(), selectedPlayer.get_PlayerControl()));
                    }
                    State.suicideDelay = GetFps() / 12;
                    State.suicideCount--;
                }
                else {
                    State.suicideLoop = false;
                    State.suicideCount = 0;
                }
            }
            else State.suicideDelay--;
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
    Application_set_targetFrameRate(State.GameFPS > 10 ? State.GameFPS : 60, NULL);
    InnerNetClient_Update(__this, method);

    if (!State.PanicMode) {
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

    if (State.FollowerCam != nullptr && State.shadowCollab != nullptr) {
        auto hud = Game::HudManager.GetInstance();
        auto chatState = hud->fields.Chat->fields.state;
        bool chatOpen = chatState == ChatControllerState__Enum::Open || chatState == ChatControllerState__Enum::Opening || chatState == ChatControllerState__Enum::Closing;

        auto fullScreen = hud->fields.FullScreen;
        Color fullScreenCol = fullScreen != NULL ? SpriteRenderer_get_color(fullScreen, NULL) : Color(1.f, 1.f, 1.f, 0.f);
        bool isFullScreenActive = fullScreen != NULL &&
            ((fullScreenCol.r == 0.f && fullScreenCol.g == 0.f && fullScreenCol.b == 0.f) || fullScreenCol.a <= 0.05f) &&
            GameObject_GetActive(Component_get_gameObject((Component_1*)fullScreen, NULL), NULL);

        auto gameMenu = hud->fields.GameMenu;
        bool isGameMenuActive = gameMenu != NULL &&
            GameObject_GetActive(Component_get_gameObject((Component_1*)gameMenu, NULL), NULL);

        bool isKillOverlayActive = hud->fields.KillOverlay != NULL &&
            KillOverlay_get_IsOpen((KillOverlay*)hud->fields.KillOverlay, NULL);

        float oldCamHeight = Camera_get_orthographicSize(State.FollowerCam, NULL);
        // State.EnableZoom_ResolutionSetFlag = false;

        auto mig = MatchInfoGuide_get_Instance(NULL);
        bool migOpen = mig != NULL && MatchInfoGuide_get_IsActive(mig, NULL);

        bool shouldEnableZoom = (!State.InMeeting && !State.InExileUI &&
            !chatOpen && !migOpen && !isFullScreenActive && !isGameMenuActive && !isKillOverlayActive &&
            (State.GameLoaded || (IsInLobby() && State.LobbyTimer <= 600.f - (Time_get_deltaTime(NULL) * 20) )) && !State.PanicMode);
        // from my testing, deltaTime * 20 doesn't cause UI bugs in the lobby
        float camHeight = shouldEnableZoom && State.EnableZoom ?
            (State.CameraHeight * 3) : 3.f;

        float del = camHeight - oldCamHeight;
        float step = std::abs(del) * Time_get_deltaTime(NULL) * 12;

        float newCamHeight = 0.f;
        float precision = 1e-6f;

        if (!State.EnableZoom_SmoothZoom || !shouldEnableZoom) newCamHeight = camHeight;
        else if (del < 0.f) newCamHeight = (std::max)(camHeight, oldCamHeight - step);
        else if (del > 0.f) newCamHeight = (std::min)(camHeight, oldCamHeight + step);

        if (std::abs(del) > precision) { // minimize floating point errors
            State.HasRefreshedUI = false;
            Camera_set_orthographicSize(State.FollowerCam, newCamHeight, NULL);
            // State.EnableZoom_PreResolutionSetCamHeight = newCamHeight;
            float aspect = Camera_get_aspect(State.FollowerCam, NULL);
            Camera_set_orthographicSize(State.shadowCollab->fields.ShadowCamera, newCamHeight, NULL);
            Camera_set_orthographicSize(hud->fields.UICamera, newCamHeight, NULL);
            auto shadowQuadTransform = Component_get_transform((Component_1*)State.shadowCollab->fields.ShadowQuad, NULL);
            Transform_set_localScale(shadowQuadTransform, { newCamHeight * aspect * 2.f, newCamHeight * 2.f, 0.f }, NULL);
            // rescale most UI elements accordingly (done in KeyboardJoystick.cpp)
        }
        else if (!State.HasRefreshedUI) {
            State.HasRefreshedUI = true;
        }

        /*if (State.EnableZoom && !State.InMeeting && !chatOpen && (State.GameLoaded || IsInLobby()) && !State.PanicMode) //chat button disappears after meeting
            Camera_set_orthographicSize(State.FollowerCam, State.CameraHeight * 3, NULL);
        else
            Camera_set_orthographicSize(State.FollowerCam, 3.0f, NULL);*/
        
        Transform* cameraTransform = Component_get_transform((Component_1*)State.FollowerCam, NULL);
        Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);
        if (State.EnableZoom && !State.InMeeting && State.CameraHeight > 3.0f)
            Transform_set_position(cameraTransform, { cameraVector3.x, cameraVector3.y, 100 }, NULL);
    }

    if (!State.PanicMode && (IsInGame() || IsInLobby())) {
        if (State.FreeCam) {
            auto mainCamera = Camera_get_main(NULL);

            Transform* cameraTransform = Component_get_transform((Component_1*)mainCamera, NULL);
            Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);

            if (State.camPos.x == NULL) {
                State.camPos = cameraVector3;
            }
            if (State.prevCamPos.x == NULL) {
                State.prevCamPos = cameraVector3;
            }

            auto kbjPlayer = (Player*)KeyboardJoystick__TypeInfo->static_fields->player;
            // BYTE arr[256];
            if (/*GetKeyboardState(arr) && */kbjPlayer != NULL && !State.ChatFocused)
            {
                // adhere to the game's keybinds, which can be changed in game

                float xOffset = 0, yOffset = 0;
                if (Player_GetButton(kbjPlayer, 44, NULL) /*(arr[0x57] & 0x80) != 0*/) {
                    yOffset = 1;
                }
                if (Player_GetButton(kbjPlayer, 39, NULL) /*(arr[0x41] & 0x80) != 0*/) {
                    xOffset = -1;
                }
                if (Player_GetButton(kbjPlayer, 42, NULL) /*(arr[0x53] & 0x80) != 0*/) {
                    yOffset = -1;
                }
                if (Player_GetButton(kbjPlayer, 40, NULL) /*(arr[0x44] & 0x80) != 0*/)
                {
                    xOffset = 1;
                }
                float magnitude = (xOffset == 0 && yOffset == 0) ? 1 : sqrt(xOffset * xOffset + yOffset * yOffset);

                float del = Time_get_deltaTime(NULL); // in seconds
                //check for zero and prevent you from moving ~1.414 times faster diagonally
                State.camPos.x += float(del * State.FreeCamSpeed * 3.f * xOffset / magnitude);
                State.camPos.y += float(del * State.FreeCamSpeed * 3.f * yOffset / magnitude);
                // 3 is multiplied because that is 1x speed as the ghost
            }

            Transform_set_position(cameraTransform, { State.camPos.x, State.camPos.y }, NULL);
        }

        static float petRpcDelay = 0.f;
        if (State.ControlPet && *Game::pLocalPlayer != nullptr && (IsInGame() || IsInLobby())) {
            // reference: https://github.com/MrDiamond64/Hydra/blob/main/src/routines/PetPlayer.cs

            auto local = *Game::pLocalPlayer;
            if (State.petPos.x == NULL) {
                State.petPos = GetTrueAdjustedPosition(*Game::pLocalPlayer);
            }

            auto kbjPlayer = (Player*)KeyboardJoystick__TypeInfo->static_fields->player;
            // BYTE arr[256];
            if (/*GetKeyboardState(arr) && */kbjPlayer != NULL && !State.ChatFocused)
            {
                float xOffset = 0, yOffset = 0;
                if (Player_GetButton(kbjPlayer, 44, NULL) /*(arr[0x57] & 0x80) != 0*/) {
                    yOffset = 1;
                }
                if (Player_GetButton(kbjPlayer, 39, NULL) /*(arr[0x41] & 0x80) != 0*/) {
                    xOffset = -1;
                }
                if (Player_GetButton(kbjPlayer, 42, NULL) /*(arr[0x53] & 0x80) != 0*/) {
                    yOffset = -1;
                }
                if (Player_GetButton(kbjPlayer, 40, NULL) /*(arr[0x44] & 0x80) != 0*/)
                {
                    xOffset = 1;
                }
                float magnitude = (xOffset == 0 && yOffset == 0) ? 1 : sqrt(xOffset * xOffset + yOffset * yOffset);

                float del = Time_get_deltaTime(NULL); // in seconds
                //check for zero and prevent you from moving ~1.414 times faster diagonally

                if (!State.FreeCam) {
                    State.petPos.x += float(del * 5.f * xOffset / magnitude);
                    State.petPos.y += float(del * 5.f * yOffset / magnitude);
                }
            }

            auto mainCamera = Camera_get_main(NULL);
            Transform* cameraTransform = Component_get_transform((Component_1*)mainCamera, NULL);
            Vector3 cameraVector3 = Transform_get_position(cameraTransform, NULL);

            if (!State.FreeCam) {
                Transform_set_position(cameraTransform, { State.petPos.x, State.petPos.y }, NULL);
            }

            if (State.prevCamPos.x == NULL) {
                State.prevCamPos = cameraVector3;
            }

            if (petRpcDelay <= 0.f && local->fields.MyPhysics != NULL) {
                auto currentPet = local->fields.cosmetics->fields.currentPet;
                PetBehaviour_SetGettingPet(currentPet, true, State.petPos, NULL);

                // auto pettingHand = CosmeticsLayer_get_PettingHand(local->fields.cosmetics, NULL);
                // PlayerPettingHand_StartPet(pettingHand, currentPet, NULL);
                // else PlayerPettingHand_StopPetting(pettingHand, NULL);

                auto inc = (InnerNetClient*)(*Game::pAmongUsClient);
                Vector2 localPos = { 10000.f * std::cos(Time_get_time(NULL)), 10000.f * std::sin(Time_get_time(NULL)) };

                auto writer = InnerNetClient_StartRpcImmediately(inc,
                    local->fields.MyPhysics->fields._.NetId, (uint8_t)RpcCalls__Enum::Pet,
                    SendOption__Enum::Reliable, -1, NULL);
                NetHelpers_WriteVector2(localPos, writer, NULL);
                NetHelpers_WriteVector2(State.petPos, writer, NULL);
                InnerNetClient_FinishRpcImmediately(inc, writer, NULL);

                petRpcDelay = (1.f / 30.f);
            }
            else petRpcDelay -= Time_get_deltaTime(NULL);
        }
        else petRpcDelay = 0.f;
    }

    if (State.DisableControlPetHand && (IsInGame() || IsInLobby())) {
        auto inc = (InnerNetClient*)(*Game::pAmongUsClient);
        auto local = *Game::pLocalPlayer;

        if (local != NULL) {
            auto writer = InnerNetClient_StartRpcImmediately(inc,
                local->fields.MyPhysics->fields._.NetId, (uint8_t)RpcCalls__Enum::Pet,
                SendOption__Enum::Reliable, -1, NULL);
            NetHelpers_WriteVector2(PlayerControl_GetTruePosition(local, NULL), writer, NULL);
            NetHelpers_WriteVector2(State.petPos, writer, NULL);
            InnerNetClient_FinishRpcImmediately(inc, writer, NULL);
            // move hand back to local player to not leave a lingering pet for others

            auto writer2 = InnerNetClient_StartRpcImmediately(inc,
                local->fields.MyPhysics->fields._.NetId, (uint8_t)RpcCalls__Enum::CancelPet,
                SendOption__Enum::Reliable, -1, NULL);
            InnerNetClient_FinishRpcImmediately(inc, writer2, NULL);

            PlayerPhysics_CancelPet(local->fields.MyPhysics, NULL);
        }

        State.petPos = { NULL, NULL };
        State.DisableControlPetHand = false;
    }

    if (State.OverflowTimer > 0.f) {
        State.OverflowTimer -= Time_get_deltaTime(NULL);
        if (State.OverflowTimer < 0.f) State.OverflowTimer = 0.f;
    }
}

void dAmongUsClient_OnGameJoined(AmongUsClient* __this, String* gameIdString, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dAmongUsClient_OnGameJoined executed", false);
    try {
        State.AutoJoinLobby = false;
        if (!State.PanicMode) {
            Log.Debug("Joined lobby " + convert_from_string(gameIdString));
            State.LastLobbyJoined = convert_from_string(gameIdString);

            std::string code = convert_from_string(InnerNet_GameCode_IntToGameName(
                __this->fields._.GameId, NULL));
            if (!code.empty() && code != "LWQQQQ" && code != "QQQQQQ") {
                std::string existingHost = "";
                bool found = false;
                for (auto it = State.LobbyHistory.begin(); it != State.LobbyHistory.end(); ++it) {
                    if (it->Code == code) {
                        existingHost = it->HostName;
                        State.LobbyHistory.erase(it);
                        found = true;
                        break;
                    }
                }

                auto cacheIt = State.LobbyHostCache.find(code);
                std::string hostName = (cacheIt != State.LobbyHostCache.end())
                    ? cacheIt->second
                    : existingHost;

                decltype(State.LobbyHistory)::value_type lobby;
                lobby.Code = code;
                lobby.HostName = hostName;
                State.LobbyHistory.push_front(lobby);
                while ((int)State.LobbyHistory.size() > State.LobbyHistoryMaxStored)
                    State.LobbyHistory.pop_back();
                State.Save();
            }
            State.LobbyHostCache.clear();
            State.assignedRolesPlayer.fill(nullptr);
            State.assignedRoles.fill(RoleType::Random);

            if (!State.PendingRejoinTargetFC.empty())
                State.PendingRejoinReady = true;
            else
                State.VotekickRejoinCount.clear();

            /*if (!State.PanicMode) {
                State.PanicMode = true;
                State.TempPanicMode = true;
            }*/
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in AmongUsClient_OnGameJoined (InnerNetClient)");
    }
    AmongUsClient_OnGameJoined(__this, gameIdString, method);
}

void dAmongUsClient_OnPlayerLeft(AmongUsClient* __this, ClientData* data, DisconnectReasons__Enum reason, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dAmongUsClient_OnPlayerLeft executed", false);
    try {
        State.BlinkPlayersTab = true;
        if (data->fields.Character) { // Don't use Object_1_IsNotNull().
            auto playerInfo = GetPlayerData(data->fields.Character);

            const std::string playerName = convert_from_string(data->fields.PlayerName);
            const std::string stringReason = GetDisconnectReasonString(reason);

            Log.Debug(playerName + " left by reason: " + stringReason);

            uint8_t playerId = data->fields.Character->fields.PlayerId;

            if (State.playerToAttach.get_PlayerId() == playerId)
                State.playerToAttach = {};

            if (State.modUsers.find(playerId) != State.modUsers.end())
                State.modUsers.erase(playerId);

            if (State.spamVentTpPlayers.find(playerId) != State.spamVentTpPlayers.end())
                State.spamVentTpPlayers.erase(playerId);

            auto it = std::find(State.spamRandomVentTpPlayers.begin(), State.spamRandomVentTpPlayers.end(), playerId);
            if (it != State.spamRandomVentTpPlayers.end())
                State.spamRandomVentTpPlayers.erase(it);

            auto it2 = std::find(State.spamZiplinePlayers.begin(), State.spamZiplinePlayers.end(), playerId);
            if (it2 != State.spamZiplinePlayers.end())
                State.spamZiplinePlayers.erase(it2);

            auto colorCycleIt = std::find(State.ColorCycledPlayers.begin(), State.ColorCycledPlayers.end(), playerId);
            if (colorCycleIt != State.ColorCycledPlayers.end()) {
                State.ColorCycledPlayers.erase(colorCycleIt);
            }

            auto voteIt = std::find(State.VoteImmunePlayers.begin(), State.VoteImmunePlayers.end(), playerId);
            if (voteIt != State.VoteImmunePlayers.end()) {
                State.VoteImmunePlayers.erase(voteIt);

                if (State.VoteRedirectTargets.find(playerId) != State.VoteRedirectTargets.end())
                    State.VoteRedirectTargets.erase(playerId);
            }

            auto cpiIt = std::find(State.checkedPlayerIds.begin(), State.checkedPlayerIds.end(), playerId);
            if (cpiIt != State.checkedPlayerIds.end()) {
                State.checkedPlayerIds.erase(cpiIt);
            }

            if (auto evtPlayer = GetEventPlayer(playerInfo); evtPlayer) {
                synchronized(Replay::replayEventMutex) {
                    auto source = evtPlayer.value();
                    State.liveReplayEvents.emplace_back(std::make_unique<DisconnectEvent>(source));
                    State.liveConsoleEvents.emplace_back(std::make_unique<DisconnectEvent>(source));

                    if (State.ShowConsoleEventsAsToasts &&
                        ConsoleGui::IsEventFiltered(EVENT_TYPES::EVENT_DISCONNECT) &&
                        ConsoleGui::IsPlayerFiltered(playerInfo->fields.PlayerId)) {
                        std::string toastContent = std::format("{} ({}) left the game!",
                            source.playerName, GetColorName(source.colorId));
                        Toasts::AddToast("Player Disconnected", toastContent, ImVec4(1.f, 1.f, 1.f, 1.f));
                    }
                }
            }

            if (!State.PanicMode && State.ExtendedNotifications) {
                const bool smacPunished = State.SMAC_PunishedPlayers.find(playerId) != State.SMAC_PunishedPlayers.end();

                if (smacPunished) {
                    State.SMAC_PunishedPlayers.erase(playerId);
                }

                if (!smacPunished) {
                    if (auto* notifier = (NotificationPopper*)Game::HudManager.GetInstance()->fields.Notifier) {
                        std::string notifyText = "Left: " + convert_from_string(data->fields.PlayerName) + " | Reason: " + stringReason;
                        NotificationPopper_AddDisconnectMessage(notifier, convert_to_string(notifyText), nullptr);
                        State.IgnoreOriginalInit_NotificationPopper = true;
                    }
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
    State.MIG_ThemeChanged = true;
}

void dAmongUsClient_OnPlayerJoined(AmongUsClient* __this, ClientData* data, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dAmongUsClient_OnPlayerJoined executed", false);
    State.BlinkPlayersTab = true;

    if (!data) {
        AmongUsClient_OnPlayerJoined(__this, data, method);
        return;
    }

    if (!State.PanicMode && State.ExtendedNotifications) {
        std::string name = data->fields.PlayerName != nullptr ? convert_from_string(data->fields.PlayerName) : "<N/A>";
        std::string friendCode = data->fields.FriendCode != nullptr ? convert_from_string(data->fields.FriendCode) : "<N/A>";

        if (friendCode.empty()) friendCode = "N/A";

        if (auto* notifier = (NotificationPopper*)Game::HudManager.GetInstance()->fields.Notifier) {
            AudioClip* soundBackup = notifier->fields.playerDisconnectSound;
            Color colorBackup = notifier->fields.disconnectColor;

            notifier->fields.disconnectColor = Color(0.278f, 1.0f, 0.278f, 1.0f);
            notifier->fields.playerDisconnectSound = nullptr;

            std::string notifyText = std::format("Joined: <noparse>{}</noparse> | <noparse>{}</noparse>", name, friendCode);
            NotificationPopper_AddDisconnectMessage(notifier, convert_to_string(notifyText), nullptr);

            notifier->fields.playerDisconnectSound = soundBackup;
            notifier->fields.disconnectColor = colorBackup;
        }
    }

    AmongUsClient_OnPlayerJoined(__this, data, method);
}

void dLobbyNotificationMessage_SetUp(LobbyNotificationMessage* __this, String* item, Sprite* icon, Color textColor, void* onDestroy, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dAmongUsClient_OnPlayerJoined executed", false);
    LobbyNotificationMessage_SetUp(__this, item, icon, textColor, onDestroy, method);

    if (textColor.r == 0.278f && textColor.g == 1.0f && textColor.b == 0.278f && textColor.a == 1.0f) {
        // check for the color being green to flip the sprite when needed
        SpriteRenderer_set_flipX(__this->fields.Icon, true, NULL);
    }
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
    if (State.ShowHookLogs) Log.HookDebug("Hook dCustomNetworkTransform_SnapTo executed", false);
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
    if (State.ShowHookLogs) Log.HookDebug("Hook dAmongUsClient_OnGameEnd executed", false);
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
    if (State.ShowHookLogs) Log.HookDebug("Hook dInnerNetClient_DisconnectInternal executed", false);
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
    if (State.ShowHookLogs) Log.HookDebug("Hook dInnerNetClient_EnqueueDisconnect executed", false);
    try {
        std::string reasonStr = convert_from_string(stringReason);
        if (reason == DisconnectReasons__Enum::Error &&
            (reasonStr == "Timeout while waiting for player ID assignment" || reasonStr == "Timeout while waiting for player data containers"))
            return;
        State.FollowerCam = nullptr;
        onGameEnd(); //removed antiban cuz it glitches the game
    }
    catch (...) {
        LOG_ERROR("Exception occurred in InnerNetClient_EnqueueDisconnect (InnerNetClient)");
    }
    return InnerNetClient_EnqueueDisconnect(__this, reason, stringReason, method);
}

void dGameManager_RpcEndGame(GameManager* __this, GameOverReason__Enum endReason, bool showAd, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dGameManager_RpcEndGame executed", false);
    if (!State.PanicMode && IsHost() && State.NoGameEnd)
        return;
    GameManager_RpcEndGame(__this, endReason, showAd, method);
}

void dKillOverlay_ShowKillAnimation_1(KillOverlay* __this, NetworkedPlayerInfo* killer, NetworkedPlayerInfo* victim, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dKillOverlay_ShowKillAnimation_1 executed", false);
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
    if (State.ShowHookLogs) Log.HookDebug("Hook dLogicOptions_GetKillDistance executed", false);

    return LogicOptions_GetKillDistance(__this, method);
}

void dLadder_SetDestinationCooldown(Ladder* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dLadder_SetDestinationCooldown executed", false);
    try {
        if (!State.PanicMode && State.NoLadderZiplineCooldown) {
            __this->fields._CoolDown_k__BackingField = 0.f;
            return;
        }
    }
    catch (...) {
        Log.Debug("Exception occurred in Ladder_SetDestinationCooldown (InnerNetClient)");
    }
    return Ladder_SetDestinationCooldown(__this, method);
}

void dVoteBanSystem_AddVote(VoteBanSystem* __this, int32_t srcClient, int32_t clientId, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dVoteBanSystem_AddVote executed", false);
    try {
        PlayerControl* sourcePlayer = *Game::pLocalPlayer;
        PlayerControl* affectedPlayer = *Game::pLocalPlayer;

        for (auto p : GetAllPlayerControl()) {
            if (p->fields._.OwnerId == srcClient) sourcePlayer = p;
            if (p->fields._.OwnerId == clientId) affectedPlayer = p;
        }
        if (sourcePlayer == NULL || affectedPlayer == NULL) return;

        std::string sourceplayerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(sourcePlayer), nullptr));
        std::string affectedplayerName = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(affectedPlayer), nullptr));

        if (clientId == (*Game::pLocalPlayer)->fields._.OwnerId) {
            State.VoteKicks++;
            if (State.ShowVoteKicks) {
                Toasts::AddToast("Votekick Alert", RemoveHtmlTags(sourceplayerName) + " attempted to votekick you!", ImVec4(1.f, 0.f, 0.f, 1.f));
            }
        }

        if (IsHost()) {
            if (affectedPlayer == *Game::pLocalPlayer && !State.PanicMode && State.AntiExploit_VotekicksAgainstSelfHost)
                return; // anti kick as host
            if (sourcePlayer == *Game::pLocalPlayer) {
                InnerNetClient_KickPlayer((InnerNetClient*)(*Game::pAmongUsClient), clientId, false, NULL);
                return;
            }
            if (State.DisableAllVotekicks) return;
        }

        if (State.AutoRejoinOnKick && !IsHost() && IsInLobby()
            && sourcePlayer == *Game::pLocalPlayer
            && !State.LastLobbyJoined.empty()) {
            int& count = State.VotekickRejoinCount[clientId];
            count++;
            if (count <= 2) {
                auto affectedData = GetPlayerData(affectedPlayer);
                if (affectedData != nullptr) {
                    std::string fc = convert_from_string(affectedData->fields.FriendCode);
                    State.PendingRejoinTargetFC = fc.empty()
                        ? convert_from_string(NetworkedPlayerInfo_get_PlayerName(affectedData, nullptr))
                        : fc;
                }
                else
                    State.PendingRejoinTargetFC = convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerData(affectedPlayer), nullptr));
                State.VotekickRejoinLobbyCode = State.LastLobbyJoined;
                State.VotekickRejoinPending = true;
                State.VotekickRejoinDelay = 0.25f; // a small delay to let the votekick go through.
            }
        }
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
    if (State.ShowHookLogs) Log.HookDebug("Hook dDisconnectPopup_DoShow executed", false);
    DisconnectPopup_DoShow(__this, method);
    bool shouldCopyCode = State.AutoCopyLobbyCode && State.LastLobbyJoined != "";
    if (!State.PanicMode || State.TempPanicMode) {
        switch (((InnerNetClient*)(*Game::pAmongUsClient))->fields.LastDisconnectReason) {
        case DisconnectReasons__Enum::Hacking: {
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("You were banned for hacking.\n\n{}{}",
                    shouldCopyCode ? "Lobby Code has been copied to the clipboard." : "Please stop.",
                    State.SafeMode ? "" : "\n\nDisabling safe mode isn't recommended on official servers!")), NULL);
        }
        break;
        /*case DisconnectReasons__Enum::Kicked: {
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("You were kicked from the lobby.\n\n{}",
                    shouldCopyCode ? "Lobby Code has been copied to the clipboard." : "You can rejoin the lobby if it hasn't started.")), NULL);
        }
        break;
        case DisconnectReasons__Enum::Banned: {
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("You were banned from the lobby.\n\n{}",
                    shouldCopyCode ? "Lobby Code has been copied to the clipboard." : "You can rejoin the lobby by changing your IP address.")), NULL);
        }
        break;*/
        default: {
            std::string prevText = convert_from_string(TMP_Text_get_text((TMP_Text*)__this->fields._textArea, NULL));
            TMP_Text_set_text((TMP_Text*)__this->fields._textArea,
                convert_to_string(std::format("{}{}", prevText,
                    shouldCopyCode ? "\n\nLobby Code has been copied to the clipboard." : "")), NULL);
        }
        break;
        }
        if (shouldCopyCode) ClipboardHelper_PutClipboardString(convert_to_string(State.LastLobbyJoined), NULL);
    }
}

bool dGameManager_DidImpostorsWin(GameManager* __this, GameOverReason__Enum reason, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dGameManager_DidImpostorsWin executed", false);
    return GameManager_DidImpostorsWin(__this, reason, method);
}

void dInnerNetClient_SetEndpoint(InnerNetClient* __this, String* addr, uint16_t port, bool dtls, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dInnerNetClient_SetEndpoint executed", false);

    if (!State.PanicMode) {
        try {
            if (State.UseCustomServer && !State.CustomServerIp.empty()) {
                addr = convert_to_string(State.CustomServerIp);
                port = State.CustomServerPort;
            }

            if (State.ForceDTLS) {
                dtls = true;
            }
        }
        catch (...) {
            LOG_ERROR("Exception in dInnerNetClient_SetEndpoint");
        }
    }

    InnerNetClient_SetEndpoint(__this, addr, port, dtls, method);
}

// Ignores the original NotificationPopper initialization method
void dNotificationPopper_AddDisconnectMessage(NotificationPopper* __this, String* item, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dNotificationPopper_AddDisconnectMessage executed", false);

    if (State.IgnoreOriginalInit_NotificationPopper) {
        State.IgnoreOriginalInit_NotificationPopper = false;
        return;
    }

    NotificationPopper_AddDisconnectMessage(__this, item, method);
}

void dCustomNetworkTransform_HandleRpc(CustomNetworkTransform* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dCustomNetworkTransform_HandleRpc executed", false);

    if (!State.PanicMode && callId == (uint8_t)RpcCalls__Enum::SnapTo &&
        __this->fields.myPlayer == *Game::pLocalPlayer && State.AntiExploit_UnauthorizedTeleports) return;

    CustomNetworkTransform_HandleRpc(__this, callId, reader, method);
}