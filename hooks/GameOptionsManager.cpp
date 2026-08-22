#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

int32_t dLogicOptionsHnS_GetCrewmateLeadTime(LogicOptionsHnS* __this, MethodInfo* method) {
	auto localData = *Game::pLocalPlayer == NULL ? NULL : GetPlayerData(*Game::pLocalPlayer);
	int32_t defaultTime = LogicOptionsHnS_GetCrewmateLeadTime(__this, method);
	return (!State.PanicMode && State.NoSeekerAnim &&
		localData != NULL && PlayerIsImpostor(localData)) ? 0 : defaultTime;
}

bool dLogicOptions_GetVisualTasks(LogicOptions* __this, MethodInfo* method) {
	return (!State.PanicMode && State.BypassVisualTasks &&
		GameOptions().HasOptions() && GameOptions().GetGameMode() == GameModes__Enum::HideNSeek) ||
		LogicOptions_GetVisualTasks(__this, method);
}