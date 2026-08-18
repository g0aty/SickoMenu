#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "esp.hpp"
#include <iostream>

static float camHeight = 3.f;
static bool refreshChat = true;

Vector3 dCamera_ScreenToWorldPoint(Camera* __this, Vector3 position, MethodInfo* method)
{
	if (State.ShowHookLogs) Log.HookDebug("Hook dCamera_ScreenToWorldPoint executed", false);
	/*try {
		if (!State.PanicMode && (State.GameLoaded || IsInLobby()))
		{
			//Figured it is better to restore the current camera height than using state
			float orthographicSize = Camera_get_orthographicSize(__this, NULL);
			Camera_set_orthographicSize(__this, 3.0f, NULL);
			Vector3 ret = Camera_ScreenToWorldPoint(__this, position, method);
			Camera_set_orthographicSize(__this, orthographicSize, NULL);
			return ret;
		}
	}
	catch (...) {
		LOG_ERROR("Exception occurred in Camera_ScreenToWorldPoint (Camera)"); //better safe than sorry
	}*/

	return Camera_ScreenToWorldPoint(__this, position, method);
}

void dFollowerCamera_Update(FollowerCamera* __this, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dFollowerCamera_Update executed", false);
	try {
		if (!State.PanicMode) {
			if (auto playerToFollow = State.playerToFollow.validate(); playerToFollow.has_value())
			{
				__this->fields.Target = (MonoBehaviour*)playerToFollow.get_PlayerControl();
				auto shadowCamTransform = Component_get_transform((Component_1*)State.shadowCollab->fields.ShadowCamera, NULL);
				auto camTransform = Component_get_transform((Component_1*)State.FollowerCam, NULL);
				Vector3 position = Transform_get_position(camTransform, NULL);
				Transform_set_position(shadowCamTransform, position, NULL);
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

void dScreen_SetResolution(int32_t width, int32_t height, bool fullscreen, MethodInfo* method) {
	if (State.ShowHookLogs) Log.HookDebug("Hook dScreen_SetResolution executed", false);
	if (State.EnableZoom_ResolutionSetFlag) {
		State.EnableZoom_ResolutionSetFlag = false;
		return;
	}
	Screen_SetResolution(width, height, fullscreen, method);
}