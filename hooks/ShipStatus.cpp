#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include "utility.h"
#include "replay.hpp"
#include "profiler.h"
#include "game.h"

float dShipStatus_CalculateLightRadius(ShipStatus* __this, NetworkedPlayerInfo* player, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_CalculateLightRadius executed");
	if (IsHost() && State.TaskSpeedrun && State.GameLoaded && State.mapType != Settings::MapType::Airship)
		State.SpeedrunTimer += Time_get_deltaTime(NULL);
	switch (__this->fields.Type) {
	case ShipStatus_MapType__Enum::Ship:
		if (State.mapType != Settings::MapType::Airship) State.mapType = Settings::MapType::Ship;
		if (State.FlipSkeld && IsHost() && GameOptions().GetByte(app::ByteOptionNames__Enum::MapId) != 3) {
			GameOptions().SetByte(app::ByteOptionNames__Enum::MapId, 3);
			auto gameOptionsManager = GameOptionsManager_get_Instance(NULL);
			GameManager* gameManager = GameManager_get_Instance(NULL);
			GameOptionsManager_set_GameHostOptions(gameOptionsManager, GameOptionsManager_get_CurrentGameOptions(gameOptionsManager, NULL), NULL);
			LogicOptions_SyncOptions(GameManager_get_LogicOptions(gameManager, NULL), NULL);
		}
		if (GameOptions().GetByte(app::ByteOptionNames__Enum::MapId) == 3) State.FlipSkeld = true;
		else State.FlipSkeld = false;
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
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_OnEnable executed");
	try {
		State.BlinkPlayersTab = false;

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
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_RpcUpdateSystem executed");
	ShipStatus_RpcUpdateSystem(__this, systemType, amount, method);
}

void dShipStatus_RpcCloseDoorsOfType(ShipStatus* __this, SystemTypes__Enum type, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_RpcCloseDoorsOfType executed");
	ShipStatus_RpcCloseDoorsOfType(__this, type, method);
}

void dShipStatus_HandleRpc(ShipStatus* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_HandleRpc executed");
	ShipStatus_HandleRpc(__this, callId, reader, method);
}

bool DetectCheatSabotageResult(PlayerControl* player, bool result) {
	if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
	return result;
}

bool DetectCheatSabotage(SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount) {
	uint8_t mapId = (uint8_t)State.mapType;
	if (systemType == SystemTypes__Enum::Sabotage && PlayerIsImpostor(GetPlayerData(player)))
		return false;
	else if (systemType == SystemTypes__Enum::LifeSupp &&
		(mapId == 0 || mapId == 1) && (amount == 64 || amount == 65))
		return false;
	// Only Skeld and Mira have oxygen sabotage
	else if (systemType == SystemTypes__Enum::Comms) {
		if (amount == 0 && mapId != 1 && mapId != 4) return false;
		if ((amount == 64 || amount == 65 || amount == 32 || amount == 33 || amount == 16 || amount == 17)
			&& (mapId == 1 || mapId == 5)) return false;
	}
	else if (systemType == SystemTypes__Enum::Electrical) {
		if (mapId != 4 && amount < 5) return false;
		else if (amount >= 5 && !(State.DisableSabotages && IsHost())) {
			return DetectCheatSabotageResult(player, false);
		}
	}
	else if (systemType == SystemTypes__Enum::Laboratory &&
		mapId == 2 && (amount == 64 || amount == 65 || amount == 32 || amount == 33))
		return false;
	else if (systemType == SystemTypes__Enum::Reactor &&
		mapId != 2 && mapId != 3 && (amount == 64 || amount == 65 || amount == 32 || amount == 33))
		return false;
	else if (systemType == SystemTypes__Enum::HeliSabotage &&
		mapId == 3 && (amount == 64 || amount == 65 || amount == 16 || amount == 17 || amount == 32 || amount == 33))
		return false;
	else if (systemType == SystemTypes__Enum::MushroomMixupSabotage) {
		if (mapId == 4 && !(State.DisableSabotages && IsHost())) {
			return DetectCheatSabotageResult(player, false);
		}
	}
	else if (State.InMeeting && MeetingHud__TypeInfo->static_fields->Instance->fields.state != MeetingHud_VoteStates__Enum::Animating) {
		if (!(State.DisableSabotages && IsHost())) {
			return DetectCheatSabotageResult(player, false);
		}
	}
	return DetectCheatSabotageResult(player, true);
}

void dShipStatus_UpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_UpdateSystem executed");
	if (player == NULL) return;
	LOG_DEBUG(std::format("SystemType {} updated with amount {}", (std::string)TranslateSystemTypes(systemType), amount).c_str());
	if (!State.DisableSabotages || State.PanicMode || !IsHost())
		return ShipStatus_UpdateSystem(__this, systemType, player, amount, method);
	if (systemType == SystemTypes__Enum::Ventilation ||
		systemType == SystemTypes__Enum::Security ||
		systemType == SystemTypes__Enum::Decontamination ||
		systemType == SystemTypes__Enum::Decontamination2 ||
		systemType == SystemTypes__Enum::Decontamination3 ||
		systemType == SystemTypes__Enum::MedBay)
		return ShipStatus_UpdateSystem(__this, systemType, player, amount, method);
	if (!DetectCheatSabotage(systemType, player, amount)) return ShipStatus_UpdateSystem(__this, systemType, player, amount, method);
}