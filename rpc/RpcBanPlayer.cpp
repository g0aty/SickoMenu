#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"

RpcBanPlayer::RpcBanPlayer(PlayerControl* target, int count, ReportReasons__Enum reason)
{
	this->target = target;
	this->count = count;
	this->reason = reason;
}

void RpcBanPlayer::Process()
{
	if (!PlayerSelection(target).has_value()) return;

	for (int i = 0; i < count; ++i) {
		if (!PlayerSelection(target).has_value()) break;
		InnerNetClient_ReportPlayer((InnerNetClient*)(*Game::pAmongUsClient), target->fields._.OwnerId, reason, NULL);
	}
}
