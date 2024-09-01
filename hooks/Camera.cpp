#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "esp.hpp"
#include <iostream>

static float camHeight = 3.f;
static bool refreshChat = true;

Vector3 dCamera_ScreenToWorldPoint(Camera* __this, Vector3 position, MethodInfo* method)
{
	try {
		if (!State.PanicMode && (IsInGame() || IsInLobby()))
		{
			auto chatState = Game::HudManager.GetInstance()->fields.Chat->fields.state;
			bool chatOpen = chatState == ChatControllerState__Enum::Open || chatState == ChatControllerState__Enum::Opening || chatState == ChatControllerState__Enum::Closing;
			auto hudCamera = Game::HudManager.GetInstance()->fields.UICamera;/*//Figured it is better to restore the current camera height than using state
			float orthographicSize = Camera_get_orthographicSize(__this, NULL);
			Camera_set_orthographicSize(__this, 3.0f, NULL);
			Vector3 ret = Camera_ScreenToWorldPoint(__this, position, method);
			Camera_set_orthographicSize(__this, orthographicSize, NULL);
			return ret;*/
			float newCamHeight = 3.f * ((State.EnableZoom && !State.InMeeting && !chatOpen) ? State.CameraHeight : 1.f);
			if (camHeight != newCamHeight) {
				camHeight = newCamHeight;
				Camera_set_orthographicSize(__this, camHeight, NULL);
				Camera_set_orthographicSize(hudCamera, camHeight, NULL);
				Screen_SetResolution_1(Screen_get_width(NULL), Screen_get_height(NULL), Screen_get_fullScreen(NULL), 165, NULL);
			}
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in Camera_ScreenToWorldPoint (Camera)"); //better safe than sorry
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
		LOG_ERROR("Exception occurred in FollowerCamera_Update (Camera)");
	}
	FollowerCamera_Update(__this, method);
}