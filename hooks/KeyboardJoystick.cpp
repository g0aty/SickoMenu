#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dKeyboardJoystick_Update(KeyboardJoystick* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dKeyboardJoystick_Update executed", false);

    auto hudGameObject = Component_get_gameObject((Component_1*)Game::HudManager.GetInstance(), NULL);
    auto shadowGameObject = Component_get_gameObject((Component_1*)Game::HudManager.GetInstance()->fields.ShadowQuad, NULL);
    if (hudGameObject != NULL && !State.HasRefreshedUI) {
        // this is done here to not disable movement while zooming in/out
        bool wasShadowQuadActive = shadowGameObject == NULL ? true : GameObject_GetActive(shadowGameObject, NULL);

        GameObject_SetActive(hudGameObject, false, NULL);
        GameObject_SetActive(hudGameObject, true, NULL);

        if (shadowGameObject != NULL)
            GameObject_SetActive(shadowGameObject, wasShadowQuadActive, NULL);
    }

    if ((!State.FreeCam && !State.playerToAttach.has_value()) || State.PanicMode) {
        app::KeyboardJoystick_Update(__this, method);
    }
    else
        __this->fields.del = app::Vector2(); //Reset to zero
}

void dScreenJoystick_FixedUpdate(ScreenJoystick* __this, MethodInfo* method)
{
    if (State.ShowHookLogs) Log.HookDebug("Hook dScreenJoystick_FixedUpdate executed", false);
    static int countdown;
    if ((!State.FreeCam && !State.playerToAttach.has_value()) || State.PanicMode) {
        app::ScreenJoystick_FixedUpdate(__this, method);
        countdown = 3; //This is necessary for mouseup to zero out the delta movement
    }
    else if (countdown > 0) {
        app::ScreenJoystick_FixedUpdate(__this, method);
        countdown--;
    }
    
}