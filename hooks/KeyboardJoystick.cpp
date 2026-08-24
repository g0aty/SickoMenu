#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

void dKeyboardJoystick_Update(KeyboardJoystick* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dKeyboardJoystick_Update executed", false);

    auto hud = Game::HudManager.GetInstance();

    auto hudGameObject = Component_get_gameObject((Component_1*)hud, NULL);
    auto shadowGameObject = Component_get_gameObject((Component_1*)hud->fields.ShadowQuad, NULL);

    bool isKillOverlayActive = hud->fields.KillOverlay != NULL &&
        KillOverlay_get_IsOpen((KillOverlay*)hud->fields.KillOverlay, NULL);

    if (hudGameObject != NULL && !isKillOverlayActive && !State.HasRefreshedUI) {
        // this is done here to not disable movement while zooming in/out
        bool wasShadowQuadActive = shadowGameObject == NULL ? true : GameObject_GetActive(shadowGameObject, NULL);

        GameObject_SetActive(hudGameObject, false, NULL);
        GameObject_SetActive(hudGameObject, true, NULL);

        auto taskOverlay = (Component_1*)hud->fields.TaskCompleteOverlay;
        if (taskOverlay != NULL && Component_get_gameObject(taskOverlay, NULL) != NULL) {
            GameObject_SetActive(Component_get_gameObject(taskOverlay, NULL), false, NULL);
            // prevent the persistent "Task Completed!" text while zooming
        }

        if (shadowGameObject != NULL)
            GameObject_SetActive(shadowGameObject, wasShadowQuadActive, NULL);
    }

    if (ImGui::GetIO().WantTextInput) {
        __this->fields.del = app::Vector2(); //Typing in a SickoMenu text field, suppress movement input entirely
        return;
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