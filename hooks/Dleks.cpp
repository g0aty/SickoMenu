#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

constexpr int32_t AnticheatPenalty = 25;

void LogIfEnabled(const std::string& message) {
    if (State.ShowHookLogs) {
        Log.Debug(message.c_str(), false);
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
    int32_t baseVersion;
    if (State.SpoofAUVersion) {
        switch (State.FakeAUVersion) {
            case 0: // AU v16.0.0 / v16.0.2
                baseVersion = 50614950;
                break;
            case 1: // AU v16.0.5 / v16.1.0
            default:
                baseVersion = 50632950;
                break;
        }
    }
    else baseVersion = Constants_1_GetBroadcastVersion(method);
    return baseVersion + (ShouldDisableHostAnticheat() ? AnticheatPenalty : 0);
}

bool dConstants_1_IsVersionModded(MethodInfo* method) {
    LogIfEnabled("Hook dConstants_1_IsVersionModded executed");
    return ShouldDisableHostAnticheat() || Constants_1_IsVersionModded(method);
}

AsyncOperationHandle_1_UnityEngine_GameObject_ InstantiateAssetAsync(
    AssetReference* assetRef,
    Transform* parent,
    bool instantiateInWorldSpace,
    MethodInfo* method)
{
    LOG_DEBUG(std::format("AssetReference_InstantiateAsync executed with scene {}", State.CurrentScene).c_str());

    if (!assetRef) {
        LOG_ERROR("AssetReference is null!");
        return {};
    }

    auto amongUsClient = *Game::pAmongUsClient;
    if (!amongUsClient) {
        LOG_ERROR("AmongUsClient is null!");
        return {};
    }

    if (IsHost() && !IsInGame() && parent == nullptr && !instantiateInWorldSpace) {
        il2cpp::List shipPrefabs = amongUsClient->fields.ShipPrefabs;
        if (shipPrefabs.size() > 3 && assetRef == shipPrefabs[0] && State.FlipSkeld) {
            auto asyncHandle = AssetReference_InstantiateAsync_1(shipPrefabs[3], parent, instantiateInWorldSpace, method);
            amongUsClient->fields.ShipLoadingAsyncHandle = asyncHandle;
            return asyncHandle;
        }
    }

    try {
        return AssetReference_InstantiateAsync_1(assetRef, parent, instantiateInWorldSpace, method);
    }
    catch (const std::exception& e) {
        LOG_ERROR(std::format("Exception caught in AssetReference_InstantiateAsync: {}", e.what()).c_str());
    }
    catch (...) {
        LOG_ERROR("Unknown exception caught in AssetReference_InstantiateAsync");
    }

    return {};
}

AsyncOperationHandle_1_UnityEngine_GameObject_ dAssetReference_InstantiateAsync_1(
    AssetReference* __this,
    Transform* parent,
    bool instantiateInWorldSpace,
    MethodInfo* method)
{
    if (!__this) {
        LOG_ERROR("AssetReference is null in dAssetReference_InstantiateAsync_1!");
        return {};
    }

    return InstantiateAssetAsync(__this, parent, instantiateInWorldSpace, method);
}

bool dAprilFoolsMode_ShouldFlipSkeld(MethodInfo* method) {
    /*if (IsHost()) return State.FlipSkeld;
    State.FlipSkeld = AprilFoolsMode_ShouldFlipSkeld(method);
    return State.FlipSkeld;*/
    return AprilFoolsMode_ShouldFlipSkeld(method); // the previous method leads to unexpected crashing
}

void dMainMenuManager_LateUpdate(MainMenuManager* __this, MethodInfo* method) {
    LogIfEnabled("Hook dMainMenuManager_OpenOnlineMenu executed");
    MainMenuManager_LateUpdate(__this, method);
    State.ShouldIgnoreBroadcastVersionHook = GameObject_GetActive(__this->fields.gameModeButtons, NULL);
}
