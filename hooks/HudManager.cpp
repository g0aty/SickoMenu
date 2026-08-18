#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"
#include "utility.h"
#include "game.h"

static std::string strToLower(std::string str) {
    std::string new_str = "";
    for (auto i : str) {
        new_str += char(std::tolower(i));
    }
    return new_str;
}

static std::string strRev(std::string str) {
    std::string new_str = str;
    std::reverse(new_str.begin(), new_str.end());
    return new_str;
}

void SetHoverStateBasedColors(Transform* transform, Color col, bool capitalizeBackground = true) {
    if (transform == NULL) return;

    static std::string spriteRendererTypeName = translate_type_name("UnityEngine.SpriteRenderer, UnityEngine.CoreModule");
    Type* spriteRendererType = app::Type_GetType(convert_to_string(spriteRendererTypeName), NULL);

    auto backgroundChild = (Component_1*)Transform_FindChild(transform,
        convert_to_string(capitalizeBackground ? "Background" : "background"), NULL);
    auto backgroundSprite = backgroundChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(backgroundChild, spriteRendererType, NULL);

    auto inactiveChild = (Component_1*)Transform_FindChild(transform, convert_to_string("Inactive"), NULL);
    auto inactiveSprite = inactiveChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(inactiveChild, spriteRendererType, NULL);

    auto activeChild = (Component_1*)Transform_FindChild(transform, convert_to_string("Active"), NULL);
    auto activeSprite = activeChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(activeChild, spriteRendererType, NULL);

    auto selectedChild = (Component_1*)Transform_FindChild(transform, convert_to_string("Selected"), NULL);
    auto selectedSprite = selectedChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(selectedChild, spriteRendererType, NULL);

    if (backgroundSprite != NULL)
        SpriteRenderer_set_color(backgroundSprite, col, NULL);

    if (inactiveSprite != NULL)
        SpriteRenderer_set_color(inactiveSprite, col, NULL);

    /*if (activeSprite != NULL)
        SpriteRenderer_set_color(activeSprite, col, NULL);

    if (selectedSprite != NULL)
        SpriteRenderer_set_color(selectedSprite, col, NULL);*/
}

void ChangeOtherHudObjectColors(HudManager* hudManager, Color col) {
    float brighteningFactor = 2.36f;
    if (col.r * brighteningFactor <= 1 && col.g * brighteningFactor <= 1 && col.b * brighteningFactor <= 1) {
        col.r *= brighteningFactor;
        col.g *= brighteningFactor;
        col.b *= brighteningFactor;
    }

    if (hudManager == NULL) return;

    // https://github.com/xChipseq/VanillaEnhancements/blob/main/VanillaEnhancements/Patches/DarkModePatches.cs

    auto mapButton = hudManager->fields.MapButton;
    auto mapButtonTransform = mapButton == NULL ? NULL : Component_get_transform((Component_1*)mapButton, NULL);

    auto settingsButtonObj = hudManager->fields.SettingsButton;
    static std::string passiveButtonTypeName = translate_type_name("PassiveButton, Assembly-CSharp");
    Type* passiveButtonType = app::Type_GetType(convert_to_string(passiveButtonTypeName), NULL);
    auto settingsButton = (PassiveButton*)GameObject_GetComponent(settingsButtonObj, passiveButtonType, NULL);
    auto settingsButtonTransform = settingsButton == NULL ? NULL : Component_get_transform((Component_1*)settingsButton, NULL);

    SetHoverStateBasedColors(mapButtonTransform, col);
    SetHoverStateBasedColors(settingsButtonTransform, col);
}

void ChangeFriendsListButtonColors(FriendsListButton* friendsListButton, Color col) {
    float brighteningFactor = 2.36f;
    if (col.r * brighteningFactor <= 1 && col.g * brighteningFactor <= 1 && col.b * brighteningFactor <= 1) {
        col.r *= brighteningFactor;
        col.g *= brighteningFactor;
        col.b *= brighteningFactor;
    }

    if (friendsListButton == NULL) return;

    // https://github.com/xChipseq/VanillaEnhancements/blob/main/VanillaEnhancements/Patches/DarkModePatches.cs

    auto buttonObj = friendsListButton->fields.Button;
    static std::string passiveButtonTypeName = translate_type_name("PassiveButton, Assembly-CSharp");
    Type* passiveButtonType = app::Type_GetType(convert_to_string(passiveButtonTypeName), NULL);
    auto button = (PassiveButton*)GameObject_GetComponent(buttonObj, passiveButtonType, NULL);
    auto buttonTransform = button == NULL ? NULL : Component_get_transform((Component_1*)button, NULL);

    SetHoverStateBasedColors(buttonTransform, col, false);
}

void dHudManager_Update(HudManager* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dHudManager_Update executed", false);
    try {
        static bool bChatAlwaysActivePrevious = false;
        if (bChatAlwaysActivePrevious != State.ChatAlwaysActive)
        {
            if (State.ChatAlwaysActive && !State.PanicMode)
                ChatController_SetVisible(__this->fields.Chat, true, NULL);
            else if (!State.InMeeting && !IsInLobby()) //You will lose chat ability in meeting otherwise
                ChatController_SetVisible(__this->fields.Chat, State.ChatActiveOriginalState, NULL);
            bChatAlwaysActivePrevious = State.ChatAlwaysActive;
        }
        if (__this->fields.PlayerCam)
            __this->fields.PlayerCam->fields.Locked = State.FreeCam && !State.PanicMode;


        static bool DisableActivation = false; //so a ghost seek button doesn't show up

        if (State.InMeeting)
            HudManager_SetHudActive(__this, false, NULL);
        else {
            auto hudTransform = Component_get_transform((Component_1*)__this, NULL);
            if (State.DisableHud && !State.PanicMode) {
                HudManager_SetHudActive(__this, false, NULL);
                Transform_set_localScale(hudTransform, { 0.f, 0.f, 0.f }, NULL);
                DisableActivation = false;
            }
            else if (!DisableActivation) {
                HudManager_SetHudActive(__this, true, NULL);
                Transform_set_localScale(hudTransform, { 1.f, 1.f, 1.f }, NULL);
                DisableActivation = true;
            }
        }

        Color uiCol = (!State.PanicMode && State.DarkMode) ? Color(0.5f, 0.5f, 0.5f, 1.f) :
            Palette__TypeInfo->static_fields->White;

        ChangeOtherHudObjectColors(__this, uiCol);

        if (IsInGame() || IsInLobby()) {
            auto localData = GetPlayerData(*Game::pLocalPlayer);
            GameObject* shadowLayerObject = Component_get_gameObject((Component_1*)__this->fields.ShadowQuad, NULL);
            float camHeight = State.FollowerCam == NULL ? 3.f : Camera_get_orthographicSize(State.FollowerCam, NULL);
            bool hideZoomShadows = State.EnableZoom && !State.EnableZoom_ShowShadows;
            bool shouldShowShadowQuad = (State.PanicMode || !(State.IsRevived || State.FreeCam || hideZoomShadows || State.playerToFollow.has_value() || State.Wallhack || (State.MaxVision && IsInLobby())))
                && (localData != NULL && !localData->fields.IsDead);
            if (shadowLayerObject != NULL)
                GameObject_SetActive(shadowLayerObject, shouldShowShadowQuad, NULL);
            if (!localData) {
                // oops: game bug
                return;
            }

            static int initialOutfitCooldown = 0;
            if (State.OutfitCooldown == 0) {
                if (State.PanicMode && State.TempPanicMode) {
                    State.PanicMode = false;
                    State.TempPanicMode = false;
                }
                if (!State.CanChangeOutfit && IsInLobby() && !State.PanicMode && State.confuser && State.confuseOnJoin)
                    ControlAppearance(true);
                State.CanChangeOutfit = true;
                if (State.ProGamer) {
                    std::string rofl = "sesaeler/uneMokciS/yta0g/moc.buhtig//:sptth morf unem eht dedaolnwod ev'uoy erus ekaM\n.uneMokciS fo noisrev dezirohtuanu na gnisu ma I";
                    rofl = strRev(rofl);
                    PlayerControl_RpcSendChat(*Game::pLocalPlayer, convert_to_string(rofl), NULL);
                    CustomNetworkTransform_RpcSnapTo((*Game::pLocalPlayer)->fields.NetTransform, app::Vector2(0.f, 0.f), NULL);
                    (*Game::pLocalPlayer)->fields.moveable = false;
                    InnerNetClient_DisconnectInternal((InnerNetClient*)(*Game::pAmongUsClient), DisconnectReasons__Enum::Sanctions, convert_to_string(rofl), NULL);
                    InnerNetClient_EnqueueDisconnect((InnerNetClient*)(*Game::pAmongUsClient), DisconnectReasons__Enum::Sanctions, convert_to_string(rofl), NULL);
                    State.OutfitCooldown = GetFps();
                    initialOutfitCooldown = State.OutfitCooldown;
                    if (State.PanicMode && State.TempPanicMode) {
                        State.PanicMode = false;
                        State.TempPanicMode = false;
                    }
                }
            }
            else if (State.OutfitCooldown == (int)(initialOutfitCooldown / 2)) {
                if (State.PanicMode && State.TempPanicMode) {
                    State.PanicMode = false;
                    State.TempPanicMode = false;
                }
                ChatController_SetVisible(__this->fields.Chat, true, NULL);
                State.OutfitCooldown--;
            }
            else State.OutfitCooldown--;

            if (!State.InMeeting && !State.DisableHud)
            {
                app::RoleBehaviour* playerRole = localData->fields.Role; // Nullable
                app::RoleTypes__Enum role = playerRole != nullptr ? playerRole->fields.Role : app::RoleTypes__Enum::Crewmate;
                GameObject* ImpostorVentButton = app::Component_get_gameObject((Component_1*)__this->fields.ImpostorVentButton, NULL);

                if (ImpostorVentButton != NULL) {
                    if (role == RoleTypes__Enum::Engineer && State.UnlockVents && !State.PanicMode)
                    {
                        app::EngineerRole* engineerRole = (app::EngineerRole*)playerRole;
                        if (engineerRole->fields.cooldownSecondsRemaining > 0.0f)
                            engineerRole->fields.cooldownSecondsRemaining = 0.01f; //This will be deducted below zero on the next FixedUpdate call
                        engineerRole->fields.inVentTimeRemaining = 30.0f; //Can be anything as it will always be written
                    }
                    else if ((GetPlayerData(*Game::pLocalPlayer)->fields.IsDead || IsInLobby()))
                    {
                        app::GameObject_SetActive(ImpostorVentButton, false, nullptr);
                    }
                    else
                    {
                        app::GameObject_SetActive(ImpostorVentButton, (State.UnlockVents && !State.PanicMode) || (((*Game::pLocalPlayer)->fields.inVent && role != RoleTypes__Enum::Engineer)) || (PlayerIsImpostor(localData) && GameOptions().GetGameMode() == GameModes__Enum::Normal), nullptr);
                    }
                }

                if ((IsInGame() || (IsInLobby() && State.KillInLobbies))) {
                    bool amImpostor = false;
                    try {
                        amImpostor = PlayerIsImpostor(localData);
                    }
                    catch (...) {
                        LOG_ERROR("Exception occured while fetching whether player is impostor or not.");
                    }

                    for (auto player : GetAllPlayerControl())
                    {
                        auto playerInfo = GetPlayerData(player);
                        if (!playerInfo) break; //This happens sometimes during loading

                        if ((!IsInLobby()) && !State.PanicMode && State.KillImpostors && !playerInfo->fields.IsDead && amImpostor)
                            playerInfo->fields.Role->fields.CanBeKilled = true;
                        else if (PlayerIsImpostor(playerInfo))
                            playerInfo->fields.Role->fields.CanBeKilled = false;
                    }
                    GameObject* KillButton = app::Component_get_gameObject((Component_1*)__this->fields.KillButton, NULL);
                    if (KillButton != NULL && (IsInGame())) {
                        if ((!State.PanicMode && State.UnlockKillButton && (IsHost() || !State.SafeMode) && !localData->fields.IsDead) || amImpostor) {
                            app::GameObject_SetActive(KillButton, true, nullptr);
                            playerRole->fields.CanUseKillButton = true;
                        }
                        else {
                            app::GameObject_SetActive(KillButton, false, nullptr);
                            playerRole->fields.CanUseKillButton = false;
                        }
                    }
                    else if (KillButton != NULL && IsInLobby()) {
                        app::GameObject_SetActive(KillButton, false, nullptr);
                    }
                }
            }
        }
    }
    catch (...) {
        //LOG_ERROR("Exception occurred in HudManager_Update (HudManager)");
    }

    HudManager_Update(__this, method);
}

void dVersionShower_Start(VersionShower* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dVersionShower_Start executed", false);
    State.versionShower = __this;
    VersionShower_Start(__this, method);
    State.versionShowerDefaultText = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = {};
    localtime_s(&tm, &t);  // Safe version of localtime
    std::ostringstream oss;
    oss << std::put_time(&tm, "%m-%d");
    if (oss.str() == "04-01") State.AprilFoolsMode = true;

    std::string wtf = "lld.unemokcis";
    std::string xd = "lld.noisrev";
    wtf = strRev(wtf);
    xd = strRev(xd);
    std::string lmao = strToLower(State.lol);

    if (lmao != wtf && lmao != xd) {
        State.ProGamer = true;
        if (!State.TempPanicMode) State.PanicMode = false;
        State.HideWatermark = false;
    }

    if (State.PanicMode) return;

    int watermarkSize = 100;
    if (!State.HideWatermark) {
        if (State.CurrentScene == "FindAGame") watermarkSize = 60;
        else if (State.CurrentScene == "MainMenu") watermarkSize = 75;
    }
    std::string spoofVersionText = "";
    if (State.SpoofAUVersion && !State.HideWatermark) {
        switch (State.FakeAUVersion) {
        case 0: // AU v16.0.0 / v16.0.2
            spoofVersionText = " <#fb0>[Spoofing v16.0.2]</color>";
            break;
        case 1: // AU v16.0.5 / v16.1.0
            spoofVersionText = " <#fb0>[Spoofing v16.1.0]</color>";
            break;
        }
    }
    std::string disableHostAnticheatText = State.CurrentScene == "FindAGame" && State.DisableHostAnticheat ? " • <#f00>+25 Mode is ON</color>" : "";
    std::string watermarkOffset = State.CurrentScene == "MMOnline" ? "<#0000>00000</color>" : "";
    std::string sickoText = "<#ff006c>SickoMenu</color>";
    std::string goatText = "<#ef0143>g0aty</color>";
    std::string sickoVersionText = "<#fb0>" + State.SickoVersion + "</color>";
    if (State.SickoVersion.find("pr") != std::string::npos || State.SickoVersion.find("rc") != std::string::npos) {
        sickoVersionText = "<#a700ff>" + State.SickoVersion + "</color>";
    }
    /*if (!State.HideWatermark) {
        sickoText = GetGradientUsername("SickoMenu", ImVec4(1.f, 0.f, 0.424f, 1.f), ImVec4(0.502f, 0.075f, 0.256f, 1.f));
        goatText = GetGradientUsername("g0aty", ImVec4(0.937f, 0.004f, 0.263f, 1.f), ImVec4(0.529f, 0.008f, 0.157f, 1.f));
    }*/
    std::string watermarkText = /*State.AprilFoolsMode ? std::format(" • {} <#fb0>{}</color> <#ca08ff>[{} Mode]</color> by {}", sickoText,
        State.SickoVersion, State.DiddyPartyMode ? "Diddy Party" : (IsChatCensored() || IsStreamerMode() ? "F***son" : "Fuckson"), goatText) :*/
        std::format(" • {} <#fb0>{}</color> by {}", sickoText, sickoVersionText, goatText);
    const auto& versionText = std::format("<font=\"Barlow-Regular SDF\"><size={}%>{}{}{}{}{}{}</color></size></font>",
        watermarkSize, State.DarkMode ? "<#666>" : "<#fff>", State.versionShowerDefaultText, spoofVersionText,
        State.HideWatermark ? "" : watermarkText, disableHostAnticheatText, watermarkOffset);
    TMP_Text_set_text((TMP_Text*)State.versionShower->fields.text, convert_to_string(versionText), nullptr);
}

void dPingTracker_Update(PingTracker* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dPingTracker_Update executed", false);
    __this->fields.gamePos.x = 0.f, __this->fields.lobbyPos.x = 0.f; // Make the PingTracker actually look centered
    bool isFreeplay = ((InnerNetClient*)(*Game::pAmongUsClient))->fields.NetworkMode == NetworkModes__Enum::FreePlay;
    app::PingTracker_Update(__this, method);
    float initialYdist = IsInGame() ? __this->fields.gamePos.y : __this->fields.lobbyPos.y;
    // float camHeight = State.FollowerCam == NULL ? 3.f : Camera_get_orthographicSize(State.FollowerCam, NULL);
    // if (!State.PanicMode && State.EnableZoom) __this->fields.aspectPosition->fields.DistanceFromEdge.y = initialYdist;
    app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, app::TextAlignmentOptions__Enum::Top, nullptr);
    if (isFreeplay) {
        auto obj = (GameObject*)Component_get_gameObject((Component_1*)__this, NULL);
        if (!GameObject_GetActive(obj, NULL))
            GameObject_SetActive(obj, true, NULL);
        if ((State.PanicMode && !State.TempPanicMode) || State.OldStylePingText)
            return app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(""), nullptr);
        else {
            __this->fields.aspectPosition->fields.DistanceFromEdge = __this->fields.gamePos;
            // if (!State.PanicMode && State.EnableZoom) __this->fields.aspectPosition->fields.DistanceFromEdge.y = initialYdist;
        }
    }
    try {
        if (!State.PanicMode || State.TempPanicMode) {
            if (State.OldStylePingText) {
                // these calculations are based on 16:9 (2.5) and 4:3 (1.2) aspect ratios
                float aspectRatio = (float)Screen_get_width(NULL) / (float)Screen_get_height(NULL);
                float xOffset = aspectRatio * 2.925f - 2.7f;

                Vector3 oldDistFromEdge = Vector3(xOffset, 5.9f, 0.f);
                float camHeight = State.FollowerCam != NULL ? Camera_get_orthographicSize(State.FollowerCam, NULL) / 3.f : 1.f;
                // if (!State.PanicMode && State.EnableZoom)
                oldDistFromEdge.x *= camHeight * (camHeight * 0.425f + 0.575f);
                oldDistFromEdge.y *= camHeight;
                __this->fields.aspectPosition->fields.DistanceFromEdge = oldDistFromEdge;
            }
            std::string sep = State.OldStylePingText ? "\n" : " • ";
            std::string ping = convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
            static int fps = GetFps();
            static int fpsDelay = 0;
            if (fpsDelay <= 0 || GetFps() <= 30) {
                fps = GetFps();
                fpsDelay = int(0.5 * GetFps()); // 0.5 sec delay
            }
            else fpsDelay--;
            std::string fpsText = State.ShowFps ? sep : "";
            if (State.ShowFps) {
                if (fps <= 20) fpsText += std::format("<#f00>FPS: {}</color>", fps);
                else if (fps <= 40) fpsText += std::format("<#ff0>FPS: {}</color>", fps);
                else fpsText += std::format("<#0f0>FPS: {}</color>", fps);
            }
            std::string timeText = State.ShowTime ? sep + "<#b0f>Time: " +
                GetTimeString(State.UseLeadingZeroForHours, State.ShowSeconds) + "</color>" : "";
            std::string autoKill = State.AutoKill ? (sep + "<#f00>Autokill</color>") : "";
            std::string noClip = State.NoClip ? (sep + "NoClip") : "";
            std::string freeCam = State.FreeCam ? (sep + "Freecam") : "";
            std::string spectating = "";
            if (auto playerToFollow = State.playerToFollow.validate(); playerToFollow.has_value()) {
                app::NetworkedPlayerInfo_PlayerOutfit* outfit = GetPlayerOutfit(playerToFollow.get_PlayerData());
                Color32 playerColor = GetPlayerColor(outfit->fields.ColorId);
                std::string colorCode = std::format("<#{:02x}{:02x}{:02x}{:02x}>",
                    playerColor.r, playerColor.g, playerColor.b, playerColor.a);
                auto name = RemoveHtmlTags(convert_from_string(outfit->fields.PlayerName));
                if (name == "") spectating = sep + "Now Spectating";
                else spectating = sep + "Now Spectating: " + colorCode + name + "</color>";
            }
            uint8_t pingSize = 100;
            if (!State.OldStylePingText) {
                if (!State.HideWatermark || spectating != "") pingSize = 75;
                if (!State.HideWatermark && spectating != "") pingSize = 50;
            }
            std::string hostText = State.ShowHost && IsInGame() ?
                (IsHost() ? (sep + "You are Host") : std::format("{}Host: {}", sep, GetHostUsername(true))) : "";
            std::string voteKicksText = (State.ShowVoteKicks && State.VoteKicks > 0) ? std::format("{}Vote Kicks: {}", sep, State.VoteKicks) : "";
            std::string sickoText = "";
            std::string versionText = "";
            std::string goatText = "";
            if (!State.HideWatermark) {
                static uint8_t gradientOffset = 0;
                static int gradientDelay = 0;
                sickoText = GetGradientUsername("SickoMenu", ImVec4(1.f, 0.f, 0.424f, 1.f), ImVec4(0.502f, 0.075f, 0.256f, 1.f), gradientOffset);

                if (State.SickoVersion.find("pr") != std::string::npos || State.SickoVersion.find("rc") != std::string::npos) {
                    versionText = GetGradientUsername(State.SickoVersion, ImVec4(0.656f, 0.f, 1.f, 1.f), ImVec4(0.334f, 0.f, 0.624f, 1.f), gradientOffset);
                }
                else versionText = "<#fb0>" + State.SickoVersion + "</color>";

                goatText = GetGradientUsername("g0aty", ImVec4(0.937f, 0.004f, 0.263f, 1.f), ImVec4(0.529f, 0.008f, 0.157f, 1.f), gradientOffset);
                if (gradientDelay <= 0) {
                    gradientOffset++;
                    gradientDelay = (int)(0.1 * fps);
                }
                else gradientDelay--;
            }

            int overflowTimer = (int)std::ceilf(State.OverflowTimer);
            std::string overflowText = IsInLobby() && State.Overflow ? std::format("\n<#f00>Overflow Countdown: 00:{}{}</color>",
                overflowTimer < 10 ? "0" : "", overflowTimer) : "";

            std::string watermarkText = /*State.AprilFoolsMode ? std::format("<size={}%>{} <#fb0>{}</color> <#ca08ff>[{} Mode]</color> by {}{}",
                IsInGame() ? pingSize : 100, sickoText, State.SickoVersion, State.DiddyPartyMode ? "Diddy Party" : (IsChatCensored() || IsStreamerMode() ? "F***son" : "Fuckson"), goatText, sep) :*/
                std::format("<size={}%>{} {} by {}{}", IsInGame() ? pingSize : 100, sickoText, versionText, goatText, sep);
            std::string pingText = (isFreeplay && !State.OldStylePingText ? "<size=150%><#0000>0</color></size>\n" : "") +
                std::format("{}{}{}{}{}{}{}{}{}{}{}{}</color></size>", State.DarkMode ? "<#666>" : "<#fff>",
                    State.HideWatermark ? "" : watermarkText, ping, fpsText, timeText, hostText, voteKicksText, autoKill, noClip, freeCam, spectating, overflowText);
            app::TMP_Text_set_alignment((app::TMP_Text*)__this->fields.text, State.OldStylePingText ? 
                app::TextAlignmentOptions__Enum::TopRight : app::TextAlignmentOptions__Enum::Top, nullptr);
            app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(pingText), nullptr);
        }
        else {
            std::string ping = (isFreeplay ? "<size=150%><#0000>0</color></size>\n" : "") +
                convert_from_string(app::TMP_Text_get_text((app::TMP_Text*)__this->fields.text, nullptr));
            app::TMP_Text_set_text((app::TMP_Text*)__this->fields.text, convert_to_string(ping), nullptr);
        }
        //"<#0000>00 00</color>" has been added to center the ping text
    }
    catch (...) {
        LOG_ERROR("Exception occurred in PingTracker_Update (HudManager)");
    }
}

bool dLogicGameFlowNormal_IsGameOverDueToDeath(LogicGameFlowNormal* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dLogicGameFlowNormal_IsGameOverDueToDeath executed", false);
    return false; //fix black screen when you set fake role
}
bool dLogicGameFlowHnS_IsGameOverDueToDeath(LogicGameFlowHnS* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dLogicGameFlowHnS_IsGameOverDueToDeath executed", false);
    return false; //fix black screen when you set fake role
}

void dModManager_LateUpdate(ModManager* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dModManager_LateUpdate executed", false);
    ModManager_LateUpdate(__this, method);

    ModManager_ShowModStamp(__this, NULL);

    static FieldInfo* field = il2cpp_class_get_field_from_name(((Il2CppObject*)__this)->klass, "ModStamp");
    if (field == nullptr) return;
    auto modStampSprite = (SpriteRenderer*)il2cpp_field_get_value_object(field, (Il2CppObject*)__this);

    bool shouldShowModStamp = !State.PanicMode && !State.DisableHud && !State.HideWatermark;
    SpriteRenderer_set_color(modStampSprite, Color(1.f, 1.f, 1.f, shouldShowModStamp ? 0.498f : 0.f), NULL);
}

void dEndGameNavigation_ShowDefaultNavigation(EndGameNavigation* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dEndGameNavigation_ShowDefaultNavigation executed", false);
    EndGameNavigation_ShowDefaultNavigation(__this, method);
}

void dFriendsListUI_UpdateFriendCodeUI(FriendsListUI* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dFriendsListUI_UpdateFriendCodeUI executed", false);
    FriendsListUI_UpdateFriendCodeUI(__this, method);
}

void dMapCountOverlay_OnEnable(MapCountOverlay* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dMapCountOverlay_OnEnable executed", false);
    State.IsAdminMapOpen = true;
    MapCountOverlay_OnEnable(__this, method);
}

void dMapCountOverlay_OnDisable(MapCountOverlay* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dMapCountOverlay_OnDisable executed", false);
    State.IsAdminMapOpen = false;
    MapCountOverlay_OnDisable(__this, method);
}

void* dIntroCutscene_ShowTeam(IntroCutscene* __this, List_1_PlayerControl_* teamToShow, float duration, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dIntroCutscene_ShowTeam executed", false);
    return IntroCutscene_ShowTeam(__this, teamToShow, duration, method);
}

void dEndGameManager_ShowButtons(EndGameManager* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dEndGameManager_ShowButtons executed", false);
    EndGameManager_ShowButtons(__this, method);
    if (!State.PanicMode && State.AutoRejoin)
        EndGameNavigation_NextGame(__this->fields.Navigation, NULL);
}

void* dShhhBehaviour_PlayAnimation(ShhhBehaviour* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dShhhBehaviour_Update executed", false);
    if (!State.PanicMode && State.DisableShushAnimation) {
        auto shhhEmblemObject = app::Component_get_gameObject((Component_1*)Game::HudManager.GetInstance()->fields.shhhEmblem, NULL);
        if (shhhEmblemObject != NULL) {
            GameObject_SetActive(shhhEmblemObject, false, NULL);
        }
        return nullptr;
    }
    return ShhhBehaviour_PlayAnimation(__this, NULL);
}

void dFriendsListButton_Update(FriendsListButton* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dFriendsListButton_Update executed", false);
    FriendsListButton_Update(__this, method);

    Color uiCol = (!State.PanicMode && State.DarkMode) ? Color(0.5f, 0.5f, 0.5f, 1.f) :
        Palette__TypeInfo->static_fields->White;

    Color notifCol = (!State.PanicMode && State.DarkMode) ? Color(0.8f, 0.8f, 0.8f, 1.f) :
        Palette__TypeInfo->static_fields->White;

    ChangeFriendsListButtonColors(__this, uiCol);

    auto buttonObj = __this->fields.Button;
    static std::string passiveButtonTypeName = translate_type_name("PassiveButton, Assembly-CSharp");
    Type* passiveButtonType = app::Type_GetType(convert_to_string(passiveButtonTypeName), NULL);
    auto button = (PassiveButton*)GameObject_GetComponent(buttonObj, passiveButtonType, NULL);
    auto buttonTransform = button == NULL ? NULL : Component_get_transform((Component_1*)button, NULL);

    if (buttonTransform != NULL) {
        static std::string spriteRendererTypeName = translate_type_name("UnityEngine.SpriteRenderer, UnityEngine.CoreModule");
        Type* spriteRendererType = app::Type_GetType(convert_to_string(spriteRendererTypeName), NULL);

        auto notifChild = (Component_1*)Transform_FindChild(buttonTransform, convert_to_string("NotifCount"), NULL);
        auto notifSprite = notifChild == NULL ? NULL : (SpriteRenderer*)Component_GetComponent(notifChild, spriteRendererType, NULL);
        SpriteRenderer_set_color(notifSprite, notifCol, NULL);
    }
}

void dProgressTracker_FixedUpdate(ProgressTracker* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dProgressTracker_FixedUpdate executed", false);
    ProgressTracker_FixedUpdate(__this, method);

    auto ptTransform = Component_get_transform((Component_1*)__this, NULL);
    if (ptTransform == NULL) return;

    auto backgroundChild = (Component_1*)Transform_FindChild(ptTransform, convert_to_string("Background"), NULL);
    if (backgroundChild == NULL) return;

    static std::string spriteRendererTypeName = translate_type_name("UnityEngine.SpriteRenderer, UnityEngine.CoreModule");
    Type* spriteRendererType = app::Type_GetType(convert_to_string(spriteRendererTypeName), NULL);

    auto bgSprite = (SpriteRenderer*)Component_GetComponent(backgroundChild, spriteRendererType, NULL);
    if (bgSprite == NULL) return;

    Color ptCol = (!State.PanicMode && State.DarkMode) ? Color(0.2f, 0.2f, 0.2f, 1.f) : Palette__TypeInfo->static_fields->White;

    SpriteRenderer_set_color(bgSprite, ptCol, NULL);
}

void dHideAndSeekTimerBar_Update(HideAndSeekTimerBar* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dHideAndSeekTimerBar_Update executed", false);

    auto gameObject = Component_get_gameObject((Component_1*)__this, NULL);
    if (gameObject != NULL && !State.HasRefreshedUI) {
        // update the timer bar according to the camera zoom
        GameObject_SetActive(gameObject, false, NULL);
        GameObject_SetActive(gameObject, true, NULL);
    }

    HideAndSeekTimerBar_Update(__this, method);

    auto barTransform = Component_get_transform((Component_1*)__this, NULL);
    if (barTransform == NULL) return;

    auto backgroundChild = (Component_1*)Transform_FindChild(barTransform, convert_to_string("Background"), NULL);
    if (backgroundChild == NULL) return;

    static std::string spriteRendererTypeName = translate_type_name("UnityEngine.SpriteRenderer, UnityEngine.CoreModule");
    Type* spriteRendererType = app::Type_GetType(convert_to_string(spriteRendererTypeName), NULL);

    auto bgSprite = (SpriteRenderer*)Component_GetComponent(backgroundChild, spriteRendererType, NULL);
    if (bgSprite == NULL) return;

    Color barCol = (!State.PanicMode && State.DarkMode) ? Color(0.2f, 0.2f, 0.2f, 1.f) : Palette__TypeInfo->static_fields->White;

    SpriteRenderer_set_color(bgSprite, barCol, NULL);
}

void dLobbyInfoPane_Update(LobbyInfoPane* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dLobbyInfoPane_Update executed", false);
    LobbyInfoPane_Update(__this, method);

    // the following code is used as __this->fields.InfoPaneBackground throws null reference errors when directly used
    static FieldInfo* field = il2cpp_class_get_field_from_name(((Il2CppObject*)__this)->klass, "InfoPaneBackground");
    if (field == nullptr) return;
    auto bgSprite = (SpriteRenderer*)il2cpp_field_get_value_object(field, (Il2CppObject*)__this);

    Color bgCol = (!State.PanicMode && State.DarkMode) ? Color(0.5f, 0.5f, 0.5f, 1.f) : Palette__TypeInfo->static_fields->White;
    SpriteRenderer_set_color(bgSprite, bgCol, NULL);
}

void dShadowCollab_OnEnable(ShadowCollab* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dShadowCollab_OnEnable executed", false);
    ShadowCollab_OnEnable(__this, method);

    State.shadowCollab = __this;
}

void dPassiveButton_ReceiveClickDown(PassiveButton* __this, MethodInfo* method) {
    if (!State.ClickThroughMenuUI && ImGui::GetIO().WantCaptureMouse) return;
    PassiveButton_ReceiveClickDown(__this, method);
}

void dPassiveButton_ReceiveRepeatDown(PassiveButton* __this, MethodInfo* method) {
    if (!State.ClickThroughMenuUI && ImGui::GetIO().WantCaptureMouse) return;
    PassiveButton_ReceiveRepeatDown(__this, method);
}

void dPassiveButton_ReceiveClickUp(PassiveButton* __this, MethodInfo* method) {
    if (!State.ClickThroughMenuUI && ImGui::GetIO().WantCaptureMouse) return;
    PassiveButton_ReceiveClickUp(__this, method);
}

void dPassiveButton_ReceiveMouseOver(PassiveButton* __this, MethodInfo* method) {
    if (!State.ClickThroughMenuUI && ImGui::GetIO().WantCaptureMouse) return;
    PassiveButton_ReceiveMouseOver(__this, method);
}

void dMapBehaviour_FixedUpdate(MapBehaviour* __this, MethodInfo* method) {
    MapBehaviour_FixedUpdate(__this, method);
}

void dRoomTracker_FixedUpdate(RoomTracker* __this, MethodInfo* method) {
    RoomTracker_FixedUpdate(__this, method);
}

void ColorRoomTrackerText(RoomTracker* roomTracker) {
    // the reason that this isn't done in dRoomTracker_FixedUpdate is because
    // the slide in/out animations are hardcoded to start with white text
}