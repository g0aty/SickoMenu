#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
RpcPlayAnimation::RpcPlayAnimation(uint8_t animId)
{
	this->animId = animId;
}

void RpcPlayAnimation::Process()
{
	if (State.bypasvisoff) {
		PlayerControl_PlayAnimation((*Game::pLocalPlayer), animId, NULL);
		auto writer = InnerNetClient_StartRpc((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId, uint8_t(RpcCalls__Enum::PlayAnimation), SendOption__Enum::None, NULL);
		MessageWriter_WriteByte(writer, animId, NULL);
		MessageWriter_EndMessage(writer, NULL);
		return;
	}
	PlayerControl_RpcPlayAnimation(*Game::pLocalPlayer, animId, NULL);
}