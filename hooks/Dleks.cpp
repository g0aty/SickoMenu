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
    if (State.CurrentScene == "" || State.CurrentScene == "SplashIntro" || State.CurrentScene == "MatchMaking" || State.CurrentScene == "Tutorial" ||
        (State.CurrentScene == "MainMenu" && State.ShouldIgnoreBroadcastVersionHook)) {
        // This should not lead to unexpected behavior with unexpected disconnections
        return Constants_1_GetBroadcastVersion(method);
	}
    int32_t baseVersion = 50632950;
    if (State.SpoofAUVersion) {
        switch (State.FakeAUVersion) {
		case 0: // AU v16.0.0 / v16.0.2
			baseVersion = 50614950;
			break;
		case 1: // AU v16.0.5 / v16.1.0
			baseVersion = 50632950;
			break;
        }
    }
    else int32_t baseVersion = Constants_1_GetBroadcastVersion(method);
    // This is the broadcast version for v16.1.0
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

void dMainMenuManager_LateUpdate(MainMenuManager* __this, MethodInfo* method) {
    LogIfEnabled("Hook dMainMenuManager_OpenOnlineMenu executed");
    MainMenuManager_LateUpdate(__this, method);
    State.ShouldIgnoreBroadcastVersionHook = GameObject_GetActive(__this->fields.gameModeButtons, NULL);
}
