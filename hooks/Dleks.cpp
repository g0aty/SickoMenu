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
	if (State.ShowHookLogs) LOG_DEBUG("Hook dConstants_1_GetBroadcastVersion executed");
	int32_t orig_return = Constants_1_GetBroadcastVersion(method);
	if (State.DisableHostAnticheat) orig_return += 25;
	return orig_return;
}

bool dConstants_1_IsVersionModded(MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dConstants_1_IsVersionModded executed");
	if (State.DisableHostAnticheat) return true; //this helps to bypass anticheat in our hosted lobbies
	//return false;
	return Constants_1_IsVersionModded(method);
}

AsyncOperationHandle_1_UnityEngine_GameObject_ dAssetReference_InstantiateAsync_1(AssetReference* __this, Transform* parent, bool instantiateInWorldSpace, MethodInfo* method) {
	LOG_DEBUG(std::format("AssetReference_InstantiateAsync executed with scene {}", State.CurrentScene).c_str());
	try {
		if (IsHost() && !IsInGame() && (*Game::pAmongUsClient) != NULL && parent == NULL && !instantiateInWorldSpace) {
			il2cpp::List shipPrefabs = (*Game::pAmongUsClient)->fields.ShipPrefabs;
			if (__this == shipPrefabs[0] && State.FlipSkeld) {
				(*Game::pAmongUsClient)->fields.ShipLoadingAsyncHandle = AssetReference_InstantiateAsync_1(shipPrefabs[3], parent, instantiateInWorldSpace, method);
				return AssetReference_InstantiateAsync_1(shipPrefabs[3], parent, instantiateInWorldSpace, method);
			}
		}
	}
	catch (...) {}
	return AssetReference_InstantiateAsync_1(__this, parent, instantiateInWorldSpace, method);
}