#pragma once

void DetourInitilization();
void DetourUninitialization();

bool dAutoOpenDoor_DoUpdate(AutoOpenDoor* __this, float dt, MethodInfo* method);
void dInnerNetClient_Update(InnerNetClient* __this, MethodInfo* method);
void dAmongUsClient_OnGameJoined(AmongUsClient* __this, String* gameIdString, MethodInfo* method);
void dPlayerControl_OnGameStart(PlayerControl* __this, MethodInfo* method);
void dAmongUsClient_OnPlayerLeft(AmongUsClient* __this, ClientData* data, DisconnectReasons__Enum reason, MethodInfo* method);
void dAmongUsClient_OnPlayerJoined(AmongUsClient* __this, ClientData* data, MethodInfo* method);
void dCustomNetworkTransform_SnapTo(CustomNetworkTransform* __this, Vector2 position, uint16_t minSid, MethodInfo* method);
bool dStatsManager_get_AmBanned(StatsManager* __this, MethodInfo* method);
float dShipStatus_CalculateLightRadius(ShipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method);
float dStatsManager_get_BanPoints(StatsManager* __this, MethodInfo* method);
float dVent_CanUse(Vent* __this, NetworkedPlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method);
int32_t dStatsManager_get_BanMinutesLeft(StatsManager* __this, MethodInfo* method);
void dChatBubble_SetName(ChatBubble* __this, String* playerName, bool isDead, bool voted, Color color, MethodInfo* method);
void dChatController_AddChat(ChatController* __this, PlayerControl* sourcePlayer, String* chatText, bool censor, MethodInfo* method);
void dChatController_SetVisible(ChatController* __this, bool visible, MethodInfo* method);
void dGameStartManager_Update(GameStartManager* __this, MethodInfo* method);
void dHudManager_Update(HudManager* __this, MethodInfo* method);
Vector3 dCamera_ScreenToWorldPoint(Camera* __this, Vector3 position, MethodInfo* method);
void dKeyboardJoystick_Update(KeyboardJoystick* __this, MethodInfo* method);
void dScreenJoystick_FixedUpdate(ScreenJoystick* __this, MethodInfo* method);
void dMeetingHud_Awake(MeetingHud* __this, MethodInfo* method);
void dMeetingHud_Close(MeetingHud* __this, MethodInfo* method);
void dMeetingHud_Update(MeetingHud* __this, MethodInfo* method);
void dPlayerControl_StartMeeting(PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method);
void dMeetingHud_PopulateResults(MeetingHud* __this, Il2CppArraySize* states, MethodInfo* method);
void dPlainDoor_SetDoorway(PlainDoor* __this, bool open, MethodInfo* method);
void dPlayerControl_CompleteTask(PlayerControl* __this, uint32_t idx, MethodInfo* method);
void dPlayerControl_FixedUpdate(PlayerControl* __this, MethodInfo* method);
bool dPlayerControl_get_CanMove(PlayerControl* __this, MethodInfo* method);
void dPlayerControl_MurderPlayer(PlayerControl* __this, PlayerControl* target, MurderResultFlags__Enum resultFlags, MethodInfo* method);
void dPlayerControl_CmdCheckMurder(PlayerControl* __this, PlayerControl* target, MethodInfo* method);
void dPlayerControl_CheckMurder(PlayerControl* __this, PlayerControl* target, MethodInfo* method);
void dPlayerControl_RpcSyncSettings(PlayerControl* __this, Byte__Array* optionsByteArray, MethodInfo* method);
void dPlayerControl_CmdReportDeadBody(PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method);
void dPlayerControl_RpcSendChat(PlayerControl* __this, String* chatText, MethodInfo* method);
void dPlayerControl_RpcStartMeeting(PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method);
void dPlayerControl_HandleRpc(PlayerControl* __this, uint8_t callId, MessageReader* reader, MethodInfo* method);
void dPlayerControl_Shapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method);
void dPlayerControl_CmdCheckShapeshift(PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method);
void dPlayerControl_CmdCheckRevertShapeshift(PlayerControl* __this, bool animate, MethodInfo* method);
void dPlayerControl_ProtectPlayer(PlayerControl* __this, PlayerControl* target, int32_t colorId, MethodInfo* method);
void dRenderer_set_enabled(Renderer* __this, bool value, MethodInfo* method);
void dSceneManager_Internal_ActiveSceneChanged(Scene previousActiveScene, Scene newActiveScene, MethodInfo* method);
void dShipStatus_OnEnable(ShipStatus* __this, MethodInfo* method);
void dShipStatus_RpcUpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, int32_t amount, MethodInfo* method);
void dShipStatus_RpcCloseDoorsOfType(ShipStatus* __this, SystemTypes__Enum type, MethodInfo* method);
void dPolusShipStatus_OnEnable(PolusShipStatus* __this, MethodInfo* method);
void dVent_EnterVent(Vent* __this, PlayerControl* pc, MethodInfo* method);
void* dVent_ExitVent(Vent* __this, PlayerControl* pc, MethodInfo* method);
void dLobbyBehaviour_Start(LobbyBehaviour* __this, MethodInfo* method);
void dLobbyBehaviour_Update(LobbyBehaviour* __this, MethodInfo* method);
void dGameObject_SetActive(GameObject* __this, bool value, MethodInfo* method);
void dNoShadowBehaviour_LateUpdate(NoShadowBehaviour* __this, MethodInfo* method);
void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method);
void dAirshipStatus_OnEnable(AirshipStatus* __this, MethodInfo* method);
float dAirshipStatus_CalculateLightRadius(AirshipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method);
void dFungleShipStatus_OnEnable(FungleShipStatus* __this, MethodInfo* method);
void dSabotageSystemType_SetInitialSabotageCooldown(SabotageSystemType* __this, MethodInfo* method);
void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method);
void dDoorBreakerGame_Start(DoorBreakerGame* __this, MethodInfo* method);
void dDoorCardSwipeGame_Begin(DoorCardSwipeGame* __this, PlayerTask* playerTask, MethodInfo* method);
void dDebug_Log(Object* message, MethodInfo* method);
void dDebug_LogError(Object* message, MethodInfo* method);
void dDebug_LogException(Exception* exception, MethodInfo* method);
void dDebug_LogWarning(Object* message, MethodInfo* method);
void dVersionShower_Start(VersionShower* __this, MethodInfo* method);
void dEOSManager_StartInitialLoginFlow(EOSManager* __this, MethodInfo* method);
void dEOSManager_LoginFromAccountTab(EOSManager* __this, MethodInfo* method);
void dEOSManager_InitializePlatformInterface(EOSManager* __this, MethodInfo* method);
bool dEOSManager_IsFreechatAllowed(EOSManager* __this, MethodInfo* method);
bool dEOSManager_IsFriendsListAllowed(EOSManager* __this, MethodInfo* method);
void dEOSManager_UpdatePermissionKeys(EOSManager* __this, void* callback, MethodInfo* method);
void dEOSManager_Update(EOSManager* __this, MethodInfo* method);
String* dEOSManager_get_ProductUserId(EOSManager* __this, MethodInfo* method);
void dChatController_Update(ChatController* __this, MethodInfo* method);
void dChatController_SendFreeChat(ChatController* __this, MethodInfo* method);
bool dTextBoxTMP_IsCharAllowed(TextBoxTMP* __this, uint16_t i, MethodInfo* method);
void dTextBoxTMP_SetText(TextBoxTMP* __this, String* input, String* inputCompo, MethodInfo* method);
void dInnerNetClient_EnqueueDisconnect(InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method);
void dInnerNetClient_DisconnectInternal(InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method);
float dLogicOptions_GetKillDistance(LogicOptions* __this, MethodInfo* method);
//TaskBarMode__Enum dLogicOptions_GetTaskBarMode(LogicOptions* __this, MethodInfo* method);
void dGameManager_RpcEndGame(GameManager* __this, GameOverReason__Enum endReason, bool showAd, MethodInfo* method);
void dRoleManager_SelectRoles(RoleManager* __this, MethodInfo* method);
//void dRoleManager_AssignRolesForTeam(List_1_GameData_PlayerInfo_* players, RoleOptionsData* opts, RoleTeamTypes__Enum team, int32_t teamMax, Nullable_1_RoleTypes_ defaultRole, MethodInfo* method);
//void dRoleManager_AssignRolesFromList(List_1_GameData_PlayerInfo_* players, int32_t teamMax, List_1_RoleTypes_* roleList, int32_t* rolesAssigned, MethodInfo* method);
void dPlayerPhysics_FixedUpdate(PlayerPhysics* __this, MethodInfo* method);
bool dSaveManager_GetPurchase(String* itemKey, String* bundleKey, MethodInfo* method);
void dPlayerControl_TurnOnProtection(PlayerControl* __this, bool visible, int32_t colorId, int32_t guardianPlayerId, MethodInfo* method);
void dPlayerControl_RemoveProtection(PlayerControl* __this, MethodInfo* method);
void dAmongUsClient_OnGameEnd(AmongUsClient* __this, void* endGameResult, MethodInfo* method);
void dAccountManager_UpdateKidAccountDisplay(AccountManager* __this, MethodInfo* method);
void dPlayerStorageManager_OnReadPlayerPrefsComplete(PlayerStorageManager* __this, void* data, MethodInfo* method);
bool dPlayerPurchasesData_GetPurchase(PlayerPurchasesData* __this, String* itemKey, String* bundleKey, MethodInfo* method);
void dGameOptionsManager_set_CurrentGameOptions(GameOptionsManager* __this, IGameOptions* value, MethodInfo* method);
void dExileController_ReEnableGameplay(ExileController* __this, MethodInfo* method);
//void dActivityManager_UpdateActivity(void* __this, Activity_1 activity, void* callback, MethodInfo* method);
void dPingTracker_Update(PingTracker* __this, MethodInfo* method);
void dFriendsListUI_UpdateFriendCodeUI(FriendsListUI* __this, MethodInfo* method);
void dKillOverlay_ShowKillAnimation_1(KillOverlay* __this, NetworkedPlayerInfo* killer, NetworkedPlayerInfo* victim, MethodInfo* method);
void dKillButton_SetTarget(KillButton* __this, PlayerControl* target, MethodInfo* method);
PlayerControl* dImpostorRole_FindClosestTarget(ImpostorRole* __this, MethodInfo* method);
void dMushroomDoorSabotageMinigame_Begin(MushroomDoorSabotageMinigame* __this, PlayerTask* task, MethodInfo* method);
//void* dAmongUsClient_CoStartGameHost(AmongUsClient* __this, MethodInfo* method);
float dConsole_1_CanUse(Console_1* __this, NetworkedPlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method);
void dLadder_SetDestinationCooldown(Ladder* __this, MethodInfo* method);
void dZiplineConsole_SetDestinationCooldown(ZiplineConsole* __this, MethodInfo* method);
void dMushroomWallDoor_SetDoorway(MushroomWallDoor* __this, bool open, MethodInfo* method);
void dVoteBanSystem_AddVote(VoteBanSystem* __this, int32_t srcClient, int32_t clientId, MethodInfo* method);
void dPlatformSpecificData_Serialize(PlatformSpecificData* __this, MessageWriter* writer, MethodInfo* method);
int32_t dConstants_1_GetBroadcastVersion(MethodInfo* method);
bool dConstants_1_IsVersionModded(MethodInfo* method);
void dPlatformSpecificData_Serialize(PlatformSpecificData* __this, MessageWriter* writer, MethodInfo* method);
bool dLogicGameFlowNormal_IsGameOverDueToDeath(LogicGameFlowNormal* __this, MethodInfo* method);
bool dLogicGameFlowHnS_IsGameOverDueToDeath(LogicGameFlowHnS* __this, MethodInfo* method);
void dPlayerControl_CoSetRole(PlayerControl* __this, RoleTypes__Enum role, bool canOverride, MethodInfo* method);
void dNetworkedPlayerInfo_Serialize(NetworkedPlayerInfo* __this, MessageWriter* writer, bool initialState, MethodInfo* method);
void dNetworkedPlayerInfo_Deserialize(NetworkedPlayerInfo* __this, MessageReader* reader, bool initialState, MethodInfo* method);
void dDisconnectPopup_DoShow(DisconnectPopup* __this, MethodInfo* method);
void dEditAccountUsername_SaveUsername(EditAccountUsername* __this, MethodInfo* method);
bool dGameManager_DidImpostorsWin(GameManager* __this, GameOverReason__Enum reason, MethodInfo* method);
void dShipStatus_HandleRpc(ShipStatus* __this, uint8_t callId, MessageReader* reader, MethodInfo* method);
void dExileController_BeginForGameplay(ExileController* __this, NetworkedPlayerInfo* exiled, bool voteTie, MethodInfo* method);
void dChatBubble_SetText(ChatBubble* __this, String* chatText, MethodInfo* method);
void dPlayerControl_CmdCheckVanish(PlayerControl* __this, float maxDuration, MethodInfo* method);
void dPlayerControl_CmdCheckAppear(PlayerControl* __this, bool shouldAnimate, MethodInfo* method);
void dPlayerControl_SetRoleInvisibility(PlayerControl* __this, bool isActive, bool shouldAnimate, bool playFullAnimation, MethodInfo* method);
void dShipStatus_UpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount, MethodInfo* method);
void dPlayerControl_CmdCheckProtect(PlayerControl* __this, PlayerControl* target, MethodInfo* method);
void dMeetingHud_RpcVotingComplete(MeetingHud* __this, MeetingHud_VoterState__Array* states, NetworkedPlayerInfo* exiled, bool tie, MethodInfo* method);
bool dAccountManager_CanPlayOnline(AccountManager* __this, MethodInfo* method);
bool dLogicOptions_GetAnonymousVotes(LogicOptions* __this, MethodInfo* method);
AsyncOperationHandle_1_UnityEngine_GameObject_ dAssetReference_InstantiateAsync_1(AssetReference* __this, Transform* parent, bool instantiateInWorldSpace, MethodInfo* method);
void dMatchMakerGameButton_SetGame(MatchMakerGameButton* __this, GameListing gameListing, MethodInfo* method);
void dModManager_LateUpdate(ModManager* __this, MethodInfo* method);
void dEndGameNavigation_ShowDefaultNavigation(EndGameNavigation* __this, MethodInfo* method);
void dPlayerControl_SetLevel(PlayerControl* __this, uint32_t level, MethodInfo* method);
PlayerBodyTypes__Enum dHideAndSeekManager_GetBodyType(HideAndSeekManager* __this, PlayerControl* player, MethodInfo* method);
PlayerBodyTypes__Enum dNormalGameManager_GetBodyType(NormalGameManager* __this, PlayerControl* player, MethodInfo* method);
bool dVent_TryMoveToVent(Vent* __this, Vent* otherVent, String** error, MethodInfo* method);
float dPlayerControl_get_CalculatedAlpha(PlayerControl* __this, MethodInfo* method);
bool dPlayerControl_get_Visible(PlayerControl* __this, MethodInfo* method);
void dMeetingHud_CastVote(MeetingHud* __this, uint8_t playerId, uint8_t suspectIdx, MethodInfo* method);
QuickChatModes__Enum dMultiplayerSettingsData_get_ChatMode(MultiplayerSettingsData* __this, QuickChatModes__Enum value, MethodInfo* method);