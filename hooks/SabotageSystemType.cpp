#include "pch-il2cpp.h"
#include "_hooks.h"
#include "utility.h"
#include "logger.h"
#include "state.hpp"

void dSabotageSystemType_SetInitialSabotageCooldown(SabotageSystemType* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dSabotageSystemType_SetInitialSabotageCooldown executed");
	app::SabotageSystemType_SetInitialSabotageCooldown(__this, method);

	// ESP: Initialize Kill Cooldown
	for (auto pc : GetAllPlayerControl()) {
		if (auto player = PlayerSelection(pc).validate();
			player.has_value() && !player.is_LocalPlayer() && !player.is_Disconnected()) {
			if (auto role = player.get_PlayerData()->fields.Role;
				role != nullptr && role->fields.CanUseKillButton && !player.get_PlayerData()->fields.IsDead) {
				pc->fields.killTimer = 10.f;
				//STREAM_DEBUG("Player " << ToString(pc) << " KillTimer " << pc->fields.killTimer);
			}
		}
	}
}