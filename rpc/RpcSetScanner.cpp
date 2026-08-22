#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
RpcSetScanner::RpcSetScanner(bool playAnimation)
{
	this->playAnimation = playAnimation;
}

void RpcSetScanner::Process()
{
    bool visualsOn = GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks, false);
    if ((State.BypassVisualTasks || !playAnimation) && !visualsOn) {
        (*Game::pLocalPlayer)->fields.scannerCount++;
        PlayerControl_SetScanner(*Game::pLocalPlayer, playAnimation, (*Game::pLocalPlayer)->fields.scannerCount);
        auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId,
            uint8_t(RpcCalls__Enum::SetScanner), SendOption__Enum::Reliable, -1, NULL);
        MessageWriter_WriteBoolean(writer, playAnimation, NULL);
        MessageWriter_WriteByte(writer, (*Game::pLocalPlayer)->fields.scannerCount, NULL);
        InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
        return;
    }
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

    bool visualsOn = GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks, false);
    if ((State.BypassVisualTasks || !playAnimation) && !visualsOn) {
        Player->fields.scannerCount++;
        PlayerControl_SetScanner(Player, playAnimation, Player->fields.scannerCount);
        auto writer = InnerNetClient_StartRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), (*Game::pLocalPlayer)->fields._.NetId,
            uint8_t(RpcCalls__Enum::SetScanner), SendOption__Enum::Reliable, -1, NULL);
        MessageWriter_WriteBoolean(writer, playAnimation, NULL);
        MessageWriter_WriteByte(writer, Player->fields.scannerCount, NULL);
        InnerNetClient_FinishRpcImmediately((InnerNetClient*)(*Game::pAmongUsClient), writer, NULL);
        return;
    }
    PlayerControl_RpcSetScanner(Player, playAnimation, NULL);
}