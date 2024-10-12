#include "pch-il2cpp.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"

void dPlayerStorageManager_OnReadPlayerPrefsComplete(PlayerStorageManager* __this, void* data, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dPlayerStorageManager_OnReadPlayerPrefsComplete executed");
    app::PlayerStorageManager_OnReadPlayerPrefsComplete(__this, data, method);
    if (!State.PanicMode) __this->fields._PlayerPrefs_k__BackingField.IsAdult = 1;
}