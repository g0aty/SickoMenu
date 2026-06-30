#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "game.h"

// deprecated
bool dSaveManager_GetPurchase(String* itemKey, String* bundleKey, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.Debug("Hook dSaveManager_GetPurchase executed", false);
	return true;
}

// v2022.10.25s
bool dPlayerPurchasesData_GetPurchase(PlayerPurchasesData* __this, String* itemKey, String* bundleKey, MethodInfo* method) {
	if (State.ShowHookLogs) Log.Debug("Hook dPlayerPurchasesData_GetPurchase executed", false);
	if (State.UnlockCosmetics) {
		return true;
	}
	return PlayerPurchasesData_GetPurchase(__this, itemKey, bundleKey, method);
}