#include "pch-il2cpp.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"

void dAccountManager_UpdateKidAccountDisplay(AccountManager* __this, MethodInfo* method) {
    if (State.ShowHookLogs) LOG_DEBUG("Hook dAccountManager_UpdateKidAccountDisplay executed");
    // grant permissions
    if (!State.PanicMode) {
        __this->fields.freeChatAllowed = KWSPermissionStatus__Enum::Granted;
        __this->fields.customDisplayName = KWSPermissionStatus__Enum::Granted;
        __this->fields.friendsListAllowed = KWSPermissionStatus__Enum::Granted;
    }
    app::AccountManager_UpdateKidAccountDisplay(__this, method);
}