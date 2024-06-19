#include "pch-il2cpp.h"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include "logger.h"
#include "utility.h"

void HandleRpc(PlayerControl* player, uint8_t callId, MessageReader* reader) {
	if (player == nullptr) return;
	switch (callId) {
		case (uint8_t)420:
		{
			uint8_t playerId = player->fields.PlayerId; //true SickoMenu detection
			if (!std::count(State.sickoUsers.begin(), State.sickoUsers.end(), playerId)) {
				State.sickoUsers.push_back(playerId);
				STREAM_DEBUG("RPC Received for another SickoMenu user from " << ToString((Game::PlayerId)playerId));
			}
		}
		break;
		case (uint8_t)42069:
		{
			uint8_t playerId = MessageReader_ReadByte(reader, NULL);
			if (!std::count(State.aumUsers.begin(), State.aumUsers.end(), playerId)) {
				State.aumUsers.push_back(playerId);
				STREAM_DEBUG("RPC Received for another AUM user from " << ToString((Game::PlayerId)playerId) << " (RPC sent by " << ToString((Game::PlayerId)player->fields.PlayerId) << ")");
			}
		}
		break;
		case (uint8_t)101:
		{
			std::string playerName = convert_from_string(MessageReader_ReadString(reader, NULL));
			//we have to get only the message, however aum sends the player's name before this
			std::string message = convert_from_string(MessageReader_ReadString(reader, NULL));
			uint32_t colorId = MessageReader_ReadInt32(reader, NULL);
			if (message.size() == 0) break;
			if (!State.PanicMode && State.ReadAndSendAumChat) {
				NetworkedPlayerInfo* local = GetPlayerData(*Game::pLocalPlayer);
				bool wasDead = false;
				if (player != NULL && GetPlayerData(player)->fields.IsDead && local != NULL && !local->fields.IsDead) {
					local->fields.IsDead = true; //see aum chat of ghosts
					wasDead = true;
				}
				ChatController_AddChat(Game::HudManager.GetInstance()->fields.Chat, player, convert_to_string("<#f55><b>[AUM Chat]</b></color>\n" + message), false, NULL);
				if (wasDead) {
					local->fields.IsDead = false;
				}
				STREAM_DEBUG("AUM Chat RPC from " << playerName << " (RPC sent by " << ToString((Game::PlayerId)player->fields.PlayerId) << ")");
			}
		}
		break;
	}
}