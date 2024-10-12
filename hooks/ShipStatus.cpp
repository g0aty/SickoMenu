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
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_OnEnable executed");
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
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_RpcUpdateSystem executed");
	if (!State.PanicMode && IsHost() && State.DisableSabotages) {
		return;
	}
	ShipStatus_RpcUpdateSystem(__this, systemType, amount, method);
}

void dShipStatus_RpcCloseDoorsOfType (ShipStatus* __this, SystemTypes__Enum type, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_RpcCloseDoorsOfType executed");
	if (!State.PanicMode && State.DisableSabotages) {
		return;
	}
	ShipStatus_RpcCloseDoorsOfType(__this, type, method);
}

void dGameStartManager_Update(GameStartManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dGameStartManager_Update executed");
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
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_HandleRpc executed");
	if (IsHost() && (!State.PanicMode || State.BattleRoyale) && State.DisableSabotages && 
		(callId == (uint8_t)RpcCalls__Enum::CloseDoorsOfType || callId == (uint8_t)RpcCalls__Enum::UpdateSystem)) return;
	ShipStatus_HandleRpc(__this, callId, reader, method);
}

void dShipStatus_UpdateSystem(ShipStatus* __this, SystemTypes__Enum systemType, PlayerControl* player, uint8_t amount, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dShipStatus_UpdateSystem executed");
	if (State.mapType != Settings::MapType::Fungle && systemType == SystemTypes__Enum::Electrical) {
		il2cpp::Dictionary<Dictionary_2_SystemTypes_ISystemType_> systems = (*Game::pShipStatus)->fields.Systems;

		auto switchSystem = (SwitchSystem*)(systems[SystemTypes__Enum::Electrical]);
		auto actualSwitches = switchSystem->fields.ActualSwitches;
		auto expectedSwitches = switchSystem->fields.ExpectedSwitches;

		auto switchMask = 1 << (amount & 0x1F);

		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, 
				(actualSwitches & switchMask) != (expectedSwitches & switchMask) ? SABOTAGE_ACTIONS::SABOTAGE_FIX : SABOTAGE_ACTIONS::SABOTAGE_CALL));
		}

		if (amount >= 5 && State.Enable_SMAC && State.SMAC_CheckSabotage)
			SMAC_OnCheatDetected(player, "Bad Sabotage");
	}
	if (systemType == SystemTypes__Enum::Comms) {
		if (amount == 128) {
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
		}
		else {
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_FIX));
			}
		}
	}
	switch (State.mapType) {
	case Settings::MapType::Pb:
	{
		switch (systemType) {
		case SystemTypes__Enum::Laboratory:
			if (amount == 128) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
				}
				if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			}
			else {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_FIX));
				}
			}
			break;
		case SystemTypes__Enum::Comms:
			break;
		case SystemTypes__Enum::Electrical:
			break;
		default:
			if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
		}
	}
	break;
	case Settings::MapType::Airship:
	{
		switch (systemType) {
		case SystemTypes__Enum::HeliSabotage:
			if (amount == 128) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
				}
				if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			}
			else {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_FIX));
				}
			}
			break;
		case SystemTypes__Enum::Comms:
			break;
		case SystemTypes__Enum::Electrical:
			break;
		default:
			if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
		}
	}
	break;
	case Settings::MapType::Fungle:
	{
		switch (systemType) {
		case SystemTypes__Enum::Reactor:
			if (amount == 128) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
				}
				if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			}
			else {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_FIX));
				}
			}
			break;
		case SystemTypes__Enum::MushroomMixupSabotage:
			if (amount == 1) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
				}
			}
			if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			break;
		case SystemTypes__Enum::Electrical:
			break;
		default:
			if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
		}
	}
	break;
	default: //skeld and mira have same sabotages
	{
		switch (systemType) {
		case SystemTypes__Enum::Reactor:
			if (amount == 128) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
				}
				if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			}
			else {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_FIX));
				}
			}
			break;
		case SystemTypes__Enum::LifeSupp:
			if (amount == 128) {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_CALL));
				}
				if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			}
			else {
				synchronized(Replay::replayEventMutex) {
					State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), systemType, SABOTAGE_ACTIONS::SABOTAGE_FIX));
				}
			}
			break;
		case SystemTypes__Enum::Electrical:
			break;
		case SystemTypes__Enum::Comms:
			break;
		default:
			if (State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
		}
	}
	break;
	}

	if (systemType == SystemTypes__Enum::Sabotage) {
		switch (amount) {
		case (int)SystemTypes__Enum::MushroomMixupSabotage:
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), SystemTypes__Enum::MushroomMixupSabotage, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			if (State.mapType != Settings::MapType::Fungle && State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			break;
		case (int)SystemTypes__Enum::Reactor:
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), SystemTypes__Enum::Reactor, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			if (!(State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle)
				&& State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			break;
		case (int)SystemTypes__Enum::Laboratory:
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), SystemTypes__Enum::Laboratory, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			if (State.mapType != Settings::MapType::Pb && State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			break;
		case (int)SystemTypes__Enum::HeliSabotage:
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), SystemTypes__Enum::HeliSabotage, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			if (State.mapType != Settings::MapType::Airship && State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			break;
		case (int)SystemTypes__Enum::LifeSupp:
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), SystemTypes__Enum::LifeSupp, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			if (!(State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq)
				&& State.Enable_SMAC && State.SMAC_CheckSabotage) SMAC_OnCheatDetected(player, "Bad Sabotage");
			break;
		case (int)SystemTypes__Enum::Comms:
			synchronized(Replay::replayEventMutex) {
				State.liveReplayEvents.emplace_back(std::make_unique<SabotageEvent>(GetEventPlayerControl(player).value(), SystemTypes__Enum::Comms, SABOTAGE_ACTIONS::SABOTAGE_CALL));
			}
			break;
		}
	}
	ShipStatus_UpdateSystem(__this, systemType, player, amount, method);
	LOG_DEBUG(std::format("SystemType {} updated with amount {}", (std::string)TranslateSystemTypes(systemType), amount).c_str());
}