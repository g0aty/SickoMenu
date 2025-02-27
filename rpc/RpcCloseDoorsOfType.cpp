#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"

using namespace std::string_view_literals;

RpcCloseDoorsOfType::RpcCloseDoorsOfType(SystemTypes__Enum selectedSystem, bool pinDoor)
{
	this->selectedSystem = selectedSystem;
	this->pinDoor = pinDoor;
}

void RpcCloseDoorsOfType::Process()
{
	if (selectedSystem == SystemTypes__Enum::Decontamination || selectedSystem == SystemTypes__Enum::Decontamination2 || selectedSystem == SystemTypes__Enum::Decontamination3)
		return;
	app::ShipStatus_RpcCloseDoorsOfType(*Game::pShipStatus, this->selectedSystem, NULL);
	if (this->pinDoor)
		State.pinnedDoors.push_back(this->selectedSystem);
}

RpcOpenDoorsOfType::RpcOpenDoorsOfType(SystemTypes__Enum selectedSystem)
{
	this->selectedSystem = selectedSystem;
}

void RpcOpenDoorsOfType::Process()
{
	if (selectedSystem == SystemTypes__Enum::Decontamination || selectedSystem == SystemTypes__Enum::Decontamination2 || selectedSystem == SystemTypes__Enum::Decontamination3)
		return;
	for (auto door : il2cpp::Array((*Game::pShipStatus)->fields.AllDoors))
	{
		if (door->fields.Room == selectedSystem)
		{
			app::ShipStatus_RpcUpdateSystem(*Game::pShipStatus, SystemTypes__Enum::Doors, (uint8_t)(door->fields.Id | 64), NULL);
			if ("PlainDoor"sv == door->klass->name) app::PlainDoor_SetDoorway(reinterpret_cast<PlainDoor*>(door), true, {});
			else if ("MushroomWallDoor"sv == door->klass->name) app::MushroomWallDoor_SetDoorway(reinterpret_cast<MushroomWallDoor*>(door), true, {});
		}
	}

}