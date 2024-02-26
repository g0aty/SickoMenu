#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

RpcReportBody::RpcReportBody(const PlayerSelection& target)
{
	this->reportedPlayer = target;
}

void RpcReportBody::Process()
{
	PlayerControl_CmdReportDeadBody(*Game::pLocalPlayer, reportedPlayer.get_PlayerData().value_or(nullptr), nullptr);
}

RpcForceReportBody::RpcForceReportBody(PlayerControl* Player, const PlayerSelection& target)
{
	this->Player = Player;
	this->reportedPlayer = target;
}

void RpcForceReportBody::Process()
{
	if (Player == nullptr) return;

	PlayerControl_CmdReportDeadBody(Player, reportedPlayer.get_PlayerData().value_or(nullptr), nullptr);
}

RpcForceMeeting::RpcForceMeeting(PlayerControl* Player, const PlayerSelection& target)
{
	this->Player = Player;
	this->reportedPlayer = target;
}

void RpcForceMeeting::Process()
{
	if (Player == nullptr) return;

	PlayerControl_RpcStartMeeting(Player, reportedPlayer.get_PlayerData().value_or(nullptr), nullptr);
}