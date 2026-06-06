#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"
#include "logger.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>

using namespace app;
using SteadyClock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<SteadyClock>;

static const std::unordered_map<uint8_t, std::pair<int, float>> RpcLimits = {
    { 18, {100, 0.1f} }, { 49, {100, 0.1f} }, { 44, {50,  0.1f} },
    { 50, {100, 0.1f} }, { 21, {20,  0.1f} }, { 8,  {30,  0.1f} },
    { 6,  {30,  0.1f} }, { 39, {30,  0.1f} }, { 40, {30,  0.1f} },
    { 42, {30,  0.1f} }, { 41, {30,  0.1f} }, { 33, {1,   1.0f} },
    { 54, {5,   1.0f} }, { 7,  {100, 0.1f} },
};

static const std::unordered_set<uint8_t> ImmediatelyRPCs = {
    51,54,5,7,14,47,48,12,52,53,45,46,62,64,55,56,2,63,65,21
};
static const std::unordered_set<uint8_t> SusRPCs = { 101,164,154,85 };
static const std::unordered_set<uint8_t> ExcludedNumMsg = { 41,39,40,42,43,38 };
static const std::unordered_set<uint8_t> ExcludedForLobby = {
    2,5,7,9,10,13,17,18,21,33,36,37,38,39,40,41,42,43,49,50,60,61,80,78,70,210,81,176
};
static const std::unordered_set<uint8_t> NormalCustomIds = { 80,78,70,210,81,176 };

struct PlayerRpcTracker {
    TimePoint joinTime;
    std::unordered_map<uint8_t, std::queue<TimePoint>> timestamps;
};
static std::unordered_map<uint8_t, PlayerRpcTracker> g_trackers;

static bool CheckSpam(PlayerRpcTracker& tracker, uint8_t callId) {
    auto& q = tracker.timestamps[callId];
    auto  now = SteadyClock::now();
    int   maxCount = 10;
    float window = 1.0f;
    auto  lim = RpcLimits.find(callId);
    if (lim != RpcLimits.end()) { maxCount = lim->second.first; window = lim->second.second; }
    while (!q.empty()) {
        if (std::chrono::duration<float>(now - q.front()).count() > window) q.pop(); else break;
    }
    q.push(now);
    if ((int)q.size() > maxCount) {
        LOG_DEBUG(("AntiOverload: RPC Spam id=" + std::to_string(callId) +
            " (" + std::to_string(q.size()) + "/" + std::to_string(maxCount) + ")").c_str());
        return false;
    }
    return true;
}

bool RpcCheck(PlayerControl* player, uint8_t callId, int numData) {
    if (!player) return true;
    if (Game::pLocalPlayer && player == *Game::pLocalPlayer) return true;
    uint8_t pid = player->fields.PlayerId;
    auto& tracker = g_trackers[pid];
    float joinAge = std::chrono::duration<float>(SteadyClock::now() - tracker.joinTime).count();
    if (joinAge < 3.0f) return true;
    if (NormalCustomIds.count(callId)) return true;
    if (SusRPCs.count(callId)) {
        LOG_DEBUG(("AntiOverload: Sus RPC blocked id=" + std::to_string(callId)).c_str());
        return false;
    }
    if (!CheckSpam(tracker, callId)) return false;
    if (IsInLobby() && !ExcludedForLobby.count(callId)) {
        LOG_DEBUG(("AntiOverload: Lobby RPC blocked id=" + std::to_string(callId)).c_str());
        return false;
    }
    if (!ExcludedNumMsg.count(callId)) {
        if (ImmediatelyRPCs.count(callId) && numData > 1) {
            LOG_DEBUG(("AntiOverload: ImmediateRPC flood id=" + std::to_string(callId)).c_str());
            return false;
        }
        if (numData > 10) {
            LOG_DEBUG(("AntiOverload: Big data RPC id=" + std::to_string(callId)).c_str());
            return false;
        }
    }
    return true;
}

void AntiOverload_OnPlayerJoin(uint8_t playerId) {
    g_trackers[playerId].joinTime = SteadyClock::now();
    g_trackers[playerId].timestamps = {};
}
void AntiOverload_OnPlayerLeave(uint8_t playerId) {
    g_trackers.erase(playerId);
}

void dPlayerPhysics_HandleRpc(PlayerPhysics* __this, uint8_t callId, MessageReader* reader, MethodInfo* method) {
    if (State.AntiOverload && __this && __this->fields.myPlayer) {
        auto* p = __this->fields.myPlayer;
        if (!(Game::pLocalPlayer && p == *Game::pLocalPlayer))
            if (!RpcCheck(p, callId, 1)) return;
    }
    PlayerPhysics_HandleRpc(__this, callId, reader, method);
}

void dInnerNetClient_HandleGameData(InnerNetClient* __this, MessageReader* parentReader, MethodInfo* method) {
    if (State.AntiOverload && __this->fields.InOnlineScene) {
        int32_t remaining = MessageReader_get_BytesRemaining(parentReader, NULL);
        if (remaining > 300) { // 100 sub-messages * 3 bytes minimum each
            LOG_DEBUG(("AntiOverload: Data flood blocked, bytes=" + std::to_string(remaining)).c_str());
            return;
        }
    }
    InnerNetClient_HandleGameData(__this, parentReader, method);
}