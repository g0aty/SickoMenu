#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include "utility.h"

void dFungleShipStatus_OnEnable(FungleShipStatus* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dFungleShipStatus_OnEnable executed", false);
	FungleShipStatus_OnEnable(__this, method);

	try {
		State.BlinkPlayersTab = false;

		Replay::Reset();

		State.MatchStart = std::chrono::system_clock::now();
		State.MatchCurrent = State.MatchStart;

		State.selectedDoor = SystemTypes__Enum::Hallway;
		State.mapDoors.clear();
		State.pinnedDoors.clear();

		il2cpp::Array allDoors = __this->fields._.AllDoors;

		for (auto door : allDoors) {
			if (std::find(State.mapDoors.begin(), State.mapDoors.end(), door->fields.Room) == State.mapDoors.end())
				State.mapDoors.push_back(door->fields.Room);
		}

		std::sort(State.mapDoors.begin(), State.mapDoors.end());

		if (!State.PanicMode && State.confuser && State.confuseOnStart)
			ControlAppearance(true);

		if (State.AutoFakeRole) {
			if (!State.SafeMode) State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, (RoleTypes__Enum)State.FakeRole));
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in FungleShipStatus_OnEnable (FungleShipStatus)");
	}
}

void dZiplineConsole_Update(ZiplineConsole* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dZiplineConsole_Update executed", false);

	if (!State.PanicMode && State.NoAbilityCD) __this->fields._CoolDown_k__BackingField = 0.f;

	return ZiplineConsole_Update(__this, method);
}

void dMushroom_FixedUpdate(Mushroom* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dZiplineConsole_Update executed", false);
	Mushroom_FixedUpdate(__this, method);
	
	// the following code is used as __this->fields.sporeMask throws null reference errors when directly used
	// (last time i checked this was in like 2023)
	static FieldInfo* field = il2cpp_class_get_field_from_name(((Il2CppObject*)__this)->klass, "sporeMask");
	if (field == nullptr) return;
	auto sporeMask = (GameObject*)il2cpp_field_get_value_object(field, (Il2CppObject*)__this);
	if(sporeMask == NULL) return;
	auto transform = GameObject_get_transform(sporeMask, NULL);
	if (transform == NULL) return;

	Vector3 sporeMaskPos = Transform_get_position(transform, NULL);
	auto localData = GetPlayerData(*Game::pLocalPlayer);
	bool shouldShowShadowQuad = (State.PanicMode || !(State.IsRevived || State.FreeCam || State.EnableZoom || State.playerToFollow.has_value() || State.Wallhack || (State.MaxVision && IsInLobby())))
		&& (localData != NULL && !localData->fields.IsDead);
	sporeMaskPos.z = shouldShowShadowQuad ? 5.f : -1.f;
	// https://github.com/scp222thj/MalumMenu/blob/main/src/Cheats/MalumESP.cs
	// the default Z position of the spore mask is 5
	Transform_set_position(transform, sporeMaskPos, NULL);
}