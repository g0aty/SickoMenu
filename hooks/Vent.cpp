#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"
#include <memory>

float dVent_CanUse(Vent* __this, NetworkedPlayerInfo* pc, bool* canUse, bool* couldUse, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dVent_CanUse executed", false);
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
	if (State.ShowHookLogs) Log.HookDebug("Hook dVent_EnterVent executed", false);
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
	if (State.ShowHookLogs) Log.HookDebug("Hook dVent_ExitVent executed", false);
	if (!State.PanicMode) {
		auto ventVector = app::Transform_get_position(app::Component_get_transform((Component_1*)__this, NULL), NULL);
		app::Vector2 ventVector2D = { ventVector.x, ventVector.y };
		synchronized(Replay::replayEventMutex) {
			State.liveReplayEvents.emplace_back(std::make_unique<VentEvent>(GetEventPlayerControl(pc).value(), ventVector2D, VENT_ACTIONS::VENT_EXIT));
			State.liveConsoleEvents.emplace_back(std::make_unique<VentEvent>(GetEventPlayerControl(pc).value(), ventVector2D, VENT_ACTIONS::VENT_EXIT));
		}
	}

	auto ret = Vent_ExitVent(__this, pc, method);
	if (!State.PanicMode && State.KillImmunity && pc == *Game::pLocalPlayer) SendKillImmuneToggle(true);
	return ret;
}

bool dVent_TryMoveToVent(Vent* __this, Vent* otherVent, String** error, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dVent_TryMoveToVent executed", false);
	if (!State.PanicMode && *Game::pLocalPlayer != NULL) {
		bool wasVisible = PlayerControl_get_Visible(*Game::pLocalPlayer, NULL) && !(*Game::pLocalPlayer)->fields.walkingToVent && State.ShowPlayersInVents && !GetPlayerData(*Game::pLocalPlayer)->fields.IsDead;
		if (wasVisible && (*Game::pLocalPlayer)->fields.inVent) {
			PlayerControl_set_Visible(*Game::pLocalPlayer, false, NULL);
		}
		return Vent_TryMoveToVent(__this, otherVent, error, method);
	}
	else return Vent_TryMoveToVent(__this, otherVent, error, method);
}

void dVentilationSystem_Update(VentilationSystem_Operation__Enum op, int32_t ventId, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dVentilationSystem_Update executed", false);
	if (!State.PanicMode && State.KillImmunity && op == VentilationSystem_Operation__Enum::Exit) return;
	VentilationSystem_Update(op, ventId, method);
	/*if (State.FlipSkeld && IsHost() && op == VentilationSystem_Operation__Enum::Exit && *Game::pLocalPlayer != NULL)
		(*Game::pLocalPlayer)->fields.inVent = false;*/ // Fix venting on Dleks
}

void dVentilationSystem_UpdateSystem(VentilationSystem* __this, PlayerControl* player, MessageReader* msgReader, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dVentilationSystem_UpdateSystem executed", false);

	if (!IsHost()) {
		int32_t pos = msgReader->fields._position, head = msgReader->fields.readHead;

		MessageReader_ReadUInt16(msgReader, NULL); // handle operation ID
		auto ventOp = (VentilationSystem_Operation__Enum)MessageReader_ReadByte(msgReader, NULL);

		msgReader->fields._position = pos;
		msgReader->fields.readHead = head;

		if (!State.PanicMode && ventOp == VentilationSystem_Operation__Enum::BootImpostors) {
			auto* notifier = (NotificationPopper*)Game::HudManager.GetInstance()->fields.Notifier;
			if (notifier) {
				auto* spriteBackup = new Sprite(*notifier->fields.playerDisconnectSprite);
				Color colorBackup = notifier->fields.disconnectColor;

				notifier->fields.playerDisconnectSprite = notifier->fields.settingsChangeSprite;
				notifier->fields.disconnectColor = Color(1.f, 0.f, 0.f, 1.f);

				std::string killNotif = std::format("<#f00>{} attempted to ban you, but failed!</color>",
					convert_from_string(GetPlayerOutfit(GetPlayerData(player))->fields.PlayerName));
				NotificationPopper_AddDisconnectMessage(notifier, convert_to_string(killNotif), nullptr);

				notifier->fields.playerDisconnectSprite = spriteBackup;
				notifier->fields.disconnectColor = colorBackup;
			}
		}
		return;
	}
	VentilationSystem_UpdateSystem(__this, player, msgReader, method);
}