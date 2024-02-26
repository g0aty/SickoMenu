#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"

RpcSetColor::RpcSetColor(uint8_t bodyColor, bool force)
{
	this->bodyColor = bodyColor;
	this->forceColor = force;
}

void RpcSetColor::Process()
{
	if (forceColor)
		PlayerControl_RpcSetColor(*Game::pLocalPlayer, bodyColor, NULL);
	else
		PlayerControl_CmdCheckColor(*Game::pLocalPlayer, bodyColor, NULL);
}

RpcForceColor::RpcForceColor(PlayerControl* player, uint8_t bodyColor)
{
	this->Player = player;
	this->bodyColor = bodyColor;
}

void RpcForceColor::Process()
{
	if (!PlayerSelection(Player).has_value()) return;

	//PlayerControl_RpcSetColor(Player, bodyColor, NULL);
	
	for (auto p : GetAllPlayerControl()) {
		auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), Player->fields._.NetId,
			uint8_t(RpcCalls__Enum::SetColor), SendOption__Enum::None, p->fields._.OwnerId, NULL);
		MessageWriter_WriteByte(writer, uint8_t(bodyColor), NULL);
		InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
	}
}