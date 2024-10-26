#pragma once
#include <bitset>
#include <queue>
#include <unordered_set>
#include "_events.h"
#include "_rpc.h"
#include "keybinds.h"
#include "game.h"
#include "replay.hpp"

class Settings {
public:

    KeyBinds::Config KeyBinds = {
        VK_DELETE, // toggle menu
        VK_INSERT, // toggle radar
        VK_HOME, // toggle console
        VK_NEXT, // repair sabotage
        VK_LCONTROL, // noclip
        0x00, // autokill
        0x00, // close all doors
        0x00, // toggle zoom
        0x00, // toggle freecam
        0x00, // close current room door
        VK_END, // toggle replay
        0x00, //toggle HUD
        0x00, //reset appearance
        0x00, //save appearance
        0x00, //complete tasks
        VK_PAUSE //toggle SickoMenu
    };

    std::string selectedConfig = "default";
    int selectedConfigInt = 0;

    bool ImGuiInitialized = false;
    bool HasOpenedMenuBefore = false;
    bool ShowMenuOnStartup = true;
    bool ShowMenu = false;

#ifdef _DEBUG
    bool showDebugTab = false;
#endif
    bool RgbMenuTheme = false;
    bool GradientMenuTheme = false;
    bool MatchBackgroundWithTheme = false;
    bool SetName = false;
    bool SafeMode = true;
    bool UnlockCosmetics = true;
    bool SpoofLevel = false;
    bool SpoofCrashLevel = false;
    int FakeLevel = 1;
    int CrashLevel = 0;
    bool ShowKeybinds = true;
    bool KeybindsWhileChatting = true;
    bool SpoofFriendCode = false;
    bool UseGuestFriendCode = false;
    std::string GuestFriendCode = "";
    std::string FakeFriendCode = "";
    std::string FakePuid = "";
    bool SpoofPlatform = false;
    bool SpoofRPC = false;
    int FakePlatform = 0;
    int ChangeRPC = 0;
    bool SpoofGuestAccount = false;
    bool SpoofModdedHost = false;

    bool AdjustByDPI = true;
    float dpiScale = 1.f;
    bool dpiChanged = false;

    std::string CurrentScene;

    bool MaxVision = false;
    float PrevPlayerSpeed = 1.f;
    float PlayerSpeed = 1.f;
    bool MultiplySpeed = false;
	float PrevKillCooldown = 30.f;
	float KillCooldown = 30.f;
    float PrevGACooldown = 60.f;
    float GACooldown = 60.f;
    float CycleTimer = 0.5f;
    float PrevCycleTimer = 0.5f;
    float CycleDuration = CycleTimer * 50;
    bool UnlockVents = false;
    bool UnlockKillButton = false;
    bool ShowGhosts = false;
    int FakeRole = 0;
    bool AutoFakeRole = false;
    bool DisableVents = false;
    bool SpamReport = false;
    bool CrashSpamReport = false;
    bool UltimateSpamReport = false;
    bool DisableMeetings = false;
    bool DisableSabotages = false;
    bool DisableKills = false;
    bool BattleRoyale = false;
    bool NoGameEnd = false;
    bool ChatSpam = false;
    bool ChatSpamEveryone = false;
    bool AutoJoinLobby = false;
    std::string AutoJoinLobbyCode = "";

    bool ShowProtections = false;
    std::map<Game::PlayerId, std::pair<Game::ColorId, float/*Time*/>> protectMonitor;
    std::mutex protectMutex;

    bool RefreshChatButton = false;
    bool RevealVotes = false;
    bool RevealAnonymousVotes = false;

    bool ShowKillCD = false;

    bool ChatPaste = false;
    bool RevealRoles = false;
    bool AbbreviatedRoleNames = false;
    bool PlayerColoredDots = false;
    bool ShowPlayerInfo = false;
    float PrevKillDistance = 0.f;
    float KillDistance = 0.f;
    float GameKillDistance = 0.f;
    bool ModifyKillDistance = false;
    int PrevTaskBarUpdates = 0;
    int TaskBarUpdates = 0;
    bool ModifyTaskBarUpdates = false;
	bool PrevVisualTasks = true;
	bool VisualTasks = true;
    bool PrevShowNames = false;
    bool ShowNames = false;
    bool Cycler = false;
    bool CycleName = false;
    bool RandomColor = false;
    bool RandomHat = false;
    bool RandomSkin = false;
    bool RandomVisor = false;
    bool RandomPet = false;
    bool RandomNamePlate = false;
    bool CycleForEveryone = false;
    bool ForceNameForEveryone = false;
    bool CustomNameForEveryone = false;
    bool ForceColorForEveryone = false;
    bool CustomName = false;
    bool RgbName = false;
    bool CustomRgbName = false;
    bool ResizeName = false;
    bool IndentName = false;
    bool CspaceName = false;
    bool MspaceName = false;
    bool VoffsetName = false;
    bool RotateName = false;
    float NameSize = 1.f;
    float NameIndent = 0.f;
    float NameCspace = 0.f;
    float NameMspace = 0.f;
    float NameVoffset = 0.f;
    float NameRotate = 0.f;
    bool ItalicName = false;
    bool UnderlineName = false;
    bool StrikethroughName = false;
    bool BoldName = false;
    bool NobrName = false;
    bool ColoredName = false;
    ImVec4 NameColor1 = ImVec4(1.f, 1.f, 1.f, 1.f);
    ImVec4 NameColor2 = ImVec4(1.f, 1.f, 1.f, 1.f);
    float RgbNameColor = 0.f;
    bool ServerSideCustomName = false;
    bool NoAbilityCD = false;
    bool DarkMode = false;
    bool SeeVanishedPlayers = false;
    bool CycleInMeeting = true;
    bool CycleBetweenPlayers = false;

    bool NoClip = false;
    bool HotkeyNoClip = false;

    bool DisableLights = false;

    bool MoveInVentAndShapeshift = false;
    bool AlwaysMove = false;
    bool AnimationlessShapeshift = false;
    bool DisableKillAnimation = false;
    bool KillImpostors = false;
    bool KillInVanish = false;
    bool OnlyProtectCrewmates = false;
    bool BypassAngelProt = false;
    bool InfiniteKillRange = false;
    bool KillInLobbies = false;
    bool AutoKill = false;
    bool FakeAlive = false;
    bool ShowHost = false;
    bool HideWatermark = false;
    bool ShowVoteKicks = false;
    bool ShowFps = false;
    bool DoTasksAsImpostor = false;
    bool AlwaysUseKillExploit = false;
    bool AutoCopyLobbyCode = false;

    PlayerSelection selectedPlayer;
    std::vector<uint8_t> selectedPlayers = {};
    std::queue<RPCInterface*> rpcQueue;
    std::queue<RPCInterface*> lobbyRpcQueue;

    bool ShowRadar = false;
    bool ShowRadar_DeadBodies = false;
    bool ShowRadar_Ghosts = false;
    bool HideRadar_During_Meetings = false;
    bool ShowRadar_RightClickTP = false;
    bool LockRadar = false;
    bool RadarDrawIcons = false;
    bool RadarVisorRoleColor = false;
    bool RadarBorder = false;
    int RadarExtraWidth = 0;
    int RadarExtraHeight = 0;

    bool ShowEsp = false;
    bool ShowEsp_Ghosts = true;
    bool ShowEsp_Box = true;
    bool ShowEsp_Tracers = true;
    bool ShowEsp_Distance = true;
    bool HideEsp_During_Meetings = false;
    bool ShowEsp_RoleBased = false;
    bool ShowEsp_Crew = true;
    bool ShowEsp_Imp = true;

    bool InMeeting = false;
    bool PlayMedbayScan = false;
    bool PlayWeaponsAnimation = false;

    bool ChatAlwaysActive = false;
    bool ChatActiveOriginalState = false;
    bool ReadGhostMessages = false;
    bool ReadAndSendAumChat = false;
    bool ReadAndSendKillNetChat = false;
    bool ShiftRightClickTP = false;
    bool TeleportEveryone = false;
    bool RotateEveryone = false;
    bool RotateServerSide = false;
    bool RelativeTeleport = false;
    float RotateRadius = 1.f;
    float xCoordinate = 0.f;
    float yCoordinate = 0.f;

    bool confuser = false;
    bool confuseOnJoin = false;
    bool confuseOnStart = false;
    bool confuseOnKill = false;
    bool confuseOnVent = false;
    bool confuseOnMeeting = false;

    SystemTypes__Enum selectedDoor = SystemTypes__Enum::Hallway;
    std::vector<SystemTypes__Enum> mapDoors;
    std::vector<SystemTypes__Enum> pinnedDoors;
    bool CloseAllDoors = false;

    bool ShowConsole = false;
    bool ShowReplay = false;
    bool Replay_ShowOnlyLastSeconds = false;
    int Replay_LastSecondsValue = 1;
    bool Replay_ClearAfterMeeting = false;
    std::chrono::system_clock::time_point MatchStart;
    std::chrono::system_clock::time_point MatchCurrent;
    std::chrono::system_clock::time_point MatchEnd;
    std::chrono::system_clock::time_point MatchLive;
    // NOTE:
    // any code that modifies State.liveReplayEvents or any other collection should use the Replay.replayEventMutex
    // failure to do so will invalidate any existing iterator of any thread which will lead to rare and hard to diagnose crashes
    std::vector<std::unique_ptr<EventInterface>> liveReplayEvents;
    std::array<ImVec2, Game::MAX_PLAYERS> lastWalkEventPosPerPlayer;
    std::array<std::chrono::system_clock::time_point, Game::MAX_PLAYERS> replayDeathTimePerPlayer;
    std::map<Game::PlayerId, Replay::WalkEvent_LineData> replayWalkPolylineByPlayer;
    bool Replay_IsPlaying = true;
    bool Replay_IsLive = true;

    std::map<Game::Voter, Game::VotedFor> voteMonitor;

    //std::vector<Game::PlayerId> aumUsers;
    //std::vector<Game::PlayerId> sickoUsers;
    std::map<Game::PlayerId, std::string> modUsers;
    int32_t rpcCooldown = 15;
    int32_t playerKilledId = 0;

    std::array<PlayerControl*, Game::MAX_PLAYERS> assignedRolesPlayer = {};
    std::array<RoleType, Game::MAX_PLAYERS> assignedRoles = {};
    int mapHostChoice = -1;
    int impostors_amount = 0;
    int shapeshifters_amount = 0;
    int phantoms_amount = 0;
    int engineers_amount = 0;
    int scientists_amount = 0;
    int trackers_amount = 0;
    int noisemakers_amount = 0;
    int crewmates_amount = 0;

    bool Wallhack = false;
    bool FreeCam = false;
    float FreeCamSpeed = 1.f;

    float CameraHeight = 3.0;
    Camera* FollowerCam = nullptr;
    bool EnableZoom = false;

    bool DisableHud = false;
    bool GodMode = false;

    bool ActiveAttach = false;
    bool ActiveShapeshift = false;

    bool FakeCameraUsage = false;

    ImVec4 MenuThemeColor = ImVec4(1.f, 0.f, 0.424f, 1.f);
    ImVec4 MenuGradientColor = ImVec4(1.f, 0.f, 0.424f, 1.f);
    ImVec4 MenuGradientColor1 = ImVec4(1.f, 0.f, 0.424f, 1.f);
    ImVec4 MenuGradientColor2 = ImVec4(1.f, 0.f, 0.424f, 1.f);
    ImVec4 RgbColor = ImVec4(1.f, 0.071f, 0.f, 1.f);
    ImVec4 SelectedColor = ImVec4(1.f, 1.f, 1.f, 0.75f);
    ImVec4 SelectedReplayMapColor = ImVec4(1.f, 1.f, 1.f, 0.75f);

    Game::ColorId SelectedColorId = 0; // Red
    Game::ColorId RandomColorId = 0; // Red
    Game::ColorId HostSelectedColorId = 0; //Red
    int SelectedRoleId = 0; // Crewmate
    int SelectedGameEndReasonId = 0;
    std::string originalName = "-";
    String* originalNamePlate = nullptr;
    String* originalSkin = nullptr;
    String* originalHat = nullptr;
    String* originalVisor = nullptr;
    String* originalPet = nullptr;
    String* cyclerNamePlate = nullptr;
    String* cyclerSkin = nullptr;
    String* cyclerHat = nullptr;
    String* cyclerVisor = nullptr;
    String* cyclerPet = nullptr;
    Game::ColorId originalColor = Game::NoColorId;

    bool SnipeColor = false;
    bool activeImpersonation = false;
    bool activeChatSpoof = false;
    bool activeWhisper = false;

    PlayerSelection playerToFollow;
    PlayerSelection playerToAttach;
    PlayerSelection playerToChatAs;
    PlayerSelection playerToWhisper;

    Vector3 camPos = { NULL, NULL, NULL };
    Vector3 prevCamPos = { NULL, NULL, NULL };

    bool FlipSkeld = false;
    bool CustomImpostorAmount = false;
    int ImpostorCount = 1;
    bool DisableCallId = false;
    uint8_t ToDisableCallId = 0;
    uint8_t VoteKicks = 0;

	bool OcclusionCulling = false;
    bool ShowUnityLogs = true;
    bool ShowHookLogs = false;

    int LobbyTimer = -1;
    float ChatCooldown = 0.f;
    bool MessageSent = false;
    bool ChatFocused = false;
    bool IsRevived = false;

    std::string chatMessage = "";
    std::string userName = "";
    std::vector<std::string> cyclerUserNames = {};
    int cyclerNameGeneration = 0;
    int confuserNameGeneration = 0;
    std::string rgbCode = "";
    std::string hostUserName = "";
    std::string customCode = "SICKOS";
    bool HideCode = false;
    bool RgbLobbyCode = false;

    bool PanicMode = false;
    bool TempPanicMode = false; //prevent instant crash on joining lobby
    bool BlinkPlayersTab = false; //prevent instant crash on player leaving
    bool SickoDetection = true;
    bool AmongUsMenuDetection = false;
    bool BetterAmongUsDetection = false;
    bool KillNetworkDetection = false;
    bool ForceLoginAsGuest = false;
    bool DisableHostAnticheat = false;
    bool TournamentMode = false;

    std::unordered_set<std::string> Friends;
    std::unordered_set<Game::PlayerId> InGameFriends;
    std::unordered_set<std::string> Block;
    std::unordered_set<Game::PlayerId> InGameBlock;
    std::vector<std::string /*friendcode*/> tournamentFriendCodes;
    std::map<std::string /*friendcode*/, float /*points*/> tournamentPoints;
    std::map<std::string /*friendcode*/, float /*points*/> tournamentWinPoints;
    std::map<std::string /*friendcode*/, float /*points*/> tournamentCalloutPoints;
    std::map<std::string /*friendcode*/, float /*points*/> tournamentEarlyDeathPoints;
    std::map<std::string /*friendcode*/, float /*points*/> tournamentKillCaps;
    std::vector<std::string> tournamentAssignedImpostors;
    std::vector<std::string> tournamentAliveImpostors;
    std::vector<std::string> tournamentCallers;
    std::vector<std::string> tournamentCalledOut;
    bool tournamentFirstMeetingOver = false;

    enum class MapType : uint8_t
    {
        Ship = 0,
        Hq = 1,
        Pb = 2,
        Airship = 3,
        Fungle = 4
    } mapType = MapType::Ship;

    enum class PointReason : uint8_t
    {
        ImpKill = 0,
        ImpWin = 1,
        AllImpsWin = 2,
        ImpVoteOut = 3,
        CrewVoteOut = 4,
        ImpVoteOutCorrect = 5,
        ImpVoteOutIncorrect = 6,
        CrewWin = 7,
        CorrectCallout = 8,
        IncorrectCallout = 9,
        ImpLose = 10,
    };

    bool CanChangeOutfit = false;
    uint8_t OutfitCooldown = 50;
    RoleTypes__Enum RealRole = RoleTypes__Enum::Crewmate;
    DisconnectReasons__Enum LastDisconnectReason = DisconnectReasons__Enum::Unknown;
    std::string LastLobbyJoined = "";
    bool IsPreHosting = false;
    bool GameLoaded = false;

    bool AutoOpenDoors = false;

    Settings()
    {
        Replay::Reset();
    }

    std::string SickoVersion = "v3.7.2";

    bool Enable_SMAC = false;
    int SMAC_Punishment = 0;
    int SMAC_HostPunishment = 0;
    bool SMAC_AddToBlacklist = false;
    bool SMAC_PunishBlacklist = false;
    bool SMAC_CheckAUM = true;
    bool SMAC_CheckSicko = true;
    bool SMAC_CheckBadNames = true;
    bool SMAC_CheckColor = true;
    bool SMAC_CheckCosmetics = true;
    bool SMAC_CheckChatNote = true;
    bool SMAC_CheckScanner = true;
    bool SMAC_CheckAnimation = true;
    bool SMAC_CheckTasks = true;
    bool SMAC_CheckRole = true;
    bool SMAC_CheckChat = true;
    bool SMAC_CheckMeeting = true;
    bool SMAC_CheckReport = true;
    bool SMAC_CheckMurder = true;
    bool SMAC_CheckShapeshift = true;
    bool SMAC_CheckVanish = true;
    bool SMAC_CheckLevel = true;
    bool SMAC_CheckVent = true;
    bool SMAC_CheckSabotage = true;
    int SMAC_HighLevel = 1000;
    std::map<std::string /*puid*/, std::string> SMAC_Blacklist = {};
    std::vector<uint8_t> SMAC_AttemptBanLobby = {};

    void Load();
    void Save();
    void Delete();
};

extern Settings State;