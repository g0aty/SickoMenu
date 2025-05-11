#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

constexpr int32_t AnticheatPenalty = 25;

void LogIfEnabled(const std::string& message) {
    if (State.ShowHookLogs) {
        LOG_DEBUG(message.c_str());
    }
}

bool ShouldDisableHostAnticheat() {
	return State.DisableHostAnticheat && State.CurrentScene != "MatchMaking" && State.CurrentScene != "FindAGame" && !State.IsFreePlay;
}

int32_t dConstants_1_GetBroadcastVersion(MethodInfo* method) {
    LogIfEnabled("Hook dConstants_1_GetBroadcastVersion executed");
    int32_t baseVersion = Constants_1_GetBroadcastVersion(method);
    return baseVersion + (ShouldDisableHostAnticheat() ? AnticheatPenalty : 0);
}

bool dConstants_1_IsVersionModded(MethodInfo* method) {
    LogIfEnabled("Hook dConstants_1_IsVersionModded executed");
    return ShouldDisableHostAnticheat() || Constants_1_IsVersionModded(method);
}

/*AsyncOperationHandle_1_UnityEngine_GameObject_ dAssetReference_InstantiateAsync_1(AssetReference* __this, Transform* parent, bool instantiateInWorldSpace, MethodInfo* method) {
    LogIfEnabled("Hook dAssetReference_InstantiateAsync_1 executed");
    LOG_DEBUG(std::format("AssetReference_InstantiateAsync executed with scene {}", State.CurrentScene).c_str());
    return AssetReference_InstantiateAsync_1(__this, parent, instantiateInWorldSpace, method);
}*/

bool dAprilFoolsMode_ShouldFlipSkeld(MethodInfo* method) {
    if (IsHost()) return State.FlipSkeld;
    State.FlipSkeld = AprilFoolsMode_ShouldFlipSkeld(method);
    return State.FlipSkeld;
}

void dFreeplayPopover_PlayMap(FreeplayPopover* __this, MapNames__Enum map, MethodInfo* method) {
	LogIfEnabled("Hook dFreeplayPopover_PlayMap executed");
    State.IsFreePlay = true;
	FreeplayPopover_PlayMap(__this, map, method);
}
