#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"

RpcUpdateSystem::RpcUpdateSystem(SystemTypes__Enum selectedSystem, SystemTypes__Enum amount)
{
	this->selectedSystem = selectedSystem;
	this->amount = (uint32_t)amount;
}

RpcUpdateSystem::RpcUpdateSystem(SystemTypes__Enum selectedSystem, uint32_t amount)
{
	this->selectedSystem = selectedSystem;
	this->amount = amount;
}

void RpcUpdateSystem::Process()
{
	if (IsHost() && !State.PanicMode && (State.DisableSabotages || (State.BattleRoyale || State.TaskSpeedrun))) return;
	ShipStatus_RpcUpdateSystem(*Game::pShipStatus, this->selectedSystem, this->amount, NULL);
}