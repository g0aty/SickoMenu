using namespace app;

DO_APP_FUNC(Type*, Type_GetType, (String* typeName, MethodInfo* method), "mscorlib, System.Type System.Type::GetType(System.String)");
DO_APP_FUNC(Type*, RuntimeType_MakeGenericType_1, (Type* gt, /*Type__Array**/void* types, MethodInfo* method), "mscorlib, System.Type System.RuntimeType::MakeGenericType(System.Type, System.Type[])");

//DO_APP_FUNC(Object*, MonoMethod_InternalInvoke, (MonoMethod* __this, Object* obj, /*Object__Array*/void* parameters, Exception** exc, MethodInfo* method), "mscorlib, System.Object System.Reflection.MonoMethod::InternalInvoke(System.Object, System.Object[], System.Exception&)");

DO_APP_FUNC(GameObject*, Component_get_gameObject, (Component_1* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.GameObject UnityEngine.Component::get_gameObject()");
DO_APP_FUNC(Transform*, Component_get_transform, (Component_1* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Transform UnityEngine.Component::get_transform()");

DO_APP_FUNC(void, Object_DestroyImmediate, (Object_1* obj, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Object::DestroyImmediate(UnityEngine.Object)");
DO_APP_FUNC(Component_1*, Component_GetComponent, (Component_1* __this, Type* type, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Component UnityEngine.Component::GetComponent(System.Type)");

DO_APP_FUNC(Transform*, GameObject_get_transform, (GameObject* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Transform UnityEngine.GameObject::get_transform()");
DO_APP_FUNC(Transform*, Transform_GetRoot, (Transform* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Transform UnityEngine.Transform::GetRoot()");
DO_APP_FUNC(String*, Component_get_tag, (Component_1* __this, MethodInfo* method), "UnityEngine.CoreModule, System.String UnityEngine.Component::get_tag()");
DO_APP_FUNC(void, GameObject_set_layer, (GameObject* __this, int32_t value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.GameObject::set_layer(System.Int32)");
DO_APP_FUNC(int32_t, GameObject_get_layer, (GameObject* __this, MethodInfo* method), "UnityEngine.CoreModule, System.Int32 UnityEngine.GameObject::get_layer()");
DO_APP_FUNC(int32_t, LayerMask_NameToLayer, (String* layerName, MethodInfo* method), "UnityEngine.CoreModule, System.Int32 UnityEngine.LayerMask::NameToLayer(System.String)");
DO_APP_FUNC(Object_1__Array*, Object_1_FindObjectsOfType, (Type* type, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Object[] UnityEngine.Object::FindObjectsOfType(System.Type)");
DO_APP_FUNC(String*, Scene_GetNameInternal, (int32_t sceneHandle, MethodInfo* method), "UnityEngine.CoreModule, System.String UnityEngine.SceneManagement.Scene::GetNameInternal(System.Int32)");
DO_APP_FUNC(void, SceneManager_Internal_ActiveSceneChanged, (Scene previousActiveScene, Scene newActiveScene, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.SceneManagement.SceneManager::Internal_ActiveSceneChanged(UnityEngine.SceneManagement.Scene, UnityEngine.SceneManagement.Scene)");
DO_APP_FUNC(Vector3, Transform_get_position, (Transform* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Vector3 UnityEngine.Transform::get_position()");
DO_APP_FUNC(void, Transform_set_position, (Transform* __this, Vector3 value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Transform::set_position(UnityEngine.Vector3)");
DO_APP_FUNC(Vector3, Transform_get_localPosition, (Transform* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Vector3 UnityEngine.Transform::get_localPosition()");
DO_APP_FUNC(void, Transform_set_localPosition, (Transform* __this, Vector3 value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Transform::set_localPosition(UnityEngine.Vector3)");
DO_APP_FUNC(float, Vector2_Distance, (Vector2 a, Vector2 b, MethodInfo* method), "UnityEngine.CoreModule, System.Single UnityEngine.Vector2::Distance(UnityEngine.Vector2, UnityEngine.Vector2)");
DO_APP_FUNC(bool, Collider2D_OverlapPoint, (Collider2D* __this, Vector2 point, MethodInfo* method), "UnityEngine.Physics2DModule, System.Boolean UnityEngine.Collider2D::OverlapPoint(UnityEngine.Vector2)");
DO_APP_FUNC(String*, Application_get_version, (MethodInfo* method), "UnityEngine.CoreModule, System.String UnityEngine.Application::get_version()");
DO_APP_FUNC(void, Renderer_set_enabled, (Renderer* __this, bool value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Renderer::set_enabled(System.Boolean)");
DO_APP_FUNC(int32_t, Camera_GetAllCameras, (Camera__Array* cameras, MethodInfo* method), "UnityEngine.CoreModule, System.Int32 UnityEngine.Camera::GetAllCameras(UnityEngine.Camera[])");
DO_APP_FUNC(int32_t, Camera_get_allCamerasCount, (MethodInfo* method), "UnityEngine.CoreModule, System.Int32 UnityEngine.Camera::get_allCamerasCount()");
DO_APP_FUNC(Camera*, Camera_get_main, (MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Camera UnityEngine.Camera::get_main()");
DO_APP_FUNC(void, Camera_set_orthographicSize, (Camera* __this, float value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Camera::set_orthographicSize(System.Single)");
DO_APP_FUNC(float, Camera_get_orthographicSize, (Camera* __this, MethodInfo* method), "UnityEngine.CoreModule, System.Single UnityEngine.Camera::get_orthographicSize()");
DO_APP_FUNC(Color, SpriteRenderer_get_color, (SpriteRenderer* __this, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Color UnityEngine.SpriteRenderer::get_color()");
DO_APP_FUNC(void, SpriteRenderer_set_color, (SpriteRenderer* __this, Color value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.SpriteRenderer::set_color(UnityEngine.Color)");
DO_APP_FUNC(float, Time_get_deltaTime, (MethodInfo* method), "UnityEngine.CoreModule, System.Single UnityEngine.Time::get_deltaTime()");
DO_APP_FUNC(float, Time_get_fixedDeltaTime, (MethodInfo* method), "UnityEngine.CoreModule, System.Single UnityEngine.Time::get_fixedDeltaTime()");
DO_APP_FUNC(float, Time_get_realtimeSinceStartup, (MethodInfo* method), "UnityEngine.CoreModule, System.Single UnityEngine.Time::get_realtimeSinceStartup()");
DO_APP_FUNC(float, Time_get_time, (MethodInfo* method), "UnityEngine.CoreModule, System.Single UnityEngine.Time::get_time()");

DO_APP_FUNC(int32_t, Screen_get_width, (MethodInfo* method), "UnityEngine.CoreModule, System.Int32 UnityEngine.Screen::get_width()");
DO_APP_FUNC(int32_t, Screen_get_height, (MethodInfo* method), "UnityEngine.CoreModule, System.Int32 UnityEngine.Screen::get_height()");
DO_APP_FUNC(bool, Screen_get_fullScreen, (MethodInfo* method), "UnityEngine.CoreModule, System.Boolean UnityEngine.Screen::get_fullScreen()");

DO_APP_FUNC(void, AirshipStatus_OnEnable, (AirshipStatus* __this, MethodInfo* method), "Assembly-CSharp, System.Void AirshipStatus::OnEnable()");
DO_APP_FUNC(float, AirshipStatus_CalculateLightRadius, (AirshipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method), "Assembly-CSharp, System.Single AirshipStatus::CalculateLightRadius(NetworkedPlayerInfo)");

DO_APP_FUNC(void, FungleShipStatus_OnEnable, (FungleShipStatus* __this, MethodInfo* method), "Assembly-CSharp, System.Void FungleShipStatus::OnEnable()");

DO_APP_FUNC(bool, AutoOpenDoor_DoUpdate, (AutoOpenDoor* __this, float dt, MethodInfo* method), "Assembly-CSharp, System.Boolean AutoOpenDoor::DoUpdate(System.Single)");

//DO_APP_FUNC(void, NoShadowBehaviour_LateUpdate, (NoShadowBehaviour* __this, MethodInfo* method), "Assembly-CSharp, System.Void NoShadowBehaviour::LateUpdate()");
DO_APP_FUNC(void, NoShadowBehaviour_SetMaskFunction, (NoShadowBehaviour* __this, int32_t func, MethodInfo* method), "Assembly-CSharp, System.Void NoShadowBehaviour::SetMaskFunction(System.Int32)");

DO_APP_FUNC(Vector3, Camera_ScreenToWorldPoint, (Camera* __this, Vector3 position, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Vector3 UnityEngine.Camera::ScreenToWorldPoint(UnityEngine.Vector3)");

DO_APP_FUNC(void, CustomNetworkTransform_RpcSnapTo, (CustomNetworkTransform* __this, Vector2 position, MethodInfo* method), "Assembly-CSharp, System.Void CustomNetworkTransform::RpcSnapTo(UnityEngine.Vector2)");
DO_APP_FUNC(void, CustomNetworkTransform_SnapTo, (CustomNetworkTransform* __this, Vector2 position, uint16_t minSid, MethodInfo* method), "Assembly-CSharp, System.Void CustomNetworkTransform::SnapTo(UnityEngine.Vector2, System.UInt16)");

DO_APP_FUNC(Vector2, DeadBody_get_TruePosition, (DeadBody* __this, MethodInfo* method), "Assembly-CSharp, UnityEngine.Vector2 DeadBody::get_TruePosition()");

DO_APP_FUNC(NetworkedPlayerInfo*, GameData_GetPlayerById, (GameData* __this, uint8_t id, MethodInfo* method), "Assembly-CSharp, NetworkedPlayerInfo GameData::GetPlayerById(System.Byte)");

DO_APP_FUNC(void, GameObject_SetActive, (GameObject* __this, bool value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.GameObject::SetActive(System.Boolean)");

DO_APP_FUNC(void, KeyboardJoystick_Update, (KeyboardJoystick* __this, MethodInfo* method), "Assembly-CSharp, System.Void KeyboardJoystick::Update()");
DO_APP_FUNC(void, ScreenJoystick_FixedUpdate, (ScreenJoystick* __this, MethodInfo* method), "Assembly-CSharp, System.Void ScreenJoystick::FixedUpdate()");

//DO_APP_FUNC(void, MainMenuManager_Start, (MainMenuManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void MainMenuManager::Start()");

DO_APP_FUNC(void, MeetingHud_RpcClose, (MeetingHud* __this, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::RpcClose()");
DO_APP_FUNC(void, MeetingHud_Awake, (MeetingHud* __this, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::Awake()");
DO_APP_FUNC(void, MeetingHud_Close, (MeetingHud* __this, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::Close()");
DO_APP_FUNC(void, MeetingHud_Update, (MeetingHud* __this, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::Update()");
DO_APP_FUNC(void, MeetingHud_BloopAVoteIcon, (MeetingHud* __this, NetworkedPlayerInfo* voterPlayer, int index, Transform* parent, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::BloopAVoteIcon(NetworkedPlayerInfo, System.Int32, UnityEngine.Transform)");
DO_APP_FUNC(void, MeetingHud_PopulateResults, (MeetingHud* __this, Il2CppArraySize* states, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::PopulateResults(MeetingHud.VoterState[])");

DO_APP_FUNC(void, MovingPlatformBehaviour_SetSide, (MovingPlatformBehaviour* __this, bool isLeft, MethodInfo* method), "Assembly-CSharp, System.Void MovingPlatformBehaviour::SetSide(System.Boolean)");

DO_APP_FUNC(bool, NormalPlayerTask_get_IsComplete, (NormalPlayerTask* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean NormalPlayerTask::get_IsComplete()");
DO_APP_FUNC(void, NormalPlayerTask_NextStep, (NormalPlayerTask* __this, MethodInfo* method), "Assembly-CSharp, System.Void NormalPlayerTask::NextStep()");

DO_APP_FUNC(void, PlainDoor_SetDoorway, (PlainDoor* __this, bool open, MethodInfo* method), "Assembly-CSharp, System.Void PlainDoor::SetDoorway(System.Boolean)");
DO_APP_FUNC(void, DoorBreakerGame_Start, (DoorBreakerGame* __this, MethodInfo* method), "Assembly-CSharp, System.Void DoorBreakerGame::Start()");
DO_APP_FUNC(void, DoorCardSwipeGame_Begin, (DoorCardSwipeGame* __this, PlayerTask* playerTask, MethodInfo* method), "Assembly-CSharp, System.Void DoorCardSwipeGame::Begin(PlayerTask)");
DO_APP_FUNC(void, Minigame_Close, (Minigame* __this, MethodInfo* method), "Assembly-CSharp, System.Void Minigame::Close()");
DO_APP_FUNC(void, MushroomDoorSabotageMinigame_Begin, (MushroomDoorSabotageMinigame* __this, PlayerTask* task, MethodInfo* method), "Assembly-CSharp, System.Void MushroomDoorSabotageMinigame::Begin(PlayerTask)");
DO_APP_FUNC(void, MushroomDoorSabotageMinigame_SetDoor, (MushroomDoorSabotageMinigame* __this, OpenableDoor* door, MethodInfo* method), "Assembly-CSharp, System.Void MushroomDoorSabotageMinigame::SetDoor(OpenableDoor)");

DO_APP_FUNC(void, PlayerControl_Revive, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::Revive()");
DO_APP_FUNC(void, PlayerControl_CompleteTask, (PlayerControl* __this, uint32_t idx, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CompleteTask(System.UInt32)");
DO_APP_FUNC(void, PlayerControl_RpcCompleteTask, (PlayerControl* __this, uint32_t idx, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcCompleteTask(System.UInt32)");
DO_APP_FUNC(void, PlayerControl_FixedUpdate, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::FixedUpdate()");
DO_APP_FUNC(NetworkedPlayerInfo*, PlayerControl_get_Data, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, NetworkedPlayerInfo PlayerControl::get_Data()");
DO_APP_FUNC(Vector2, PlayerControl_GetTruePosition, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, UnityEngine.Vector2 PlayerControl::GetTruePosition()");
DO_APP_FUNC(void, PlayerControl_RpcSyncSettings, (PlayerControl* __this, Byte__Array* optionsByteArray, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSyncSettings(System.Byte[])");
DO_APP_FUNC(void, PlayerControl_RpcPlayAnimation, (PlayerControl* __this, uint8_t animType, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcPlayAnimation(System.Byte)");
DO_APP_FUNC(void, PlayerControl_PlayAnimation, (PlayerControl* __this, uint8_t animType, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::PlayAnimation(System.Byte)");
DO_APP_FUNC(void, PlayerControl_CmdReportDeadBody, (PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdReportDeadBody(NetworkedPlayerInfo)");
DO_APP_FUNC(void, PlayerControl_CmdCheckMurder, (PlayerControl* __this, PlayerControl* target, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckMurder(PlayerControl)");
DO_APP_FUNC(void, PlayerControl_CheckMurder, (PlayerControl* __this, PlayerControl* target, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CheckMurder(PlayerControl)");
DO_APP_FUNC(void, PlayerControl_MurderPlayer, (PlayerControl* __this, PlayerControl* target, MurderResultFlags__Enum resultFlags, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::MurderPlayer(PlayerControl, MurderResultFlags)");
DO_APP_FUNC(void, PlayerControl_RpcMurderPlayer, (PlayerControl* __this, PlayerControl* target, bool didSucceed, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcMurderPlayer(PlayerControl, System.Boolean)");
DO_APP_FUNC(void, PlayerControl_ReportDeadBody, (PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::ReportDeadBody(NetworkedPlayerInfo)");
DO_APP_FUNC(void, PlayerControl_StartMeeting, (PlayerControl* __this, NetworkedPlayerInfo* target, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::StartMeeting(NetworkedPlayerInfo)");
DO_APP_FUNC(void, PlayerControl_RpcStartMeeting, (PlayerControl* __this, NetworkedPlayerInfo* info, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcStartMeeting(NetworkedPlayerInfo)");
DO_APP_FUNC(void, PlayerControl_RpcSetRole, (PlayerControl* __this, RoleTypes__Enum roleType, bool canOverrideRole, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetRole(AmongUs.GameOptions.RoleTypes, System.Boolean)");
DO_APP_FUNC(void*, PlayerControl_CoSetRole, (PlayerControl* __this, RoleTypes__Enum role, bool canOverride, MethodInfo* method), "Assembly-CSharp, System.Collections.IEnumerator PlayerControl::CoSetRole(AmongUs.GameOptions.RoleTypes, System.Boolean)");
DO_APP_FUNC(void, PlayerControl_RpcSetScanner, (PlayerControl* __this, bool value, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetScanner(System.Boolean)");
DO_APP_FUNC(void, PlayerControl_SetScanner, (PlayerControl* __this, bool on, uint8_t cnt), "Assembly-CSharp, System.Void PlayerControl::SetScanner(System.Boolean, System.Byte)");
DO_APP_FUNC(void, PlayerControl_CmdCheckColor, (PlayerControl* __this, uint8_t bodyColor, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckColor(System.Byte)");
DO_APP_FUNC(void, PlayerControl_RpcSetColor, (PlayerControl* __this, uint8_t bodyColor, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetColor(System.Byte)");
DO_APP_FUNC(void, PlayerControl_CmdCheckName, (PlayerControl* __this, String* name, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckName(System.String)");
DO_APP_FUNC(void, PlayerControl_RpcSetLevel, (PlayerControl* __this, uint32_t level, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetLevel(System.UInt32)");
DO_APP_FUNC(void, PlayerControl_SetLevel, (PlayerControl* __this, uint32_t level, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::SetLevel(System.UInt32)");
DO_APP_FUNC(void, PlayerControl_RpcSetName, (PlayerControl* __this, String* name, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetName(System.String)");
DO_APP_FUNC(bool, PlayerControl_get_Visible, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerControl::get_Visible()");
DO_APP_FUNC(void, PlayerControl_set_Visible, (PlayerControl* __this, bool value, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::set_Visible(System.Boolean)");
DO_APP_FUNC(void, PlayerControl_HandleRpc, (PlayerControl* __this, uint8_t callId, MessageReader* reader, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::HandleRpc(System.Byte, Hazel.MessageReader)");
DO_APP_FUNC(void, PlayerControl_RpcSetPet, (PlayerControl* __this, String* petId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetPet(System.String)");
DO_APP_FUNC(void, PlayerControl_RpcSetSkin, (PlayerControl* __this, String* skinId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetSkin(System.String)");
DO_APP_FUNC(void, PlayerControl_RpcSetHat, (PlayerControl* __this, String* hatId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetHat(System.String)");
DO_APP_FUNC(void, PlayerControl_RpcSetVisor, (PlayerControl* __this, String* visorId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetVisor(System.String)");
DO_APP_FUNC(void, PlayerControl_RpcSetNamePlate, (PlayerControl* __this, String* namePlateId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSetNamePlate(System.String)");
DO_APP_FUNC(bool, PlayerControl_RpcSendChat, (PlayerControl* __this, String* message, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerControl::RpcSendChat(System.String)");
DO_APP_FUNC(void, PlayerControl_Shapeshift, (PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::Shapeshift(PlayerControl, System.Boolean)");
DO_APP_FUNC(void, PlayerControl_RpcShapeshift, (PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcShapeshift(PlayerControl, System.Boolean)");
DO_APP_FUNC(void, PlayerControl_CmdCheckShapeshift, (PlayerControl* __this, PlayerControl* target, bool animate, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckShapeshift(PlayerControl, System.Boolean)");
DO_APP_FUNC(void, PlayerControl_CmdCheckRevertShapeshift, (PlayerControl* __this, bool shouldAnimate, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckRevertShapeshift(System.Boolean)");
DO_APP_FUNC(void, PlayerControl_CmdCheckProtect, (PlayerControl* __this, PlayerControl* target, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckProtect(PlayerControl)");
DO_APP_FUNC(void, PlayerControl_RpcProtectPlayer, (PlayerControl* __this, PlayerControl* target, int32_t colorId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcProtectPlayer(PlayerControl, System.Int32)");
DO_APP_FUNC(void, PlayerControl_ProtectPlayer, (PlayerControl* __this, PlayerControl* target, int32_t colorId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::ProtectPlayer(PlayerControl, System.Int32)");
DO_APP_FUNC(void, PlayerMaterial_SetColors, (int32_t colorId, Renderer* rend, MethodInfo* method), "Assembly-CSharp, System.Void PlayerMaterial::SetColors(System.Int32, UnityEngine.Renderer)");
DO_APP_FUNC(void, PlayerMaterial_SetColors_1, (Color color, Renderer* rend, MethodInfo* method), "Assembly-CSharp, System.Void PlayerMaterial::SetColors(UnityEngine.Color, UnityEngine.Renderer)");
DO_APP_FUNC(void, PlayerControl_OnGameEnd, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::OnGameEnd()");
DO_APP_FUNC(bool, PlayerControl_get_CanMove, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerControl::get_CanMove()");
DO_APP_FUNC(void, PlayerControl_OnGameStart, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::OnGameStart()");

DO_APP_FUNC(void, PolusShipStatus_OnEnable, (PolusShipStatus* __this, MethodInfo* method), "Assembly-CSharp, System.Void PolusShipStatus::OnEnable()");

DO_APP_FUNC(float, ShipStatus_CalculateLightRadius, (ShipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method), "Assembly-CSharp, System.Single ShipStatus::CalculateLightRadius(NetworkedPlayerInfo)");
DO_APP_FUNC(void, ShipStatus_OnEnable, (ShipStatus* __this, MethodInfo* method), "Assembly-CSharp, System.Void ShipStatus::OnEnable()");
DO_APP_FUNC(void, ShipStatus_OnDestroy, (ShipStatus* __this, MethodInfo* method), "Assembly-CSharp, System.Void ShipStatus::OnDestroy()");
DO_APP_FUNC(void, ShipStatus_RpcCloseDoorsOfType, (ShipStatus* __this, SystemTypes__Enum type, MethodInfo* method), "Assembly-CSharp, System.Void ShipStatus::RpcCloseDoorsOfType(SystemTypes)");
DO_APP_FUNC(void, ShipStatus_RpcUpdateSystem, (ShipStatus* __this, SystemTypes__Enum systemType, uint8_t amount, MethodInfo* method), "Assembly-CSharp, System.Void ShipStatus::RpcUpdateSystem(SystemTypes, System.Byte)");
DO_APP_FUNC(void, ShipStatus_UpdateSystem, (ShipStatus* __this, SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount, MethodInfo* method), "Assembly-CSharp, System.Void ShipStatus::UpdateSystem(SystemTypes, PlayerControl, System.Byte)");

/*DO_APP_FUNC(float, StatsManager_get_BanPoints, (StatsManager* __this, MethodInfo* method), "Assembly-CSharp, System.Single StatsManager::get_BanPoints()");
DO_APP_FUNC(int32_t, StatsManager_get_BanMinutesLeft, (StatsManager* __this, MethodInfo* method), "Assembly-CSharp, System.Int32 StatsManager::get_BanMinutesLeft()");
DO_APP_FUNC(bool, StatsManager_get_AmBanned, (StatsManager* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean StatsManager::get_AmBanned()");*/
//DO_APP_FUNC(float, PlayerBanData_get_BanPoints, (PlayerBanData* __this, MethodInfo* method), "Assembly-CSharp, System.Single AmongUs.Data.Player.PlayerBanData::get_BanPoints()");
DO_APP_FUNC(int32_t, PlayerBanData_get_BanMinutesLeft, (PlayerBanData* __this, MethodInfo* method), "Assembly-CSharp, System.Int32 AmongUs.Data.Player.PlayerBanData::get_BanMinutesLeft()");
//DO_APP_FUNC(bool, PlayerBanData_get_IsBanned, (PlayerBanData* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean AmongUs.Data.Player.PlayerBanData::get_IsBanned()");

DO_APP_FUNC(float, Vent_CanUse, (Vent* __this, NetworkedPlayerInfo* player, bool* canUse, bool* couldUse, MethodInfo* method), "Assembly-CSharp, System.Single Vent::CanUse(NetworkedPlayerInfo, System.Boolean&, System.Boolean&)");
DO_APP_FUNC(float, Vent_get_UsableDistance, (Vent* __this, MethodInfo* method), "Assembly-CSharp, System.Single Vent::get_UsableDistance()");
DO_APP_FUNC(void, Vent_EnterVent, (Vent* __this, PlayerControl* pc, MethodInfo* method), "Assembly-CSharp, System.Void Vent::EnterVent(PlayerControl)");
DO_APP_FUNC(void*, Vent_ExitVent, (Vent* __this, PlayerControl* pc, MethodInfo* method), "Assembly-CSharp, System.Collections.IEnumerator Vent::ExitVent(PlayerControl)");
DO_APP_FUNC(void, VentilationSystem_Update, (VentilationSystem_Operation__Enum op, int32_t ventId, MethodInfo* method), "Assembly-CSharp, System.Void VentilationSystem::Update(VentilationSystem.Operation, System.Int32)");

DO_APP_FUNC(void, HudManager_Update, (HudManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void HudManager::Update()");
DO_APP_FUNC(void, HudManager_SetHudActive, (HudManager* __this, bool isActive, MethodInfo* method), "Assembly-CSharp, System.Void HudManager::SetHudActive(System.Boolean)");
DO_APP_FUNC(void, ChatController_AddChat, (ChatController* __this, PlayerControl* sourcePlayer, String* chatText, bool censor, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::AddChat(PlayerControl, System.String, System.Boolean)");
DO_APP_FUNC(void, ChatController_AddChatWarning, (ChatController* __this, String* warningText, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::AddChatWarning(System.String)");
DO_APP_FUNC(void, ChatController_SetVisible, (ChatController* __this, bool visible, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::SetVisible(System.Boolean)");
DO_APP_FUNC(void, ChatController_Update, (ChatController* __this, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::Update()");
DO_APP_FUNC(void, ChatBubble_SetName, (ChatBubble* __this, String* playerName, bool isDead, bool voted, Color color, MethodInfo* method), "Assembly-CSharp, System.Void ChatBubble::SetName(System.String, System.Boolean, System.Boolean, UnityEngine.Color)");

DO_APP_FUNC(void, AmongUsClient_OnGameJoined, (AmongUsClient* __this, String* gameIdString, MethodInfo* method), "Assembly-CSharp, System.Void AmongUsClient::OnGameJoined(System.String)");
DO_APP_FUNC(void, AmongUsClient_OnPlayerLeft, (AmongUsClient* __this, ClientData* data, DisconnectReasons__Enum reason, MethodInfo* method), "Assembly-CSharp, System.Void AmongUsClient::OnPlayerLeft(InnerNet.ClientData, DisconnectReasons)");
DO_APP_FUNC(bool, InnerNetClient_get_AmHost, (InnerNetClient* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean InnerNet.InnerNetClient::get_AmHost()");
DO_APP_FUNC(ClientData*, InnerNetClient_GetHost, (InnerNetClient* __this, MethodInfo* method), "Assembly-CSharp, InnerNet.ClientData InnerNet.InnerNetClient::GetHost()");
DO_APP_FUNC(ClientData*, InnerNetClient_GetClientFromCharacter, (InnerNetClient* __this, PlayerControl* character, MethodInfo* method), "Assembly-CSharp, InnerNet.ClientData InnerNet.InnerNetClient::GetClientFromCharacter(PlayerControl)");
DO_APP_FUNC(void, InnerNetClient_KickPlayer, (InnerNetClient* __this, int32_t clientId, bool ban, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::KickPlayer(System.Int32, System.Boolean)");
DO_APP_FUNC(void, InnerNetClient_SendStartGame, (InnerNetClient* __this, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::SendStartGame()");
DO_APP_FUNC(void, InnerNetClient_Update, (InnerNetClient* __this, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::Update()");
//DO_APP_FUNC(MessageWriter*, InnerNetClient_StartRpc, (InnerNetClient* __this, uint32_t targetNetId, uint8_t callId, SendOption__Enum option, MethodInfo* method), "Assembly-CSharp, Hazel.MessageWriter InnerNet.InnerNetClient::StartRpc(System.UInt32, System.Byte, Hazel.SendOption)");
// StartRpc was removed in v2021.4.20
DO_APP_FUNC(MessageWriter*, InnerNetClient_StartRpcImmediately, (InnerNetClient* __this, uint32_t targetNetId, uint8_t callId, SendOption__Enum option, int32_t targetClientId, MethodInfo* method), "Assembly-CSharp, Hazel.MessageWriter InnerNet.InnerNetClient::StartRpcImmediately(System.UInt32, System.Byte, Hazel.SendOption, System.Int32)");
DO_APP_FUNC(void, InnerNetClient_FinishRpcImmediately, (InnerNetClient* __this, MessageWriter* msg, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::FinishRpcImmediately(Hazel.MessageWriter)");

DO_APP_FUNC(void, MessageExtensions_WriteNetObject, (MessageWriter* self, InnerNetObject* obj, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.MessageExtensions::WriteNetObject(Hazel.MessageWriter, InnerNet.InnerNetObject)");

DO_APP_FUNC(bool, AprilFoolsMode_ShouldFlipSkeld, (MethodInfo* method), "Assembly-CSharp, System.Boolean AprilFoolsMode::ShouldFlipSkeld()");
//this causes issues with dleks
//DO_APP_FUNC(bool, Constants_1_ShouldHorseAround, (MethodInfo* method), "Assembly-CSharp, System.Boolean Constants::ShouldHorseAround()");
DO_APP_FUNC(Platforms__Enum, Constants_1_GetPlatformType, (MethodInfo* method), "Assembly-CSharp, Platforms Constants::GetPlatformType()");
DO_APP_FUNC(int32_t, Constants_1_GetBroadcastVersion, (MethodInfo* method), "Assembly-CSharp, System.Int32 Constants::GetBroadcastVersion()");
DO_APP_FUNC(bool, Constants_1_IsVersionModded, (MethodInfo* method), "Assembly-CSharp, System.Boolean Constants::IsVersionModded()");
//DO_APP_FUNC(PlatformSpecificData*, Constants_1_GetPlatformData, (MethodInfo * method), "");
DO_APP_FUNC(void, PlatformSpecificData_Serialize, (PlatformSpecificData* __this, MessageWriter* writer, MethodInfo* method), "Assembly-CSharp, System.Void PlatformSpecificData::Serialize(Hazel.MessageWriter)");

DO_APP_FUNC(void, LobbyBehaviour_Start, (LobbyBehaviour* __this, MethodInfo* method), "Assembly-CSharp, System.Void LobbyBehaviour::Start()");
DO_APP_FUNC(void, LobbyBehaviour_Update, (LobbyBehaviour* __this, MethodInfo* method), "Assembly-CSharp, System.Void LobbyBehaviour::Update()");

DO_APP_FUNC(MessageWriter*, MessageWriter_Get, (SendOption__Enum sendOption, MethodInfo* method), "Hazel, Hazel.MessageWriter Hazel.MessageWriter::Get(Hazel.SendOption)");
DO_APP_FUNC(void, MessageWriter_StartMessage, (MessageWriter* __this, uint8_t typeFlag, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::StartMessage(System.Byte)");
DO_APP_FUNC(void, MessageWriter_WritePacked, (MessageWriter* __this, int32_t value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::WritePacked(System.Int32)");

DO_APP_FUNC(bool, MessageReader_ReadBoolean, (MessageReader* __this, MethodInfo* method), "Hazel, System.Boolean Hazel.MessageReader::ReadBoolean()");
DO_APP_FUNC(uint8_t, MessageReader_ReadByte, (MessageReader* __this, MethodInfo* method), "Hazel, System.Byte Hazel.MessageReader::ReadByte()");
DO_APP_FUNC(uint32_t, MessageReader_ReadUInt32, (MessageReader* __this, MethodInfo* method), "Hazel, System.UInt32 Hazel.MessageReader::ReadUInt32()");
DO_APP_FUNC(int32_t, MessageReader_ReadInt32, (MessageReader* __this, MethodInfo* method), "Hazel, System.Int32 Hazel.MessageReader::ReadInt32()");
DO_APP_FUNC(float, MessageReader_ReadSingle, (MessageReader* __this, MethodInfo* method), "Hazel, System.Single Hazel.MessageReader::ReadSingle()");
DO_APP_FUNC(String*, MessageReader_ReadString, (MessageReader* __this, MethodInfo* method), "Hazel, System.String Hazel.MessageReader::ReadString()");
DO_APP_FUNC(Byte__Array*, MessageReader_ReadBytesAndSize, (MessageReader* __this, MethodInfo* method), "Hazel, System.Byte[] Hazel.MessageReader::ReadBytesAndSize()");
DO_APP_FUNC(Byte__Array*, MessageReader_ReadBytes, (MessageReader* __this, int32_t length, MethodInfo* method), "Hazel, System.Byte[] Hazel.MessageReader::ReadBytes(System.Int32)");
DO_APP_FUNC(int32_t, MessageReader_get_BytesRemaining, (MessageReader* __this, MethodInfo* method), "Hazel, System.Int32 Hazel.MessageReader::get_BytesRemaining()");
DO_APP_FUNC(void, MessageWriter_WriteBoolean, (MessageWriter* __this, bool value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.Boolean)");
DO_APP_FUNC(void, MessageWriter_WriteByte, (MessageWriter* __this, uint8_t value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.Byte)");
DO_APP_FUNC(void, MessageWriter_WriteSByte, (MessageWriter* __this, int8_t value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.SByte)");
DO_APP_FUNC(void, MessageWriter_WriteUShort, (MessageWriter* __this, uint16_t value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.UInt16)");
DO_APP_FUNC(void, MessageWriter_WriteInt32, (MessageWriter* __this, int32_t value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.Int32)");
DO_APP_FUNC(void, MessageWriter_WriteSingle, (MessageWriter* __this, float value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.Single)");
DO_APP_FUNC(void, MessageWriter_WriteString, (MessageWriter* __this, String* value, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.String)");
DO_APP_FUNC(void, MessageWriter_WriteBytesAndSize, (MessageWriter* __this, Byte__Array* bytes, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.Byte[])");
DO_APP_FUNC(void, MessageWriter_WriteByteArray, (MessageWriter* __this, Byte__Array* bytes, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Write(System.Byte[])");
DO_APP_FUNC(void, MessageWriter_EndMessage, (MessageWriter* __this, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::EndMessage()");
DO_APP_FUNC(void, NetHelpers_WriteVector2, (Vector2 vec, MessageWriter* writer, MethodInfo* method), "Assembly-CSharp, System.Void NetHelpers::WriteVector2(UnityEngine.Vector2, Hazel.MessageWriter)");

DO_APP_FUNC(void, AccountTab_Open, (AccountTab* __this, MethodInfo* method), "Assembly-CSharp, System.Void AccountTab::Open()");
DO_APP_FUNC(void, FullAccount_CanSetCustomName, (FullAccount* __this, bool canSetName, MethodInfo* method), "Assembly-CSharp, System.Void FullAccount::CanSetCustomName(System.Boolean)");
DO_APP_FUNC(void, FollowerCamera_Update, (FollowerCamera* __this, MethodInfo* method), "Assembly-CSharp, System.Void FollowerCamera::Update()");
DO_APP_FUNC(void, AmongUsClient_OnGameEnd, (AmongUsClient* __this, EndGameResult* endGameResult, MethodInfo* method), "Assembly-CSharp, System.Void AmongUsClient::OnGameEnd(EndGameResult)");

DO_APP_FUNC(void, Debug_Log, (Object* message, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Debug::Log(System.Object)");
DO_APP_FUNC(void, Debug_LogError, (Object* message, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Debug::LogError(System.Object)");
DO_APP_FUNC(void, Debug_LogException, (Exception* exception, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Debug::LogException(System.Exception)");
DO_APP_FUNC(void, Debug_LogWarning, (Object* message, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Debug::LogWarning(System.Object)");
DO_APP_FUNC(String*, Object_ToString, (Object* __this, MethodInfo* method), "mscorlib, System.String System.Object::ToString()");
DO_APP_FUNC(void, VersionShower_Start, (VersionShower* __this, MethodInfo* method), "Assembly-CSharp, System.Void VersionShower::Start()");

DO_APP_FUNC(void, EOSManager_StartInitialLoginFlow, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::StartInitialLoginFlow()");
DO_APP_FUNC(void, EOSManager_LoginFromAccountTab, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::LoginFromAccountTab()");
DO_APP_FUNC(bool, EOSManager_HasFinishedLoginFlow, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean EOSManager::HasFinishedLoginFlow()");
DO_APP_FUNC(void, EOSManager_InitializePlatformInterface, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::InitializePlatformInterface()");
DO_APP_FUNC(bool, EOSManager_IsFreechatAllowed, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean EOSManager::IsFreechatAllowed()");
DO_APP_FUNC(void, EOSManager_UpdatePermissionKeys, (EOSManager* __this, void* callback, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::UpdatePermissionKeys(System.Action)");
DO_APP_FUNC(void, EOSManager_DeleteDeviceID, (EOSManager* __this, void* callback, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::DeleteDeviceID(System.Action)");
DO_APP_FUNC(void, EOSManager_Update, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::Update()");
DO_APP_FUNC(bool, EOSManager_IsFriendsListAllowed, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean EOSManager::IsFriendsListAllowed()");
DO_APP_FUNC(String*, EOSManager_get_ProductUserId, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.String EOSManager::get_ProductUserId()");
DO_APP_FUNC(void, EOSManager_StartTempAccountFlow, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::StartTempAccountFlow()");
DO_APP_FUNC(void, EOSManager_CloseStartupWaitScreen, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::CloseStartupWaitScreen()");
DO_APP_FUNC(void, PlayerAccountData_set_LoginStatus, (PlayerAccountData* __this, EOSManager_AccountLoginStatus__Enum value, MethodInfo* method), "Assembly-CSharp, System.Void AmongUs.Data.Player.PlayerAccountData::set_LoginStatus(EOSManager.AccountLoginStatus)");
DO_APP_FUNC(void, EditAccountUsername_SaveUsername, (EditAccountUsername* __this, MethodInfo* method), "Assembly-CSharp, System.Void EditAccountUsername::SaveUsername()");

DO_APP_FUNC(void, TextMeshPro_SetFaceColor, (TextMeshPro* __this, Color32 color, MethodInfo* method), "Unity.TextMeshPro, System.Void TMPro.TextMeshPro::SetFaceColor(UnityEngine.Color32)");
DO_APP_FUNC(void, TextMeshPro_SetOutlineColor, (TextMeshPro* __this, Color32 color, MethodInfo* method), "Unity.TextMeshPro, System.Void TMPro.TextMeshPro::SetOutlineColor(UnityEngine.Color32)");
DO_APP_FUNC(void, TMP_Text_set_alignment, (TMP_Text* __this, TextAlignmentOptions__Enum value, MethodInfo* method), "Unity.TextMeshPro, System.Void TMPro.TMP_Text::set_alignment(TMPro.TextAlignmentOptions)");
DO_APP_FUNC(String*, TMP_Text_get_text, (TMP_Text* __this, MethodInfo* method), "Unity.TextMeshPro, System.String TMPro.TMP_Text::get_text()");
DO_APP_FUNC(void, TMP_Text_set_text, (TMP_Text* __this, String* value, MethodInfo* method), "Unity.TextMeshPro, System.Void TMPro.TMP_Text::set_text(System.String)");
DO_APP_FUNC(Color32, Color32_op_Implicit, (Color c, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Color32 UnityEngine.Color32::op_Implicit(UnityEngine.Color)");
DO_APP_FUNC(Color, Color32_op_Implicit_1, (Color32 c, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Color UnityEngine.Color32::op_Implicit(UnityEngine.Color32)");

DO_APP_FUNC(void, RoleManager_SelectRoles, (RoleManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void RoleManager::SelectRoles()");
//DO_APP_FUNC(void, RoleManager_AssignRolesForTeam, (List_1_GameData_PlayerInfo_* players, RoleOptionsData* opts, RoleTeamTypes__Enum team, int32_t teamMax, Nullable_1_RoleTypes_ defaultRole, MethodInfo* method), "Assembly-CSharp, System.Void RoleManager::AssignRolesForTeam(System.Collections.Generic.List<GameData.PlayerInfo>, RoleOptionsData, RoleTeamTypes, System.Int32, System.Nullable<RoleTypes>)");
//DO_APP_FUNC(void, RoleManager_AssignRolesFromList, (List_1_GameData_PlayerInfo_* players, int32_t teamMax, List_1_RoleTypes_* roleList, int32_t* rolesAssigned, MethodInfo* method), "Assembly-CSharp, System.Void RoleManager::AssignRolesFromList(System.Collections.Generic.List<GameData.PlayerInfo>, System.Int32, System.Collections.Generic.List<RoleTypes>, System.Int32&)");
DO_APP_FUNC(void, RoleManager_SetRole, (RoleManager* __this, PlayerControl* targetPlayer, RoleTypes__Enum roleType, MethodInfo* method), "Assembly-CSharp, System.Void RoleManager::SetRole(PlayerControl, AmongUs.GameOptions.RoleTypes)");
DO_APP_FUNC(void, InnerNetClient_EnqueueDisconnect, (InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::EnqueueDisconnect(DisconnectReasons, System.String)");
DO_APP_FUNC(void, InnerNetClient_DisconnectInternal, (InnerNetClient* __this, DisconnectReasons__Enum reason, String* stringReason, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::DisconnectInternal(DisconnectReasons, System.String)");

DO_APP_FUNC(void, PlayerPhysics_FixedUpdate, (PlayerPhysics* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerPhysics::FixedUpdate()");
DO_APP_FUNC(void, PlayerPhysics_RpcEnterVent, (PlayerPhysics* __this, int32_t id, MethodInfo* method), "Assembly-CSharp, System.Void PlayerPhysics::RpcEnterVent(System.Int32)");
DO_APP_FUNC(void, PlayerPhysics_RpcExitVent, (PlayerPhysics* __this, int32_t id, MethodInfo* method), "Assembly-CSharp, System.Void PlayerPhysics::RpcExitVent(System.Int32)");
DO_APP_FUNC(void, PlayerPhysics_RpcBootFromVent, (PlayerPhysics* __this, int32_t ventId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerPhysics::RpcBootFromVent(System.Int32)");

DO_APP_FUNC(void, PlayerControl_TurnOnProtection, (PlayerControl* __this, bool visible, int32_t colorId, int32_t guardianPlayerId, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::TurnOnProtection(System.Boolean, System.Int32, System.Int32)");
DO_APP_FUNC(void, PlayerControl_RemoveProtection, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RemoveProtection()");

DO_APP_FUNC(bool, Object_1_op_Implicit, (Object_1* exists, MethodInfo* method), "UnityEngine.CoreModule, System.Boolean UnityEngine.Object::op_Implicit(UnityEngine.Object)");
DO_APP_FUNC(void, PlayerControl_ShowFailedMurder, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::ShowFailedMurder()");

DO_APP_FUNC(bool, PlayerControl_get_IsKillTimerEnabled, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerControl::get_IsKillTimerEnabled()");
DO_APP_FUNC(void, ExileController_ReEnableGameplay, (ExileController* __this, MethodInfo* method), "Assembly-CSharp, System.Void ExileController::ReEnableGameplay()");
DO_APP_FUNC(void, ExileController_BeginForGameplay, (ExileController* __this, NetworkedPlayerInfo* exiled, bool voteTie, MethodInfo* method), "Assembly-CSharp, System.Void ExileController::BeginForGameplay(NetworkedPlayerInfo, System.Boolean)");
DO_APP_FUNC(void, SabotageSystemType_SetInitialSabotageCooldown, (SabotageSystemType* __this, MethodInfo* method), "Assembly-CSharp, System.Void SabotageSystemType::SetInitialSabotageCooldown()");

DO_APP_FUNC(PlayerControl*, NetworkedPlayerInfo_get_Object, (NetworkedPlayerInfo* __this, MethodInfo* method), "Assembly-CSharp, PlayerControl NetworkedPlayerInfo::get_Object()");
DO_APP_FUNC(NetworkedPlayerInfo_PlayerOutfit*, NetworkedPlayerInfo_get_DefaultOutfit, (NetworkedPlayerInfo* __this, MethodInfo* method), "Assembly-CSharp, NetworkedPlayerInfo.PlayerOutfit NetworkedPlayerInfo::get_DefaultOutfit()");
DO_APP_FUNC(String*, NetworkedPlayerInfo_get_PlayerName, (NetworkedPlayerInfo* __this, MethodInfo* method), "Assembly-CSharp, System.String NetworkedPlayerInfo::get_PlayerName()");

DO_APP_FUNC(void, AccountManager_UpdateKidAccountDisplay, (AccountManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void AccountManager::UpdateKidAccountDisplay()");
DO_APP_FUNC(String*, AccountManager_GetRandomName, (AccountManager* __this, MethodInfo* method), "Assembly-CSharp, System.String AccountManager::GetRandomName()");
DO_APP_FUNC(void, PlayerStorageManager_OnReadPlayerPrefsComplete, (PlayerStorageManager* __this, void* data, MethodInfo* method), "Assembly-CSharp, System.Void PlayerStorageManager::OnReadPlayerPrefsComplete(Epic.OnlineServices.PlayerDataStorage.ReadFileCallbackInfo&)");

DO_APP_FUNC(void, AchievementManager_1_UnlockAchievement, (AchievementManager_1* __this, String* key, MethodInfo* method), "Assembly-CSharp, System.Void AchievementManager::UnlockAchievement(System.String)");

// 2022.10.25s
DO_APP_FUNC(PlayerData*, DataManager_get_Player, (MethodInfo* method), "Assembly-CSharp, AmongUs.Data.Player.PlayerData AmongUs.Data.DataManager::get_Player()");
DO_APP_FUNC(String*, PlayerCustomizationData_get_Name, (PlayerCustomizationData* __this, MethodInfo* method), "Assembly-CSharp, System.String AmongUs.Data.Player.PlayerCustomizationData::get_Name()");
DO_APP_FUNC(void, PlayerCustomizationData_set_Name, (PlayerCustomizationData* __this, String* value, MethodInfo* method), "Assembly-CSharp, System.Void AmongUs.Data.Player.PlayerCustomizationData::set_Name(System.String)");
DO_APP_FUNC(bool, PlayerPurchasesData_GetPurchase, (PlayerPurchasesData* __this, String* itemKey, String* bundleKey, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerPurchasesData::GetPurchase(System.String, System.String)");
DO_APP_FUNC(SettingsData*, DataManager_get_Settings, (MethodInfo* method), "Assembly-CSharp, AmongUs.Data.Settings.SettingsData AmongUs.Data.DataManager::get_Settings()");
DO_APP_FUNC(AccessibilitySettingsData*, SettingsData_get_Accessibility, (SettingsData* __this, MethodInfo* method), "Assembly-CSharp, AmongUs.Data.Settings.AccessibilitySettingsData AmongUs.Data.Settings.SettingsData::get_Accessibility()");
DO_APP_FUNC(MultiplayerSettingsData*, SettingsData_get_Multiplayer, (SettingsData* __this, MethodInfo* method), "Assembly-CSharp, AmongUs.Data.Settings.MultiplayerSettingsData AmongUs.Data.Settings.SettingsData::get_Multiplayer()");
DO_APP_FUNC(GameplaySettingsData*, SettingsData_get_Gameplay, (SettingsData* __this, MethodInfo* method), "Assembly-CSharp, AmongUs.Data.Settings.GameplaySettingsData AmongUs.Data.Settings.SettingsData::get_Gameplay()");
DO_APP_FUNC(bool, AccessibilitySettingsData_get_ColorBlindMode, (AccessibilitySettingsData* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean AmongUs.Data.Settings.AccessibilitySettingsData::get_ColorBlindMode()");
DO_APP_FUNC(bool, GameplaySettingsData_get_StreamerMode, (GameplaySettingsData* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean AmongUs.Data.Settings.GameplaySettingsData::get_StreamerMode()");
DO_APP_FUNC(bool, MultiplayerSettingsData_get_CensorChat, (MultiplayerSettingsData* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean AmongUs.Data.Settings.MultiplayerSettingsData::get_CensorChat()");

// 2022.12.08e
DO_APP_FUNC(GameOptionsManager*, GameOptionsManager_get_Instance, (MethodInfo* method), "Assembly-CSharp, GameOptionsManager GameOptionsManager::get_Instance()");
DO_APP_FUNC(bool, GameOptionsManager_get_HasOptions, (GameOptionsManager* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean GameOptionsManager::get_HasOptions()");
DO_APP_FUNC(IGameOptions*, GameOptionsManager_get_CurrentGameOptions, (GameOptionsManager* __this, MethodInfo* method), "Assembly-CSharp, AmongUs.GameOptions.IGameOptions GameOptionsManager::get_CurrentGameOptions()");
DO_APP_FUNC(void, GameOptionsManager_set_CurrentGameOptions, (GameOptionsManager* __this, IGameOptions* value, MethodInfo* method), "Assembly-CSharp, System.Void GameOptionsManager::set_CurrentGameOptions(AmongUs.GameOptions.IGameOptions)");
DO_APP_FUNC(GameManager*, GameManager_get_Instance, (MethodInfo* method), "Assembly-CSharp, GameManager GameManager::get_Instance()");
DO_APP_FUNC(LogicOptions*, GameManager_get_LogicOptions, (GameManager* __this, MethodInfo* method), "Assembly-CSharp, LogicOptions GameManager::get_LogicOptions()");

//adding kick/vote functions; these may have to be updated every time the game updates
DO_APP_FUNC(void, MeetingHud_CmdCastVote, (MeetingHud* __this, uint8_t playerId, uint8_t suspectIdx, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::CmdCastVote(System.Byte, System.Byte)");
DO_APP_FUNC(void, MeetingHud_CastVote, (MeetingHud* __this, uint8_t playerId, uint8_t suspectIdx, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::CastVote(System.Byte, System.Byte)");
DO_APP_FUNC(void, MeetingHud_RpcClearVote, (MeetingHud* __this, int32_t clientId, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::RpcClearVote(System.Int32)");
DO_APP_FUNC(void, VoteBanSystem_CmdAddVote, (VoteBanSystem* __this, int32_t clientId, MethodInfo* method), "Assembly-CSharp, System.Void VoteBanSystem::CmdAddVote(System.Int32)");
DO_APP_FUNC(void, VoteBanSystem_AddVote, (VoteBanSystem* __this, int32_t srcClient, int32_t clientId, MethodInfo* method), "Assembly-CSharp, System.Void VoteBanSystem::AddVote(System.Int32, System.Int32)");
DO_APP_FUNC(void, GameStartManager_Update, (GameStartManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void GameStartManager::Update()");
DO_APP_FUNC(void, PingTracker_Update, (PingTracker* __this, MethodInfo* method), "Assembly-CSharp, System.Void PingTracker::Update()");
DO_APP_FUNC(String*, InnerNet_GameCode_IntToGameName, (int32_t gameId, MethodInfo* method), "Assembly-CSharp, System.String InnerNet.GameCode::IntToGameNameV2(System.Int32)");
DO_APP_FUNC(bool, TextBoxTMP_IsCharAllowed, (TextBoxTMP* __this, uint16_t i, MethodInfo* method), "Assembly-CSharp, System.Boolean TextBoxTMP::IsCharAllowed(System.Char)");
DO_APP_FUNC(void, TextBoxTMP_SetText, (TextBoxTMP* __this, String* input, String* inputCompo, MethodInfo* method), "Assembly-CSharp, System.Void TextBoxTMP::SetText(System.String, System.String)");
DO_APP_FUNC(void, GameManager_RpcEndGame, (GameManager* __this, GameOverReason__Enum endReason, bool showAd, MethodInfo* method), "Assembly-CSharp, System.Void GameManager::RpcEndGame(GameOverReason, System.Boolean)");
DO_APP_FUNC(void, KillOverlay_ShowKillAnimation_1, (KillOverlay* __this, NetworkedPlayerInfo* killer, NetworkedPlayerInfo* victim, MethodInfo* method), "Assembly-CSharp, System.Void KillOverlay::ShowKillAnimation(NetworkedPlayerInfo, NetworkedPlayerInfo)");
DO_APP_FUNC(String*, EOSManager_get_FriendCode, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.String EOSManager::get_FriendCode()");
//DO_APP_FUNC(void, EOSManager_set_FriendCode, (EOSManager* __this, String* value, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::set_FriendCode(System.String)"); crashes epic games when run
DO_APP_FUNC(float, LogicOptions_GetKillDistance, (LogicOptions* __this, MethodInfo* method), "Assembly-CSharp, System.Single LogicOptions::GetKillDistance()");
DO_APP_FUNC(bool, LogicOptions_GetAnonymousVotes, (LogicOptions* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean LogicOptions::GetAnonymousVotes()");
//DO_APP_FUNC(TaskBarMode__Enum, LogicOptions_GetTaskBarMode, (LogicOptions* __this, MethodInfo* method), "Assembly-CSharp, TaskbarMode LogicOptions::GetTaskBarMode()");
DO_APP_FUNC(void, KillButton_SetTarget, (KillButton* __this, PlayerControl* target, MethodInfo* method), "Assembly-CSharp, System.Void KillButton::SetTarget(PlayerControl)");
DO_APP_FUNC(PlayerControl*, ImpostorRole_FindClosestTarget, (ImpostorRole* __this, MethodInfo* method), "Assembly-CSharp, PlayerControl ImpostorRole::FindClosestTarget()");
DO_APP_FUNC(AsyncOperationHandle_1_UnityEngine_GameObject_, AssetReference_InstantiateAsync_1, (AssetReference* __this, Transform* parent, bool instantiateInWorldSpace, MethodInfo* method), "Unity.Addressables, UnityEngine.ResourceManagement.AsyncOperations.AsyncOperationHandle<UnityEngine.GameObject> UnityEngine.AddressableAssets.AssetReference::InstantiateAsync(UnityEngine.Transform, System.Boolean)");
DO_APP_FUNC(float, Console_CanUse, (Console* __this, NetworkedPlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method), "Assembly-CSharp, System.Single Console::CanUse(NetworkedPlayerInfo, System.Boolean&, System.Boolean&)");
DO_APP_FUNC(void, Ladder_SetDestinationCooldown, (Ladder* __this, MethodInfo* method), "Assembly-CSharp, System.Void Ladder::SetDestinationCooldown()");
DO_APP_FUNC(void, ZiplineConsole_SetDestinationCooldown, (ZiplineConsole* __this, MethodInfo* method), "Assembly-CSharp, System.Void ZiplineConsole::SetDestinationCooldown()");
DO_APP_FUNC(void, MushroomWallDoor_SetDoorway, (MushroomWallDoor* __this, bool open, MethodInfo* method), "Assembly-CSharp, System.Void MushroomWallDoor::SetDoorway(System.Boolean)");
//DO_APP_FUNC(void, ActivityManager_UpdateActivity, (ActivityManager* __this, Activity_1 activity, void* callback, MethodInfo* method), "Assembly-CSharp, System.Void Discord.ActivityManager::UpdateActivity(Discord.Activity, Discord.ActivityManager.UpdateActivityHandler)");
DO_APP_FUNC(bool, LogicGameFlowNormal_IsGameOverDueToDeath, (LogicGameFlowNormal* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean LogicGameFlowNormal::IsGameOverDueToDeath()");
DO_APP_FUNC(bool, LogicGameFlowHnS_IsGameOverDueToDeath, (LogicGameFlowHnS* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean LogicGameFlowHnS::IsGameOverDueToDeath()");
DO_APP_FUNC(void, ChatController_OnResolutionChanged, (ChatController* __this, float aspectRatio, int32_t width, int32_t height, bool fullscreen, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::OnResolutionChanged(System.Single, System.Int32, System.Int32, System.Boolean)");
DO_APP_FUNC(void, ChatController_ForceClosed, (ChatController* __this, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::ForceClosed()");
DO_APP_FUNC(void, ChatController_SendFreeChat, (ChatController* __this, MethodInfo* method), "Assembly-CSharp, System.Void ChatController::SendFreeChat()");
DO_APP_FUNC(Byte__Array*, GameOptionsFactory_ToBytes, (GameOptionsFactory* __this, IGameOptions* data, bool forceAprilFoolsMode, MethodInfo* method), "Assembly-CSharp, System.Byte[] AmongUs.GameOptions.GameOptionsFactory::ToBytes(AmongUs.GameOptions.IGameOptions, System.Boolean)");
DO_APP_FUNC(void, NotificationPopper_AddDisconnectMessage, (NotificationPopper* __this, String* item, MethodInfo* method), "Assembly-CSharp, System.Void NotificationPopper::AddDisconnectMessage(System.String)");
//DO_APP_FUNC(bool, GameData_Serialize, (GameData* __this, MessageWriter* writer, bool initialState, MethodInfo* method), "Assembly-CSharp, System.Boolean GameData::Serialize(Hazel.MessageWriter, System.Boolean)");
DO_APP_FUNC(void, InnerNetClient_SendOrDisconnect, (InnerNetClient* __this, MessageWriter* msg, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::SendOrDisconnect(Hazel.MessageWriter)");
DO_APP_FUNC(void, MessageWriter_Recycle, (MessageWriter* __this, MethodInfo* method), "Hazel, System.Void Hazel.MessageWriter::Recycle()");
DO_APP_FUNC(void, ClipboardHelper_PutClipboardString, (String* str, MethodInfo* method), "Assembly-CSharp, System.Void ClipboardHelper::PutClipboardString(System.String)");
DO_APP_FUNC(void, GameOptionsManager_set_GameHostOptions, (GameOptionsManager* __this, IGameOptions* value, MethodInfo* method), "Assembly-CSharp, System.Void GameOptionsManager::set_GameHostOptions(AmongUs.GameOptions.IGameOptions)");
DO_APP_FUNC(void, LogicOptions_SyncOptions, (LogicOptions* __this, MethodInfo* method), "Assembly-CSharp, System.Void LogicOptions::SyncOptions()");
DO_APP_FUNC(void, EOSManager_PlayOffline, (EOSManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void EOSManager::PlayOffline()");
DO_APP_FUNC(void, DisconnectPopup_DoShow, (DisconnectPopup* __this, MethodInfo* method), "Assembly-CSharp, System.Void DisconnectPopup::DoShow()");
DO_APP_FUNC(bool, NetworkedPlayerInfo_Serialize, (NetworkedPlayerInfo* __this, MessageWriter* writer, bool initialState, MethodInfo* method), "Assembly-CSharp, System.Boolean NetworkedPlayerInfo::Serialize(Hazel.MessageWriter, System.Boolean)");
DO_APP_FUNC(void, NetworkedPlayerInfo_Deserialize, (NetworkedPlayerInfo* __this, MessageReader* reader, bool initialState, MethodInfo* method), "Assembly-CSharp, System.Void NetworkedPlayerInfo::Deserialize(Hazel.MessageReader, System.Boolean)");
DO_APP_FUNC(bool, GameManager_DidImpostorsWin, (GameManager* __this, GameOverReason__Enum reason, MethodInfo* method), "Assembly-CSharp, System.Boolean GameManager::DidImpostorsWin(GameOverReason)");
DO_APP_FUNC(void, AmongUsClient_OnPlayerJoined, (AmongUsClient* __this, ClientData* data, MethodInfo* method), "Assembly-CSharp, System.Void AmongUsClient::OnPlayerJoined(InnerNet.ClientData)");
DO_APP_FUNC(int32_t, GameCode_GameNameToInt, (String* gameId, MethodInfo* method), "Assembly-CSharp, System.Int32 InnerNet.GameCode::GameNameToIntV2(System.String)");
DO_APP_FUNC(PlayerStatsData*, PlayerData_get_Stats, (PlayerData* __this, MethodInfo* method), "Assembly-CSharp, AmongUs.Data.Player.PlayerStatsData AmongUs.Data.Player.PlayerData::get_Stats()");
DO_APP_FUNC(void, AbstractSaveData_Save, (AbstractSaveData* __this, MethodInfo* method), "Assembly-CSharp, System.Void AmongUs.Data.AbstractSaveData::Save()");
DO_APP_FUNC(void, ShipStatus_HandleRpc, (ShipStatus* __this, uint8_t callId, MessageReader* reader, MethodInfo* method), "Assembly-CSharp, System.Void ShipStatus::HandleRpc(System.Byte, Hazel.MessageReader)");
DO_APP_FUNC(void, TMP_Text_set_color, (TMP_Text* __this, Color value, MethodInfo* method), "Unity.TextMeshPro, System.Void TMPro.TMP_Text::set_color(UnityEngine.Color)");
DO_APP_FUNC(void, ChatBubble_SetText, (ChatBubble* __this, String* chatText, MethodInfo* method), "Assembly-CSharp, System.Void ChatBubble::SetText(System.String)");
DO_APP_FUNC(void, Mushroom_FixedUpdate, (Mushroom* __this, MethodInfo* method), "Assembly-CSharp, System.Void Mushroom::FixedUpdate()");
DO_APP_FUNC(void, Screen_SetResolution_1, (int32_t width, int32_t height, bool fullscreen, int32_t preferredRefreshRate, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Screen::SetResolution(System.Int32, System.Int32, System.Boolean, System.Int32)");
DO_APP_FUNC(void, GameData_ShowNotification, (GameData* __this, String* playerName, DisconnectReasons__Enum reason, MethodInfo* method), "Assembly-CSharp, System.Void GameData::ShowNotification(System.String, DisconnectReasons)");
DO_APP_FUNC(void, PlayerControl_CmdCheckVanish, (PlayerControl* __this, float maxDuration, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckVanish(System.Single)");
DO_APP_FUNC(void, PlayerControl_RpcVanish, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcVanish()");
DO_APP_FUNC(void, PlayerControl_CmdCheckAppear, (PlayerControl* __this, bool shouldAnimate, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::CmdCheckAppear(System.Boolean)");
DO_APP_FUNC(void, PlayerControl_RpcAppear, (PlayerControl* __this, bool shouldAnimate, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcAppear(System.Boolean)");
DO_APP_FUNC(void, PlayerControl_SetRoleInvisibility, (PlayerControl* __this, bool isActive, bool shouldAnimate, bool playFullAnimation, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::SetRoleInvisibility(System.Boolean, System.Boolean, System.Boolean)");
DO_APP_FUNC(void, PlayerControl_RpcSendChatNote, (PlayerControl* __this, uint8_t srcPlayerId, ChatNoteTypes__Enum noteType, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::RpcSendChatNote(System.Byte, ChatNoteTypes)");
DO_APP_FUNC(void, MeetingHud_RpcVotingComplete, (MeetingHud* __this, MeetingHud_VoterState__Array* states, NetworkedPlayerInfo* exiled, bool tie, MethodInfo* method), "Assembly-CSharp, System.Void MeetingHud::RpcVotingComplete(MeetingHud.VoterState[], NetworkedPlayerInfo, System.Boolean)");
DO_APP_FUNC(bool, AccountManager_CanPlayOnline, (AccountManager* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean AccountManager::CanPlayOnline()");
DO_APP_FUNC(void, SoundManager_StopSound, (SoundManager* __this, AudioClip* clip, MethodInfo* method), "Assembly-CSharp, System.Void SoundManager::StopSound(UnityEngine.AudioClip)");
DO_APP_FUNC(void, Application_set_targetFrameRate, (int32_t value, MethodInfo* method), "UnityEngine.CoreModule, System.Void UnityEngine.Application::set_targetFrameRate(System.Int32)");
DO_APP_FUNC(Object*, Object_1_Instantiate, (Object* original, MethodInfo* method), "UnityEngine.CoreModule, UnityEngine.Object UnityEngine.Object::Instantiate(UnityEngine.Object)");
DO_APP_FUNC(NetworkedPlayerInfo*, GameData_AddDummy, (GameData* __this, PlayerControl* pc, MethodInfo* method), "Assembly-CSharp, NetworkedPlayerInfo GameData::AddDummy(PlayerControl)");
DO_APP_FUNC(void, InnerNetClient_Spawn, (InnerNetClient* __this, InnerNetObject* netObjParent, int32_t ownerId, SpawnFlags__Enum flags, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::Spawn(InnerNet.InnerNetObject, System.Int32, InnerNet.SpawnFlags)");
DO_APP_FUNC(void, MatchMakerGameButton_SetGame, (MatchMakerGameButton* __this, GameListing gameListing, MethodInfo* method), "Assembly-CSharp, System.Void MatchMakerGameButton::SetGame(InnerNet.GameListing)");
DO_APP_FUNC(void, ModManager_LateUpdate, (ModManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void ModManager::LateUpdate()");
DO_APP_FUNC(void, ModManager_ShowModStamp, (ModManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void ModManager::ShowModStamp()");
DO_APP_FUNC(void, EndGameNavigation_ShowDefaultNavigation, (EndGameNavigation* __this, MethodInfo* method), "Assembly-CSharp, System.Void EndGameNavigation::ShowDefaultNavigation()");
//DO_APP_FUNC(void*, EndGameNavigation_CoJoinGame, (EndGameNavigation* __this, MethodInfo* method), "Assembly-CSharp, System.Collections.IEnumerator EndGameNavigation::CoJoinGame()");
DO_APP_FUNC(PlayerBodyTypes__Enum, HideAndSeekManager_GetBodyType, (HideAndSeekManager* __this, PlayerControl* player, MethodInfo* method), "Assembly-CSharp, PlayerBodyTypes HideAndSeekManager::GetBodyType(PlayerControl)");
DO_APP_FUNC(PlayerBodyTypes__Enum, NormalGameManager_GetBodyType, (NormalGameManager* __this, PlayerControl* player, MethodInfo* method), "Assembly-CSharp, PlayerBodyTypes NormalGameManager::GetBodyType(PlayerControl)");
DO_APP_FUNC(void, PlayerControl_SetInvisibility, (PlayerControl* __this, bool isActive, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::SetInvisibility(System.Boolean)");
DO_APP_FUNC(void, CosmeticsLayer_SetPhantomRoleAlpha, (CosmeticsLayer* __this, float alphaValue, MethodInfo* method), "Assembly-CSharp, System.Void CosmeticsLayer::SetPhantomRoleAlpha(System.Single)");
DO_APP_FUNC(bool, Vent_TryMoveToVent, (Vent* __this, Vent* otherVent, String** error, MethodInfo* method), "Assembly-CSharp, System.Boolean Vent::TryMoveToVent(Vent, System.String&)");
DO_APP_FUNC(void, InnerNetClient_ReportPlayer, (InnerNetClient* __this, int32_t clientId, ReportReasons__Enum reason, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::ReportPlayer(System.Int32, InnerNet.ReportReasons)");
DO_APP_FUNC(void, QuickChatNetData_Serialize, (QuickChatPhraseBuilderResult* data, MessageWriter* writer, MethodInfo* method), "Assembly-CSharp, System.Void AmongUs.QuickChat.QuickChatNetData::Serialize(AmongUs.QuickChat.QuickChatPhraseBuilderResult, Hazel.MessageWriter)");
DO_APP_FUNC(float, PlayerControl_get_CalculatedAlpha, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Single PlayerControl::get_CalculatedAlpha()");
DO_APP_FUNC(void, AbstractChatInputField_Clear, (AbstractChatInputField* __this, MethodInfo* method), "Assembly-CSharp, System.Void AbstractChatInputField::Clear()");
DO_APP_FUNC(QuickChatModes__Enum, MultiplayerSettingsData_get_ChatMode, (MultiplayerSettingsData* __this, QuickChatModes__Enum value, MethodInfo* method), "Assembly-CSharp, InnerNet.QuickChatModes AmongUs.Data.Settings.MultiplayerSettingsData::get_ChatMode()");
DO_APP_FUNC(void, InnerNetObject_Despawn, (InnerNetObject* __this, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetObject::Despawn()");
DO_APP_FUNC(void, FriendsListUI_UpdateFriendCodeUI, (FriendsListUI* __this, MethodInfo* method), "Assembly-CSharp, System.Void FriendsListUI::UpdateFriendCodeUI()");
DO_APP_FUNC(bool, PlayerControl_IsFlashlightEnabled, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerControl::IsFlashlightEnabled()");
DO_APP_FUNC(void, PlayerControl_OnDestroy, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Void PlayerControl::OnDestroy()");
DO_APP_FUNC(void, MapCountOverlay_OnEnable, (MapCountOverlay* __this, MethodInfo* method), "Assembly-CSharp, System.Void MapCountOverlay::OnEnable()");
DO_APP_FUNC(void, MapCountOverlay_OnDisable, (MapCountOverlay* __this, MethodInfo* method), "Assembly-CSharp, System.Void MapCountOverlay::OnDisable()");
DO_APP_FUNC(void, InnerNetClient_SendLateRejection, (InnerNetClient* __this, int32_t targetId, DisconnectReasons__Enum reason, MethodInfo* method), "Assembly-CSharp, System.Void InnerNet.InnerNetClient::SendLateRejection(System.Int32, DisconnectReasons)");
DO_APP_FUNC(void, BanMenu_Select, (BanMenu* __this, int32_t clientId, MethodInfo* method), "Assembly-CSharp, System.Void BanMenu::Select(System.Int32)");
DO_APP_FUNC(ClientData*, InnerNetClient_GetRecentClient, (InnerNetClient* __this, int32_t clientId, MethodInfo* method), "Assembly-CSharp, InnerNet.ClientData InnerNet.InnerNetClient::GetRecentClient(System.Int32)");
DO_APP_FUNC(void*, IntroCutscene_ShowTeam, (IntroCutscene* __this, List_1_PlayerControl_* teamToShow, float duration, MethodInfo* method), "Assembly-CSharp, System.Collections.IEnumerator IntroCutscene::ShowTeam(System.Collections.Generic.List<PlayerControl>, System.Single)");
DO_APP_FUNC(void*, IntroCutscene_ShowRole, (IntroCutscene* __this, MethodInfo* method), "Assembly-CSharp, System.Collections.IEnumerator IntroCutscene::ShowRole()");
DO_APP_FUNC(int32_t, LogicOptionsHnS_GetCrewmateLeadTime, (LogicOptionsHnS* __this, MethodInfo* method), "Assembly-CSharp, System.Int32 LogicOptionsHnS::GetCrewmateLeadTime()");
DO_APP_FUNC(void, GameContainer_SetupGameInfo, (GameContainer* __this, MethodInfo* method), "Assembly-CSharp, System.Void GameContainer::SetupGameInfo()");
DO_APP_FUNC(void, ChatNotification_SetUp, (ChatNotification* __this, PlayerControl* sender, String* text, MethodInfo* method), "Assembly-CSharp, System.Void ChatNotification::SetUp(PlayerControl, System.String)");
DO_APP_FUNC(void, FindAGameManager_Update, (FindAGameManager* __this, MethodInfo* method), "Assembly-CSharp, System.Void FindAGameManager::Update()");
DO_APP_FUNC(void, AmongUsClient_ExitGame, (AmongUsClient* __this, DisconnectReasons__Enum reason, MethodInfo* method), "Assembly-CSharp, System.Void AmongUsClient::ExitGame(DisconnectReasons)");
DO_APP_FUNC(bool, PlayerControl_AllTasksCompleted, (PlayerControl* __this, MethodInfo* method), "Assembly-CSharp, System.Boolean PlayerControl::AllTasksCompleted()");