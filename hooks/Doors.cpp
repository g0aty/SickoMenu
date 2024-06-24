#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "_rpc.h"

void dPlainDoor_SetDoorway(PlainDoor* __this, bool open, MethodInfo* method) {
	if (__this->fields.Open && (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), __this->fields._.Room) != State.pinnedDoors.end())) {
		ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, __this->fields._.Room, NULL);
	}
	app::PlainDoor_SetDoorway(__this, open, method);
}

void dMushroomWallDoor_SetDoorway(MushroomWallDoor* __this, bool open, MethodInfo* method) {
	if (__this->fields.open && (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), __this->fields._.Room) != State.pinnedDoors.end())) {
		ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, __this->fields._.Room, NULL);
	}
	app::MushroomWallDoor_SetDoorway(__this, open, method);
}

bool dAutoOpenDoor_DoUpdate(AutoOpenDoor* __this, float dt, MethodInfo* method) {
	if (__this->fields._.Open && (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), __this->fields._._.Room) != State.pinnedDoors.end()) && __this->fields.ClosedTimer < 1.5f) {
		ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, __this->fields._._.Room, NULL);
	}
	return app::AutoOpenDoor_DoUpdate(__this, dt, method);
}