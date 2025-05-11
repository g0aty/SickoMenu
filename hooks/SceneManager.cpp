#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dSceneManager_Internal_ActiveSceneChanged(Scene previousActiveScene, Scene newActiveScene, MethodInfo* method) {
	//if (State.ShowHookLogs) LOG_DEBUG("Hook dSceneManager_Internal_ActiveSceneChanged executed");
	State.CurrentScene = convert_from_string(app::Scene_GetNameInternal(newActiveScene.m_Handle, NULL));
	LOG_DEBUG(("Scene changed to " + State.CurrentScene).c_str());
	if (State.CurrentScene == "MainMenu") {
		State.MainMenuLoaded = true;
		State.IsFreePlay = false;
	}
	app::SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene, method);
}