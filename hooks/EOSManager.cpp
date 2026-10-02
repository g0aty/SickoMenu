#include "pch-il2cpp.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"
#include <regex>

static bool isGuestAccount = false;

void fakeSuccessfulLogin(EOSManager* eosManager)
{
	/*eosManager->fields.loginFlowFinished = true;
	EOSManager_HasFinishedLoginFlow(eosManager, NULL);*/
	auto player = app::DataManager_get_Player(nullptr);
	static FieldInfo* field = il2cpp_class_get_field_from_name(player->klass, "account");
	LOG_ASSERT(field != nullptr);
	auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, player);
	//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
	static FieldInfo* field1 = il2cpp_class_get_field_from_name(account->klass, "loginStatus");
	auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
	il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
}

void setUsernameEAU(EditAccountUsername* eau) {
	bool isFriendCodeValid = !State.NewFriendCode.empty() && State.NewFriendCode.find(" ") == std::string::npos && State.NewFriendCode.length() <= 10;
	auto tmpText = (TMP_Text*)eau->fields.UsernameText;

	if (State.UseNewFriendCode && isFriendCodeValid) {
		std::string newFriendCode = "";
		for (auto i : State.NewFriendCode) {
			if (newFriendCode.ends_with(" ")) {
				break;
			}
			newFriendCode += tolower(i);
		}
		TMP_Text_set_text(tmpText, convert_to_string(newFriendCode), NULL);
	}
	else {
		auto textStr = convert_from_string(TMP_Text_get_text(tmpText, NULL));
		if (!textStr.empty()) {
			std::string newFriendCode = "";
			for (auto i : textStr) {
				newFriendCode += tolower(i);
			}
			TMP_Text_set_text(tmpText, convert_to_string(newFriendCode), NULL);
		}
		else {
			std::string newFriendCode = "";
			std::string randomString = GenerateRandomString();
			for (auto i : randomString) {
				newFriendCode += tolower(i);
			}
			TMP_Text_set_text(tmpText, convert_to_string(newFriendCode), NULL);
		}
	}
}

/*void dEOSManager_StartInitialLoginFlow(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_StartInitialLoginFlow executed", false);
	EOSManager_DeleteDeviceID(__this, NULL, NULL);
	if (!State.SpoofGuestAccount) {
		EOSManager_StartInitialLoginFlow(__this, method);
		EOSManager_EndMergeGuestAccountFlow(__this, method);
		return;
	}
	EOSManager_StartTempAccountFlow(__this, method);
	//isGuestAccount = true;
	EOSManager_CloseStartupWaitScreen(__this, method);
}*/

void dEOSManager_BeginLoginFlowWithDeviceID(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_BeginLoginFlowWithDeviceID executed", false);
	EOSManager_DeleteDeviceID(__this, NULL, NULL);
	if (!State.SpoofGuestAccount) {
		EOSManager_BeginLoginFlowWithDeviceID(__this, method);
		EOSManager_EndMergeGuestAccountFlow(__this, method);
		return;
	}
	EOSManager_StartTempAccountFlow(__this, method);
	isGuestAccount = true;
	EOSManager_CloseStartupWaitScreen(__this, method);
}

void dEOSManager_LoginFromAccountTab(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_LoginFromAccountTab executed", false);
	EOSManager_LoginFromAccountTab(__this, method);
	if (State.SpoofGuestAccount) {
		LOG_DEBUG("Faking login");
		fakeSuccessfulLogin(__this);
	}
}

void dEOSManager_InitializePlatformInterface(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_InitializePlatformInterface executed", false);
	EOSManager_InitializePlatformInterface(__this, method);
	//LOG_DEBUG("Skipping device identification");
	//__this->fields.platformInitialized = true;
}

bool dEOSManager_IsFreechatAllowed(EOSManager* __this, MethodInfo* method)
{
	bool ret = !isGuestAccount || IsInGame() || IsInLobby();
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_IsFreechatAllowed executed", false);
	return ret;
}

QuickChatModes__Enum dMultiplayerSettingsData_get_ChatMode(MultiplayerSettingsData* __this, QuickChatModes__Enum value, MethodInfo* method) {
	bool commandsAllowed = (State.ReadAndSendSickoChat || State.ExtraCommands) && (IsInGame() || IsInLobby());
	State.CurrentChatMode = MultiplayerSettingsData_get_ChatMode(__this, value, method);
	return commandsAllowed ? QuickChatModes__Enum::FreeChatOrQuickChat : State.CurrentChatMode;
}

bool dEOSManager_IsFriendsListAllowed(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_IsFriendsListAllowed executed", false);
	return app::EOSManager_IsFriendsListAllowed(__this, method);
}

void dEOSManager_UpdatePermissionKeys(EOSManager* __this, void* callback, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_UpdatePermissionKeys executed", false);
	/*Il2CppClass* klass = get_class("Assembly-CSharp, EOSManager");
	LOG_ASSERT(klass);
	FieldInfo* field = il2cpp_class_get_field_from_name(klass, "isKWSMinor");
	LOG_ASSERT(field);
	bool value = false;
	il2cpp_field_set_value((Il2CppObject*)__this, field, &value);*/

	app::EOSManager_UpdatePermissionKeys(__this, callback, method);
}

void dEOSManager_Update(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_Update executed", false);
	static bool hasDeletedDeviceId = false;
	//__this->fields.ageOfConsent = 0; //why tf does amogus have an age of consent lmao
	//if (State.SpoofFriendCode) __this->fields.friendCode = convert_to_string(State.FakeFriendCode);
	EOSManager_Update(__this, method);
	//EOSManager_set_FriendCode(__this, __this->fields.friendCode, NULL);
	if (State.SpoofGuestAccount) {
		fakeSuccessfulLogin(__this);
	}

	if (State.SpoofLevel) {
		auto player = DataManager_get_Player(NULL);
		auto stats = PlayerData_get_Stats(player, NULL);
		int fakeLevel = State.SafeMode ? std::clamp(State.FakeLevel, 1, 100001) : State.FakeLevel;
		stats->fields.level = fakeLevel - 1;
		AbstractSaveData_Save((AbstractSaveData*)player, NULL);
	}
}

String* dEOSManager_get_ProductUserId(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dEOSManager_get_ProductUserId executed", false);
	auto puid = EOSManager_get_ProductUserId(__this, method);
	if (State.UseGuestPuid && State.GuestPuid != "")
		return convert_to_string(State.FakePuid);
	return puid;
}

void dPlatformSpecificData_Serialize(PlatformSpecificData* __this, MessageWriter* writer, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dPlatformSpecificData_Serialize executed", false);
	if (!State.PanicMode) {
		if (State.SpoofPlatform) __this->fields.Platform = Platforms__Enum(State.FakePlatform + 1);
		if (State.FakePlatform == (int)Platforms__Enum::Xbox)
			__this->fields.XboxPlatformId = State.FakeXboxId;
		if (State.FakePlatform == (int)Platforms__Enum::Playstation)
			__this->fields.PsnPlatformId = State.FakePsnId;
		if (State.SpoofPlName) __this->fields.PlatformName = convert_to_string(State.FakePlName);
	}
	PlatformSpecificData_Serialize(__this, writer, method);
}

void dEditAccountUsername_SaveUsername(EditAccountUsername* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dEditAccountUsername_SaveUsername executed", false);
	setUsernameEAU(__this);
	EditAccountUsername_SaveUsername(__this, method);
}