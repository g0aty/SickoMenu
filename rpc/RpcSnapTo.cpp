#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

RpcSnapTo::RpcSnapTo(Vector2 targetVector)
{
	this->targetVector = targetVector;
}

void RpcSnapTo::Process()
{
	CustomNetworkTransform_RpcSnapTo((*Game::pLocalPlayer)->fields.NetTransform, targetVector, NULL);
}

RpcForceSnapTo::RpcForceSnapTo(PlayerControl* Player, Vector2 targetVector)
{
	this->Player = Player;
	this->targetVector = targetVector;
}

void RpcForceSnapTo::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	//CustomNetworkTransform_RpcSnapTo((Player)->fields.NetTransform, targetVector, NULL);
	CustomNetworkTransform_SnapTo(Player->fields.NetTransform, targetVector, Player->fields.NetTransform->fields.lastSequenceId + 100, NULL);
	for (auto p : GetAllPlayerControl()) {
		if (p == *Game::pLocalPlayer) continue;
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields.NetTransform->fields._.NetId,
			uint8_t(RpcCalls__Enum::SnapTo), SendOption__Enum::None, p->fields._.OwnerId, NULL);
		NetHelpers_WriteVector2(targetVector, writer, NULL);
		MessageWriter_WriteUShort(writer, Player->fields.NetTransform->fields.lastSequenceId + 100, NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
	Player->fields.NetTransform->fields.lastSequenceId += 100;
}