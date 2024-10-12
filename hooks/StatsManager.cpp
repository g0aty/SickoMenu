#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"

bool dStatsManager_get_AmBanned(StatsManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dStatsManager_get_AmBanned executed");
	return false;
}

float dStatsManager_get_BanPoints(StatsManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dStatsManager_get_BanPoints executed");

	if ((__this->fields.loadedStats == true) && (__this->fields.stats != nullptr))
		__this->fields.stats->fields.banPoints = 0.F;

	return 0.F;
}

int32_t dStatsManager_get_BanMinutesLeft(StatsManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dStatsManager_get_BanMinutesLeft executed");
	return (int32_t)0;
}