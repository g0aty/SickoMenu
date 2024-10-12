#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dLobbyBehaviour_Start(LobbyBehaviour* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dLobbyBehaviour_Start executed");
	State.LobbyTimer = 15;
	LobbyBehaviour_Start(__this, method);
}