#include "pch-il2cpp.h"
#include "_hooks.h"
#include "logger.h"
#include "state.hpp"

void fakeSuccessfulLogin(EOSManager* eosManager)
{
	EOSManager_DeleteDeviceID(eosManager, NULL, NULL);
	/*eosManager->fields.loginFlowFinished = true;
	EOSManager_HasFinishedLoginFlow(eosManager, NULL);*/
	auto player = app::DataManager_get_Player(nullptr);
	static FieldInfo* field = il2cpp_class_get_field_from_name(player->Il2CppClass.klass, "account");
	LOG_ASSERT(field != nullptr);
	auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, player);
	//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
	static FieldInfo* field1 = il2cpp_class_get_field_from_name(account->Il2CppClass.klass, "loginStatus");
	auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
	il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
}

void dEOSManager_StartInitialLoginFlow(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_StartInitialLoginFlow executed");
	if (!State.SpoofGuestAccount) {
		EOSManager_StartInitialLoginFlow(__this, method);
		return;
	}
	EOSManager_StartTempAccountFlow(__this, method);
	EOSManager_CloseStartupWaitScreen(__this, method);
}

void dEOSManager_LoginFromAccountTab(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_LoginFromAccountTab executed");
	EOSManager_LoginFromAccountTab(__this, method);
	if (State.SpoofGuestAccount) {
		LOG_DEBUG("Faking login");
		fakeSuccessfulLogin(__this);
	}
}

void dEOSManager_InitializePlatformInterface(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_InitializePlatformInterface executed");
	EOSManager_InitializePlatformInterface(__this, method);
	//LOG_DEBUG("Skipping device identification");
	__this->fields.platformInitialized = true;
}

bool dEOSManager_IsFreechatAllowed(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_IsFreechatAllowed executed");
	return true;//app::EOSManager_IsFreechatAllowed(__this, method);
}

bool dEOSManager_IsFriendsListAllowed(EOSManager* __this, MethodInfo* method)
{
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_IsFriendsListAllowed executed");
	return true;//app::EOSManager_IsFriendsListAllowed(__this, method);
}

void dEOSManager_UpdatePermissionKeys(EOSManager* __this, void* callback, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_UpdatePermissionKeys executed");
	/*Il2CppClass* klass = get_class("Assembly-CSharp, EOSManager");
	LOG_ASSERT(klass);
	FieldInfo* field = il2cpp_class_get_field_from_name(klass, "isKWSMinor");
	LOG_ASSERT(field);
	bool value = false;
	il2cpp_field_set_value((Il2CppObject*)__this, field, &value);*/

	app::EOSManager_UpdatePermissionKeys(__this, callback, method);
}

void dEOSManager_Update(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_Update executed");
	static bool hasDeletedDeviceId = false;
	//__this->fields.ageOfConsent = 0; //why tf does amogus have an age of consent lmao
	if (State.SpoofFriendCode) __this->fields.friendCode = convert_to_string(State.FakeFriendCode);
	EOSManager_Update(__this, method);
	//EOSManager_set_FriendCode(__this, __this->fields.friendCode, NULL);
	if (State.SpoofGuestAccount) {
		auto player = app::DataManager_get_Player(nullptr);
		static FieldInfo* field = il2cpp_class_get_field_from_name(player->Il2CppClass.klass, "account");
		LOG_ASSERT(field != nullptr);
		auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, player);
		//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
		static FieldInfo* field1 = il2cpp_class_get_field_from_name(account->Il2CppClass.klass, "loginStatus");
		auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
		auto loggedOut = EOSManager_AccountLoginStatus__Enum::Offline;
		if ((int)il2cpp_field_get_value_object(field1, (Il2CppObject*)account) != (int)loggedOut)
			il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
		/*if (State.UseGuestFriendCode && State.GuestFriendCode != "") {
			auto username = __this->fields.editAccountUsername;
			TMP_Text_set_text((TMP_Text*)username->fields.UsernameText, convert_to_string(State.GuestFriendCode), NULL);
			//EditAccountUsername_SaveUsername(username, NULL);
		}*/
		if (__this->fields.hasRunLoginFlow && !hasDeletedDeviceId) {
			EOSManager_DeleteDeviceID(__this, NULL, NULL);
			LOG_DEBUG("Successfully deleted device ID!");
			hasDeletedDeviceId = true;
		}
	}

	if (State.ForceLoginAsGuest) {
		auto player = app::DataManager_get_Player(nullptr);
		static FieldInfo* field = il2cpp_class_get_field_from_name(player->Il2CppClass.klass, "account");
		LOG_ASSERT(field != nullptr);
		auto account = (PlayerAccountData*)il2cpp_field_get_value_object(field, player);
		//PlayerAccountData_set_LoginStatus(account, EOSManager_AccountLoginStatus__Enum::LoggedIn, NULL);
		static FieldInfo* field1 = il2cpp_class_get_field_from_name(account->Il2CppClass.klass, "loginStatus");
		auto loggedIn = EOSManager_AccountLoginStatus__Enum::LoggedIn;
		auto loggedOut = EOSManager_AccountLoginStatus__Enum::Offline;
		if ((int)il2cpp_field_get_value_object(field1, (Il2CppObject*)account) != (int)loggedOut)
			il2cpp_field_set_value((Il2CppObject*)account, field1, &loggedIn);
		/*if (State.UseGuestFriendCode && State.GuestFriendCode != "") {
			auto username = __this->fields.editAccountUsername;
			TMP_Text_set_text((TMP_Text*)username->fields.UsernameText, convert_to_string(State.GuestFriendCode), NULL);
			//EditAccountUsername_SaveUsername(username, NULL);
		}*/
		if (__this->fields.hasRunLoginFlow && !hasDeletedDeviceId) {
			EOSManager_DeleteDeviceID(__this, NULL, NULL);
			LOG_DEBUG("Successfully deleted device ID!");
			hasDeletedDeviceId = true;
		}
		State.ForceLoginAsGuest = false; //button behavior
	}

	if (State.SpoofLevel) {
		auto player = DataManager_get_Player(NULL);
		auto stats = PlayerData_get_Stats(player, NULL);
		stats->fields.level = State.FakeLevel - 1;
		AbstractSaveData_Save((AbstractSaveData*)player, NULL);
	}
}

String* dEOSManager_get_ProductUserId(EOSManager* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEOSManager_get_ProductUserId executed");
	//if (State.SpoofPuid && State.FakePuid != "") return convert_to_string(State.FakePuid);
	return EOSManager_get_ProductUserId(__this, method);
}

void dPlatformSpecificData_Serialize(PlatformSpecificData* __this, MessageWriter* writer, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dPlatformSpecificData_Serialize executed");
	if (State.SpoofPlatform) __this->fields.Platform = Platforms__Enum(State.FakePlatform + 1);
	PlatformSpecificData_Serialize(__this, writer, method);
}

void dEditAccountUsername_SaveUsername(EditAccountUsername* __this, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dEditAccountUsername_SaveUsername executed");
	if (State.UseGuestFriendCode && State.GuestFriendCode != "")
		TMP_Text_set_text((TMP_Text*)__this->fields.UsernameText, convert_to_string(State.GuestFriendCode), NULL);
	else {
		auto textStr = TMP_Text_get_text((TMP_Text*)__this->fields.UsernameText, NULL);
		if (textStr != convert_to_string("")) {
			std::string newFriendCode = "";
			for (auto i : convert_from_string(textStr)) {
				newFriendCode += tolower(i);
			}
			TMP_Text_set_text((TMP_Text*)__this->fields.UsernameText, convert_to_string(newFriendCode), NULL);
		}
		else {
			std::string newFriendCode = "";
			std::string randomString = GenerateRandomString();
			for (auto i : randomString) {
				newFriendCode += tolower(i);
			}
			TMP_Text_set_text((TMP_Text*)__this->fields.UsernameText, convert_to_string(newFriendCode), NULL);
		}
	}
	EditAccountUsername_SaveUsername(__this, method);
}