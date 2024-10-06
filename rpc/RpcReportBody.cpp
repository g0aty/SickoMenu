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

RpcSpamChatNote::RpcSpamChatNote(PlayerControl* exploitedPlayer)
{
	this->exploitedPlayer = exploitedPlayer;
}

void RpcSpamChatNote::Process()
{
	if (!PlayerSelection(exploitedPlayer).has_value()) return;

	for (size_t i = 0; i <= 50; ++i) {
		PlayerControl_RpcSendChatNote(*Game::pLocalPlayer, exploitedPlayer->fields.PlayerId, (ChatNoteTypes__Enum)1, NULL);
	}
}