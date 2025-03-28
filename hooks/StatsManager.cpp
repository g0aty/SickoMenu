#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "logger.h"

/*bool dPlayerBanData_get_IsBanned(PlayerBanData* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerBanData_get_IsBanned executed");
	return false;
}

float dPlayerBanData_get_BanPoints(PlayerBanData* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerBanData_get_BanPoints executed");

	return 0.F;
}*/

int32_t dPlayerBanData_get_BanMinutesLeft(PlayerBanData* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dStatsManager_get_BanMinutesLeft executed");
	__this->fields.banPoints = 0.f;
	return (int32_t)0;
}