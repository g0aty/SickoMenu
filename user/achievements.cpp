#include "pch-il2cpp.h"
#include "DestroyableSingleton.h"
#include "utility.h"
#include "achievements.hpp"
#include "logger.h"
#include "state.hpp"

namespace Achievements {

	_Ret_maybenull_ AchievementManager* GetAchievementManager() {
		static DestroyableSingleton<AccountManager*> accountManager{ "Assembly-CSharp, AccountManager" };
		if (!accountManager.IsInstanceExists()
			|| accountManager.GetInstance()->fields.prevLoggedInStatus == EOSManager_AccountLoginStatus__Enum::Offline)
			return nullptr;

		static DestroyableSingleton<AchievementManager*> manager{ "Assembly-CSharp, AchievementManager" };
		return manager.GetInstance();
	}

	bool IsSupported() {
		return !State.unlockAllAchievements && GetAchievementManager() != nullptr;
	}

	void UnlockAll() {
		auto manager = GetAchievementManager();
		if (!manager) return;

		ScopedThreadAttacher managedThreadAttached;
		Log.HookDebug("1");
		// il2cpp::Dictionary achievementGameModeKey = manager->klass->static_fields->AchievementGameModeKey;
		// this crashes the game, so we use the following alternative instead

		auto staticFields = (AchievementManager__StaticFields*)il2cpp_class_get_static_field_data((Il2CppClass*)manager->klass);
		il2cpp::Dictionary achievementGameModeKey = staticFields->AchievementGameModeKey;
		Log.HookDebug("2");
		for (auto pair : achievementGameModeKey) {
			Log.HookDebug("3");
			il2cpp::List list = pair.value;
			if (!list.contains(app::GameModes__Enum::Normal)) {
				list.add(app::GameModes__Enum::Normal);
			}
			if (!list.contains(app::GameModes__Enum::HideNSeek)) {
				list.add(app::GameModes__Enum::HideNSeek);
			}
			app::AchievementManager_UnlockAchievement(manager, pair.key, nullptr);
			Log.HookDebug("4");
		}
	}
}