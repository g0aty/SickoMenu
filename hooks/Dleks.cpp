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

/*int32_t dConstants_1_GetBroadcastVersion(MethodInfo* method) {
	if (IsModdedHost()) return Constants_1_GetBroadcastVersion(method) + 25; //bypass server sided anticheat as host
	return Constants_1_GetBroadcastVersion(method);
}

bool dConstants_1_IsVersionModded(MethodInfo* method) {
	if (IsModdedHost()) return true; //this helps to bypass anticheat in our hosted lobbies
	return false;
}*/