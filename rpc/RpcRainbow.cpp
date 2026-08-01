#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"

RpcRainbowPlayer::RpcRainbowPlayer(PlayerControl* Player) {
	this->Player = Player;
}

void RpcRainbowPlayer::Process() {
	if (Player == nullptr) return;
	auto pd = GetPlayerData(Player);
	if (pd == nullptr) return;
	auto outfit = GetPlayerOutfit(pd);
	if (outfit == nullptr) return;
	uint8_t nextColor = (uint8_t)((outfit->fields.ColorId + 1) % 18);
	PlayerControl_RpcSetColor(Player, nextColor, NULL);
}