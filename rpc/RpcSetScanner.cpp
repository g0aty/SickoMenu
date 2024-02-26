#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

RpcSetScanner::RpcSetScanner(bool playAnimation)
{
	this->playAnimation = playAnimation;
}

void RpcSetScanner::Process()
{
	PlayerControl_RpcSetScanner(*Game::pLocalPlayer, playAnimation, NULL);
}

RpcForceScanner::RpcForceScanner(PlayerControl* Player, bool playAnimation)
{
	this->Player = Player;
	this->playAnimation = playAnimation;
}

void RpcForceScanner::Process()
{
	if (Player == nullptr) return;

	//PlayerControl_RpcSetScanner(Player, playAnimation, NULL);

	for (auto p : GetAllPlayerControl()) {
		MessageWriter* writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId, 
			uint8_t(RpcCalls__Enum::SetScanner), SendOption__Enum::None, p->fields._.OwnerId, NULL);
		MessageWriter_WriteBoolean(writer, playAnimation, NULL);
		MessageWriter_WriteByte(writer, Player->fields.scannerCount + 1, NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
}