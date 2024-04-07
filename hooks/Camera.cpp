#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "esp.hpp"
#include <iostream>

Vector3 dCamera_ScreenToWorldPoint(Camera* __this, Vector3 position, MethodInfo* method)
{
	try {
		if (IsInGame() || IsInLobby()) {
			auto chat = Game::HudManager.GetInstance()->fields.Chat;

			if (!State.PanicMode && State.EnableZoom)
			{	//Figured it is better to restore the current camera height than using state
				float orthographicSize = Camera_get_orthographicSize(__this, NULL);
				Camera_set_orthographicSize(__this, 3.f, NULL);

				//only change on closing as chat gets closed otherwise
				if (chat->fields.state == ChatControllerState__Enum::Closing) {
					int32_t width = Screen_get_width(NULL);
					int32_t height = Screen_get_height(NULL);
					bool fullscreen = Screen_get_fullScreen(NULL);
					ChatController_OnResolutionChanged(chat, (float)(width / height), width, height, fullscreen, NULL);
					ChatController_ForceClosed(chat, NULL); //force close the chat as it stays open otherwise
				}

				Vector3 ret = Camera_ScreenToWorldPoint(__this, position, method);
				Camera_set_orthographicSize(__this, orthographicSize, NULL);
				return ret;
			}
			//fix chat button disappearing after disabling zoom
			if (chat->fields.state == ChatControllerState__Enum::Closed) {
				int32_t width = Screen_get_width(NULL);
				int32_t height = Screen_get_height(NULL);
				bool fullscreen = Screen_get_fullScreen(NULL);
				ChatController_OnResolutionChanged(chat, (float)(width / height), width, height, fullscreen, NULL);
				ChatController_ForceClosed(chat, NULL);
			}
		}
		else return Camera_ScreenToWorldPoint(__this, position, method);
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in Camera_ScreenToWorldPoint (Camera)"); //better safe than sorry
	}

	return Camera_ScreenToWorldPoint(__this, position, method);
}

void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method) {
	try {
		if (!State.PanicMode) {
			if (auto playerToFollow = State.playerToFollow.validate(); playerToFollow.has_value())
			{
				__this->fields.Target = (MonoBehaviour*)playerToFollow.get_PlayerControl();
			}
			else if (__this->fields.Target != (MonoBehaviour*)(*Game::pLocalPlayer)) {
				__this->fields.Target = (MonoBehaviour*)(*Game::pLocalPlayer);
			}
		}
		else __this->fields.Target = (MonoBehaviour*)(*Game::pLocalPlayer);
	}
	catch (...) {
		LOG_DEBUG("Exception occurred in FollowerCamera_Update (Camera)");
	}
	FollowerCamera_Update(__this, method);
}