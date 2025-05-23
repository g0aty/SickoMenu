#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include <memory>

float dVent_CanUse(Vent* __this, NetworkedPlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dVent_CanUse executed");
	if (!State.PanicMode && (State.UnlockVents || (*Game::pLocalPlayer)->fields.inVent)) {
		auto object = NetworkedPlayerInfo_get_Object(pc, nullptr);
		if (!object) {
			LOG_ERROR(ToString(pc) + " _object is null");
			return app::Vent_CanUse(__this, pc, canUse, couldUse, method);
		}

		auto ventTransform = app::Component_get_transform((Component_1*)__this, NULL);
		auto ventVector = app::Transform_get_position(ventTransform, NULL);

		auto playerPosition = app::PlayerControl_GetTruePosition(object, NULL);

		float ventDistance = app::Vector2_Distance(playerPosition, { ventVector.x, ventVector.y }, NULL);
		if (pc->fields.IsDead) {
			*canUse = false;
			*couldUse = false;
		}
		else {
			*canUse = (ventDistance < app::Vent_get_UsableDistance(__this, NULL));
			*couldUse = true;
		}
		return ventDistance;
	}

	return app::Vent_CanUse(__this, pc, canUse, couldUse, method);
};

void dVent_EnterVent(Vent* __this, PlayerControl* pc, MethodInfo * method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dVent_EnterVent executed");
	if (!State.PanicMode) {
		auto ventVector = app::Transform_get_position(app::Component_get_transform((Component_1*)__this, NULL), NULL);
		app::Vector2 ventVector2D = { ventVector.x, ventVector.y };
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<VentEvent>(GetEventPlayerControl(pc).value(), ventVector2D, VENT_ACTIONS::VENT_ENTER));
			State.liveConsoleEvents.emplace_back(std::make_unique<VentEvent>(GetEventPlayerControl(pc).value(), ventVector2D, VENT_ACTIONS::VENT_ENTER));
		}
		if (State.confuser && State.confuseOnVent && pc == *Game::pLocalPlayer)
			ControlAppearance(true);
	}
	Vent_EnterVent(__this, pc, method);
}

void* dVent_ExitVent(Vent* __this, PlayerControl* pc, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dVent_ExitVent executed");
	if (!State.PanicMode) {
		auto ventVector = app::Transform_get_position(app::Component_get_transform((Component_1*)__this, NULL), NULL);
		app::Vector2 ventVector2D = { ventVector.x, ventVector.y };
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<VentEvent>(GetEventPlayerControl(pc).value(), ventVector2D, VENT_ACTIONS::VENT_EXIT));
			State.liveConsoleEvents.emplace_back(std::make_unique<VentEvent>(GetEventPlayerControl(pc).value(), ventVector2D, VENT_ACTIONS::VENT_EXIT));
		}
	}
	return Vent_ExitVent(__this, pc, method);
}

bool dVent_TryMoveToVent(Vent* __this, Vent* otherVent, String** error, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dVent_TryMoveToVent executed");
	if (!State.PanicMode && *Game::pLocalPlayer != NULL) {
		bool wasVisible = PlayerControl_get_Visible(*Game::pLocalPlayer, NULL) && !(*Game::pLocalPlayer)->fields.walkingToVent && State.ShowPlayersInVents && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead;
		if (wasVisible) {
			PlayerControl_set_Visible(*Game::pLocalPlayer, false, NULL);
		}
		return Vent_TryMoveToVent(__this, otherVent, error, method);
	}
	else return Vent_TryMoveToVent(__this, otherVent, error, method);
}

/*void dVentilationSystem_Update(VentilationSystem_Operation__Enum op, int32_t ventId, MethodInfo* method) {
	VentilationSystem_Update(op, ventId, method);
	if (State.FlipSkeld && IsHost() && op == VentilationSystem_Operation__Enum::Exit && *Game::pLocalPlayer != NULL)
		(*Game::pLocalPlayer)->fields.inVent = false; // Fix venting on Dleks
}*/

void dPlayerPhysics_RpcExitVent(PlayerPhysics* __this, int32_t id, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerPhysics_RpcExitVent executed");
	PlayerPhysics_RpcExitVent(__this, id, method);
	/*if (State.FlipSkeld && IsHost() && *Game::pLocalPlayer != NULL && __this->fields.myPlayer != NULL && __this->fields.myPlayer == *Game::pLocalPlayer)
		(*Game::pLocalPlayer)->fields.inVent = false; // Fix venting on Dleks*/
	// Not necessary with v16.0.0 :Cool:
}