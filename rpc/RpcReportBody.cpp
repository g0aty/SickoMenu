#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"

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

RpcMeetingExploit::RpcMeetingExploit(PlayerControl* exploitedPlayer)
{
	this->exploitedPlayer = exploitedPlayer;
}

void RpcMeetingExploit::Process()
{
	if (!PlayerSelection(exploitedPlayer).has_value()) return;

	if (!State.InMeeting) {
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId,
			uint8_t(RpcCalls__Enum::ReportDeadBody), SendOption__Enum::None, exploitedPlayer->fields._.OwnerId, NULL);
		MessageWriter_WriteByte(writer, exploitedPlayer->fields.PlayerId, NULL);
		MessageWriter_EndMessage(writer, NULL);
		delete writer;
	}
	for (size_t i = 0; i <= 50; ++i) {
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), exploitedPlayer->fields._.NetId,
			uint8_t(RpcCalls__Enum::StartMeeting), SendOption__Enum::None, exploitedPlayer->fields._.OwnerId, NULL);
		MessageWriter_WriteByte(writer, exploitedPlayer->fields.PlayerId, NULL);
		MessageWriter_EndMessage(writer, NULL);
	}
	
}