#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "_rpc.h"

void dPlainDoor_SetDoorway(PlainDoor* __this, bool open, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlainDoor_SetDoorway executed");
	if (__this->fields.Open && (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), __this->fields._.Room) != State.pinnedDoors.end())) {
		ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, __this->fields._.Room, NULL);
	}
	app::PlainDoor_SetDoorway(__this, open, method);
}

void dMushroomWallDoor_SetDoorway(MushroomWallDoor* __this, bool open, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dMushroomWallDoor_SetDoorway executed");
	if (__this->fields.open && (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), __this->fields._.Room) != State.pinnedDoors.end())) {
		ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, __this->fields._.Room, NULL);
	}
	app::MushroomWallDoor_SetDoorway(__this, open, method);
}

bool dAutoOpenDoor_DoUpdate(AutoOpenDoor* __this, float dt, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dAutoOpenDoor_DoUpdate executed");
	if (__this->fields._.Open && (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), __this->fields._._.Room) != State.pinnedDoors.end()) && __this->fields.ClosedTimer < 1.5f) {
		ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, __this->fields._._.Room, NULL);
	}
	return app::AutoOpenDoor_DoUpdate(__this, dt, method);
}