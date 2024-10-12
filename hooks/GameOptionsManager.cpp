#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dGameOptionsManager_set_CurrentGameOptions(GameOptionsManager* __this, IGameOptions* value, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dGameOptionsManager_set_CurrentGameOptions executed");
	GameOptionsManager_set_CurrentGameOptions(__this, value, method);
	try {
		GameOptions gameOptions(value);
		if (gameOptions.HasOptions()) {
			SaveGameOptions(gameOptions);
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in GameOptionsManager_set_CurrentGameOptions (GameOptionsManager)");
	}
}