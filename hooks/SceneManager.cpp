#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dSceneManager_Internal_ActiveSceneChanged(Scene previousActiveScene, Scene newActiveScene, MethodInfo* method) {
	if (State.ShowHookLogs) LOG_DEBUG("Hook dSceneManager_Internal_ActiveSceneChanged executed");
	State.CurrentScene = convert_from_string(app::Scene_GetNameInternal(newActiveScene.m_Handle, NULL));
	app::SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene, method);
}