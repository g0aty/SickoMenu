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
			if (State.modUsers.find(playerId) == State.modUsers.end() && MessageReader_get_BytesRemaining(reader, NULL) == 0) {
				State.modUsers.insert({ playerId, "<#0f0>Sicko</color><#f00>Menu</color>" });
				STREAM_DEBUG("RPC Received for another SickoMenu user from " << ToString((Game::PlayerId)playerId));
			}
		}
		break;
		case (uint8_t)42069:
		{
			uint8_t playerId = player->fields.PlayerId; //MessageReader_ReadByte(reader, NULL);
			if (State.modUsers.find(playerId) == State.modUsers.end()) {
				State.modUsers.insert({ playerId, "<#f55>AmongUsMenu</color>" });
				STREAM_DEBUG("RPC Received for an AmongUsMenu user from " << ToString((Game::PlayerId)playerId) << " (RPC sent by " << ToString((Game::PlayerId)player->fields.PlayerId) << ")");
			}
		}
		case (uint8_t)150:
		{
			uint8_t playerId = player->fields.PlayerId; //MessageReader_ReadByte(reader, NULL);
			if (State.modUsers.find(playerId) == State.modUsers.end()) {
				State.modUsers.insert({ playerId, "<#5f5>BetterAmongUs</color>" });
				STREAM_DEBUG("RPC Received for a BetterAmongUs user from " << ToString((Game::PlayerId)playerId) << " (RPC sent by " << ToString((Game::PlayerId)player->fields.PlayerId) << ")");
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

bool SMAC_HandleRpc(PlayerControl* player, uint8_t callId, MessageReader* reader) {
	if (!State.Enable_SMAC || player == *Game::pLocalPlayer) return false;
	auto pData = GetPlayerData(player);
	switch (callId) {
	case (uint8_t)RpcCalls__Enum::CheckName:
	case (uint8_t)RpcCalls__Enum::SetName: {
		if (IsHost()) break;
		if (State.SMAC_CheckBadNames) {
			auto name = MessageReader_ReadString(reader, NULL);
			std::string nameStr = convert_from_string(name);
			if (MessageReader_get_BytesRemaining(reader, NULL) > 0 || MessageReader_ReadBoolean(reader, NULL)) return false;
			if (nameStr != RemoveHtmlTags(nameStr)) return true;
			if (!IsNameValid(nameStr)) return true;
			SMAC_OnCheatDetected(player, "Abnormal Name");
		}
		break;
	}
	case (uint8_t)RpcCalls__Enum::CheckColor:
		if (IsHost()) break;
		if (State.SMAC_CheckColor && IsInGame()) {
			SMAC_OnCheatDetected(player, "Abnormal Change Color");
			return true;
		}
		break;
	case (uint8_t)RpcCalls__Enum::SetHat:
	case (uint8_t)RpcCalls__Enum::SetHatStr:
	case (uint8_t)RpcCalls__Enum::SetVisor:
	case (uint8_t)RpcCalls__Enum::SetVisorStr:
	case (uint8_t)RpcCalls__Enum::SetSkin:
	case (uint8_t)RpcCalls__Enum::SetSkinStr:
	case (uint8_t)RpcCalls__Enum::SetPet:
	case (uint8_t)RpcCalls__Enum::SetPetStr:
	case (uint8_t)RpcCalls__Enum::SetNamePlate:
	case (uint8_t)RpcCalls__Enum::SetNamePlateStr:
		if (!State.SafeMode) break;
		if (State.SMAC_CheckCosmetics && IsInGame()) {
			SMAC_OnCheatDetected(player, "Abnormal Change Cosmetics");
			return true;
		}
		break;
	case (uint8_t)RpcCalls__Enum::SendChatNote:
		if (State.SMAC_CheckChatNote && (IsInLobby() || !State.InMeeting)) {
			SMAC_OnCheatDetected(player, "Abnormal Chat Note");
			return true;
		}
		break;
	case (uint8_t)RpcCalls__Enum::SetScanner:
		if (State.SMAC_CheckScanner && (IsInLobby() || State.mapType == Settings::MapType::Airship || State.mapType == Settings::MapType::Fungle)) {
			SMAC_OnCheatDetected(player, "Abnormal MedBay Scan");
			return true;
		}
		break;
	case (uint8_t)RpcCalls__Enum::SetTasks:
		if (State.SMAC_CheckTasks && (IsInLobby() || State.InMeeting)) {
			SMAC_OnCheatDetected(player, "Abnormal Set Tasks");
			return true;
		}
		break;
	case (uint8_t)RpcCalls__Enum::SendChat: {
		/*auto msg = MessageReader_ReadString(reader, NULL);
		if (State.SMAC_CheckChat && ((IsInGame() && !State.InMeeting && !pData->fields.IsDead) || msg->fields.m_stringLength > 120)) {
			SMAC_OnCheatDetected(player, "Abnormal Chat");
			return true;
		}*/
		break;
	}
	case (uint8_t)RpcCalls__Enum::StartMeeting: {
		if (State.SMAC_CheckMeeting && (IsInLobby() || GameOptions().GetGameMode() == GameModes__Enum::HideNSeek)) {
			SMAC_OnCheatDetected(player, "Abnormal Meeting");
			return true;
		}
		break;
	}
	case (uint8_t)RpcCalls__Enum::ReportDeadBody: {
		auto bodyPlayer = GetPlayerDataById(MessageReader_ReadByte(reader, NULL));
		if (State.SMAC_CheckReport) {
			if (IsInLobby()) {
				SMAC_OnCheatDetected(player, "Abnormal Report Body");
				return true;
			}
			if (IsInGame() && ((bodyPlayer != NULL && !bodyPlayer->fields.IsDead) || GameOptions().GetGameMode() == GameModes__Enum::HideNSeek)) {
				SMAC_OnCheatDetected(player, "Abnormal Report Body");
				return true;
			}
		}
		break;
	}
	/*case (uint8_t)RpcCalls__Enum::CheckMurder:
	case (uint8_t)RpcCalls__Enum::MurderPlayer:
	check PlayerControl.cpp*/
	case (uint8_t)RpcCalls__Enum::Shapeshift:
	case (uint8_t)RpcCalls__Enum::CheckShapeshift:
	case (uint8_t)RpcCalls__Enum::RejectShapeshift: {
		if (State.SMAC_CheckShapeshift && (IsInLobby() || GetPlayerData(player)->fields.RoleType != RoleTypes__Enum::Shapeshifter)) {
			SMAC_OnCheatDetected(player, "Abnormal Shapeshift");
			return true;
		}
		break;
	}
	case (uint8_t)RpcCalls__Enum::StartVanish:
	case (uint8_t)RpcCalls__Enum::StartAppear:
	case (uint8_t)RpcCalls__Enum::CheckVanish:
	case (uint8_t)RpcCalls__Enum::CheckAppear: {
		if (State.SMAC_CheckVanish && (IsInLobby() || GetPlayerData(player)->fields.RoleType != RoleTypes__Enum::Phantom)) {
			SMAC_OnCheatDetected(player, "Abnormal Vanish/Appear");
			return true;
		}
		break;
	}
	case (uint8_t)RpcCalls__Enum::SetLevel: {
		uint32_t level = MessageReader_ReadUInt32(reader, NULL);
		if (State.SMAC_CheckLevel && (IsInGame() || level >= (uint32_t)State.SMAC_HighLevel)) {
			SMAC_OnCheatDetected(player, "Abnormal Level");
			return true;
		}
		break;
	}
	case (uint8_t)RpcCalls__Enum::EnterVent: {
		if (State.SMAC_CheckVent && (IsInLobby() || pData->fields.IsDead || !(PlayerIsImpostor(pData) || pData->fields.RoleType == RoleTypes__Enum::Engineer))) {
			SMAC_OnCheatDetected(player, "Abnormal Venting");
			return true;
		}
		break;
	}
	case (uint8_t)420: {
		if (State.SMAC_CheckSicko && MessageReader_get_BytesRemaining(reader, NULL) == 0) {
			SMAC_OnCheatDetected(player, "SickoMenu User");
			return true;
		}
		break;
	}
	case (uint8_t)42069: {
		if (State.SMAC_CheckAUM) {
			SMAC_OnCheatDetected(player, "AmongUsMenu User");
			return true;
		}
		break;
	}
	}
	return false;
}

bool SMAC_HandleUpdateSystem(PlayerControl* player, SystemTypes__Enum sysType, uint8_t reader) {
	return false;
}