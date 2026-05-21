#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

int32_t dLogicOptionsHnS_GetCrewmateLeadTime(LogicOptionsHnS* __this, MethodInfo* method) {
	return !State.PanicMode && State.NoSeekerAnim ? 0 : 10; // Anyway it is hardcoded in the game itself to be 10
}
