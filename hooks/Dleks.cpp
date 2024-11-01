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

//Full Code from KillNetwork source code:

constexpr int32_t AnticheatPenalty = 25; // Prevent rare crashing

void LogIfEnabled(const std::string& message) {
    if (State.ShowHookLogs) {
        LOG_DEBUG(message.c_str());
    }
}

int32_t GetAdjustedBroadcastVersion(MethodInfo* method) {
    LogIfEnabled("Hook dConstants_1_GetBroadcastVersion executed");
    int32_t baseVersion = Constants_1_GetBroadcastVersion(method);
    return baseVersion + (State.DisableHostAnticheat ? AnticheatPenalty : 0); // Prevents bugs with protocol mixing
}

bool IsVersionModded(MethodInfo* method) {
    LogIfEnabled("Hook dConstants_1_IsVersionModded executed");
    return State.DisableHostAnticheat || Constants_1_IsVersionModded(method);
}

AsyncOperationHandle_1_UnityEngine_GameObject_ InstantiateAssetAsync(
    AssetReference* assetRef,
    Transform* parent,
    bool instantiateInWorldSpace,
    MethodInfo* method)
{
    LOG_DEBUG(std::format("AssetReference_InstantiateAsync executed with scene {}", State.CurrentScene).c_str());

    bool isHost = IsHost();
    bool isInGame = IsInGame();
    auto amongUsClient = *Game::pAmongUsClient;

    // Check for special asset instantiation case
    if (isHost && !isInGame && amongUsClient && parent == nullptr && !instantiateInWorldSpace) {
        il2cpp::List shipPrefabs = amongUsClient->fields.ShipPrefabs;

        if (assetRef == shipPrefabs[0] && State.FlipSkeld) {
            auto asyncHandle = AssetReference_InstantiateAsync_1(shipPrefabs[3], parent, instantiateInWorldSpace, method);
            amongUsClient->fields.ShipLoadingAsyncHandle = asyncHandle;
            return asyncHandle;
        }
    }

    try {
        return AssetReference_InstantiateAsync_1(assetRef, parent, instantiateInWorldSpace, method);
    }
    catch (const std::exception& e) {
        LOG_ERROR(std::format("Exception caught: {}", e.what()).c_str());
    }
    catch (...) {
        LOG_ERROR("Unknown exception caught");
    }

    return {}; // Return default constructed handle on failure
}

int32_t dConstants_1_GetBroadcastVersion(MethodInfo* method) {
    return GetAdjustedBroadcastVersion(method);
}

bool dConstants_1_IsVersionModded(MethodInfo* method) {
    return IsVersionModded(method);
}

AsyncOperationHandle_1_UnityEngine_GameObject_ dAssetReference_InstantiateAsync_1(
    AssetReference* __this,
    Transform* parent,
    bool instantiateInWorldSpace,
    MethodInfo* method)
{
    return InstantiateAssetAsync(__this, parent, instantiateInWorldSpace, method);
}
