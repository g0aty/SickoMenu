#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"

RpcReportBody::RpcReportBody(const PlayerSelection& target)
{
	this->reportedPlayer = target;
}

void RpcReportBody::Process()
{
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	if (IsHost() && !State.PanicMode && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) return;
	PlayerControl_CmdReportDeadBody(*Game::pLocalPlayer, reportedPlayer.get_PlayerData().value_or(nullptr), nullptr);
}

RpcForceReportBody::RpcForceReportBody(PlayerControl* Player, const PlayerSelection& target)
{
	this->Player = Player;
	this->reportedPlayer = target;
}

void RpcForceReportBody::Process()
{
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	if (Player == nullptr) return;
	if (IsHost() && !State.PanicMode && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) return;
	PlayerControl_CmdReportDeadBody(Player, reportedPlayer.get_PlayerData().value_or(nullptr), nullptr);
}

RpcForceMeeting::RpcForceMeeting(PlayerControl* Player, const PlayerSelection& target)
{
	this->Player = Player;
	this->reportedPlayer = target;
}

void RpcForceMeeting::Process()
{
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	if (Player == nullptr) return;
	if (IsHost() && !State.PanicMode && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) return;
	PlayerControl_RpcStartMeeting(Player, reportedPlayer.get_PlayerData().value_or(nullptr), nullptr);
}

RpcSpamMeeting::RpcSpamMeeting(PlayerControl* Player, PlayerControl* target, bool inMeeting)
{
	this->Player = Player;
	this->target = target;
	this->inMeeting = inMeeting;
}

void RpcSpamMeeting::Process()
{
	if (Object_1_IsNull((Object_1*)*Game::pShipStatus)) return;
	if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) return;
	if (IsHost() && !State.PanicMode && (State.DisableMeetings || (State.BattleRoyale || State.TaskSpeedrun))) return;
	if (!inMeeting) PlayerControl_CmdReportDeadBody(Player, GetPlayerData(target), nullptr);
	for (int i = 0; i < 200; ++i) {
		if (!PlayerSelection(Player).has_value() || !PlayerSelection(target).has_value()) break;
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), GetPlayerData(target)->fields._.NetId,
			uint8_t(RpcCalls__Enum::ReportDeadBody), SendOption__Enum::None, GetPlayerData(target)->fields._.OwnerId, NULL);
		MessageWriter_WriteByte(writer, 255, NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
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