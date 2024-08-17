#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include "utility.h"
#include "replay.hpp"
#include "profiler.h"
#include "game.h"

float dShipStatus_CalculateLightRadius(ShipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method) {
	switch (__this->fields.Type) {
	case ShipStatus_MapType__Enum::Ship:
		if (State.mapType != Settings::MapType::Airship) State.mapType = Settings::MapType::Ship;
		break;
	case ShipStatus_MapType__Enum::Hq:
		State.mapType = Settings::MapType::Hq;
		break;
	case ShipStatus_MapType__Enum::Pb:
		State.mapType = Settings::MapType::Pb;
		break;
	case ShipStatus_MapType__Enum::Fungle:
		State.mapType = Settings::MapType::Fungle;
		break;
	}
	if (!State.PanicMode && State.MaxVision)
		return 420.F;
	else
		return ShipStatus_CalculateLightRadius(__this, player, method);
}

void dShipStatus_OnEnable(ShipStatus* __this, MethodInfo* method) {
	try {

		Replay::Reset();

		State.MatchStart = std::chrono::system_clock::now();
		State.MatchCurrent = State.MatchStart;

		State.selectedDoor = SystemTypes__Enum::Hallway;
		State.mapDoors.clear();
		State.pinnedDoors.clear();

		il2cpp::Array allDoors = __this->fields.AllDoors;

		for (auto door : allDoors) {
			if (std::find(State.mapDoors.begin(), State.mapDoors.end(), door->fields.Room) == State.mapDoors.end())
				State.mapDoors.push_back(door->fields.Room);
		}

		std::sort(State.mapDoors.begin(), State.mapDoors.end());

		if (!State.PanicMode && State.confuser && State.confuseOnStart)
			ControlAppearance(true);

		if (State.AutoFakeRole) {
			if (!State.SafeMode) State.rpcQueue.push(new RpcSetRole(*Game::pLocalPlayer, (RoleTypes__Enum)State.FakeRole));
		}

		//if (!State.mapDoors.empty() && Constants_1_ShouldFlipSkeld(NULL))
			//State.FlipSkeld = true; fix later
	}
	catch (...) {
		LOG_ERROR("Exception occurred in ShipStatus_OnEnable (ShipStatus)");
	}
	ShipStatus_OnEnable(__this, method);
}

void dShipStatus_RpcUpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, int32_t amount, MethodInfo* method) {
	if (!State.PanicMode && IsHost() && State.DisableSabotages) {
		return;
	}
	ShipStatus_RpcUpdateSystem(__this, systemType, amount, method);
}

void dShipStatus_RpcCloseDoorsOfType (ShipStatus* __this, SystemTypes__Enum type, MethodInfo* method) {
	if (!State.PanicMode && State.DisableSabotages) {
		return;
	}
	ShipStatus_RpcCloseDoorsOfType(__this, type, method);
}

void dGameStartManager_Update(GameStartManager* __this, MethodInfo* method) {
	try {
		if (State.HideCode && IsStreamerMode() && !State.PanicMode) {
			std::string customCode = State.HideCode ? State.customCode : "******";
			if (State.RgbLobbyCode)
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(State.rgbCode + customCode), NULL);
			else
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(customCode), NULL);
		}
		else {
			std::string LobbyCode = convert_from_string(InnerNet_GameCode_IntToGameName((*Game::pAmongUsClient)->fields._.GameId, NULL));
			if (State.RgbLobbyCode && !State.PanicMode)
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(State.rgbCode + LobbyCode), NULL);
			else
				TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(LobbyCode), NULL);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in GameStartManager_Update (ShipStatus)");
	}
	GameStartManager_Update(__this, method);
}

void dShipStatus_HandleRpc(ShipStatus* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
	if (IsHost() && (!State.PanicMode || State.BattleRoyale) && State.DisableSabotages && 
		(callId == (uint8_t)RpcCalls__Enum::CloseDoorsOfType || callId == (uint8_t)RpcCalls__Enum::UpdateSystem)) return;
	ShipStatus_HandleRpc(__this, callId, reader, method);
}