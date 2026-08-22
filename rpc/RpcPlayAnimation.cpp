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
	bool visualsOn = GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks, false);
	if (State.BypassVisualTasks && !visualsOn) {
		PlayerControl_PlayAnimation((*Game::pLocalPlayer), animId, NULL);
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId, uint8_t(RpcCalls__Enum::PlayAnimation), SendOption__Enum::None, -1, NULL);
		MessageWriter_WriteByte(writer, animId, NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
        return;
    }
	PlayerControl_RpcPlayAnimation(*Game::pLocalPlayer, animId, NULL);
}