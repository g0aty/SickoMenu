#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

constexpr int32_t AnticheatPenalty = 25;

void LogIfEnabled(const std::string& message) {
    if (State.ShowHookLogs) {
        LOG_DEBUG(message.c_str());
    }
}

int32_t GetAdjustedBroadcastVersion(MethodInfo* method) {
    LogIfEnabled("Hook dConstants_1_GetBroadcastVersion executed");
    int32_t baseVersion = Constants_1_GetBroadcastVersion(method);
    return baseVersion + (State.DisableHostAnticheat ? AnticheatPenalty : 0);
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
    if (!__this) {
        LOG_ERROR("AssetReference is null in dAssetReference_InstantiateAsync_1!");
        return {};
    }

    return InstantiateAssetAsync(__this, parent, instantiateInWorldSpace, method);
}
