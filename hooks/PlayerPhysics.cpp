#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"

void dPlayerPhysics_FixedUpdate(PlayerPhysics* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerPhysics_FixedUpdate executed");
	if (!State.PanicMode && ((*Game::pLocalPlayer) != NULL) && (*Game::pLocalPlayer)->fields.inVent && State.MoveInVentAndShapeshift)
	{
		(*Game::pLocalPlayer)->fields.inVent = false;
		app::PlayerPhysics_FixedUpdate(__this, method);
		(*Game::pLocalPlayer)->fields.inVent = true;
	}
	else
		app::PlayerPhysics_FixedUpdate(__this, method);
}