#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dLobbyBehaviour_Start(LobbyBehaviour* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLobbyBehaviour_Start executed");
	State.LobbyTimer = 15;
	LobbyBehaviour_Start(__this, method);
}

void dLobbyBehaviour_Update(LobbyBehaviour* __this, MethodInfo* method)
{
	static bool hasStarted = true;
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLobbyBehaviour_Update executed");
	LobbyBehaviour_Update(__this, method);
	if (State.DisableLobbyMusic) {
		hasStarted = false;
		SoundManager_StopSound(SoundManager__TypeInfo->static_fields->instance, (AudioClip*)__this->fields.MapTheme, NULL);
	}
	else if (!hasStarted) {
		hasStarted = true;
		LobbyBehaviour_Start(__this, method); //restart lobby music
	}
}