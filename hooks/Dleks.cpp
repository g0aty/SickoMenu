#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

/*bool dConstants_1_ShouldFlipSkeld(MethodInfo* method) {
	bool orig_return = Constants_1_ShouldFlipSkeld(method);
	if (State.FlipSkeld) {
		return true;
	}
	else if (orig_return)
	{
		State.FlipSkeld = true;
	} //fix later
	return orig_return;
}*/

int32_t dConstants_1_GetBroadcastVersion(MethodInfo* method) {
	int32_t orig_return = Constants_1_GetBroadcastVersion(method);
	if (IsModdedHost()) orig_return += 25;
	return orig_return;
}

bool dConstants_1_IsVersionModded(MethodInfo* method) {
	if (IsModdedHost()) return true; //this helps to bypass anticheat in our hosted lobbies
	return false;
}

bool dAprilFoolsMode_ShouldFlipSkeld(MethodInfo* method) {
	bool orig_return = AprilFoolsMode_ShouldFlipSkeld(method);
	/*if (State.AprilFools_FlipSkeld) {
		return true;
	}
	else if (orig_return)
	{
		State.AprilFools_FlipSkeld = true;
	}*/ //this DOES NOT work
	return orig_return;
}

bool dAprilFoolsMode_ShouldHorseAround(MethodInfo* method) {
	return /*State.AprilFools_AlwaysHorse || */AprilFoolsMode_ShouldHorseAround(method);
}

bool dAprilFoolsMode_ShouldLongAround(MethodInfo* method) {
	return State.AprilFools_AlwaysLong || AprilFoolsMode_ShouldLongAround(method);
}