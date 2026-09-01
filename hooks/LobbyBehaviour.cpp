#include "pch-il2cpp.h"
#include "_hooks.h"
#include "state.hpp"

//std::unordered_set<std::string> glitchEndings = { "IJPG", "YTHG", "WYWG", "KHQG", "FUGG", "UFLG", "KJQG", "ZQCG", "GEWG", "NPPG", "SZAF", "PATG", "PJDG", "TPYG", "JTFG", "VDXG", "DHSG", "TQQG", "ALGG", "UMPG", "GFXG", "RGGG", "HQXG", "LDQG", "ZLHG", "WMPG", "TAGG", "FBGG", "EJYG", "AOTG", "LCAF", "DORG", "ZCQG" };

static bool s_pendingCosmeticApply = false;
static int s_pendingApplyHostPresetIndex = -1;

static std::string getHexCodeFromImVec4(ImVec4 vec) {
    return std::format("<#{:02x}{:02x}{:02x}{:02x}>",
        int(vec.x * 255), int(vec.y * 255), int(vec.z * 255), int(vec.w * 255));
}

void dLobbyBehaviour_Start(LobbyBehaviour* __this, MethodInfo* method)
{
    if (State.ShowHookLogs) Log.HookDebug("Hook dLobbyBehaviour_Start executed", false);
    LobbyBehaviour_Start(__this, method);
}

void dLobbyBehaviour_Update(LobbyBehaviour* __this, MethodInfo* method) {
    static bool hasStarted = true;
    if (State.ShowHookLogs) Log.HookDebug("Hook dLobbyBehaviour_Update executed", false);
    LobbyBehaviour_Update(__this, method);
    if (s_pendingCosmeticApply && IsInLobby()) {
        s_pendingCosmeticApply = false;
        if (!State.CosmeticPresets.empty()) {
            int idx = std::clamp(State.SelectedCosmeticPreset, 0, (int)State.CosmeticPresets.size() - 1);
            ApplyCosmeticPreset(State.CosmeticPresets[idx]);
        }
    }

    if (!State.JoinedLobby) {
        State.JoinedLobby = true;
        if (!State.PanicMode && State.AutoApplyCosmeticPreset && !State.CosmeticPresets.empty()) {
            s_pendingCosmeticApply = true;
        }

        if (IsHost()) {
            State.JoinedAsHost = true;
            if (!State.PanicMode && State.AutoApplyHostPreset && !State.HostPresets.empty()) {
                s_pendingApplyHostPresetIndex = std::clamp(State.SelectedHostPreset, 0, (int)State.HostPresets.size() - 1);
            }
        }
    }

    if (s_pendingApplyHostPresetIndex >= 0 && IsHost() && (IsInLobby() || IsInGame())) {
        int idx = s_pendingApplyHostPresetIndex;
        s_pendingApplyHostPresetIndex = -1;
        if (idx < (int)State.HostPresets.size())
            ApplyHostPreset(State.HostPresets[idx]);
    }
    if (State.DisableLobbyMusic) {
        hasStarted = false;
        SoundManager_StopSound(SoundManager__TypeInfo->static_fields->instance, (AudioClip*)__this->fields.MapTheme, NULL);
    }
    else if (!hasStarted) {
        hasStarted = true;
        LobbyBehaviour_Start(__this, method); //restart lobby music
    }
    /*if (GameOptions().GetByte(app::ByteOptionNames__Enum::MapId) == 3) {
        GameOptions().SetByte(app::ByteOptionNames__Enum::MapId, 0);
        if (GameOptionsManager_get_Instance && GameOptionsManager_get_CurrentGameOptions && GameOptionsManager_set_GameHostOptions
            && GameManager_get_Instance && GameManager_get_LogicOptions && LogicOptions_SyncOptions) {
            auto gameOptionsManager = GameOptionsManager_get_Instance(NULL);
            GameManager* gameManager = GameManager_get_Instance(NULL);
            if (gameOptionsManager != nullptr) {
                auto currentOptions = GameOptionsManager_get_CurrentGameOptions(gameOptionsManager, NULL);
                if (currentOptions != nullptr)
                    GameOptionsManager_set_GameHostOptions(gameOptionsManager, currentOptions, NULL);
            }
            if (gameManager != nullptr) {
                auto logicOptions = GameManager_get_LogicOptions(gameManager, NULL);
                if (logicOptions != nullptr)
                    LogicOptions_SyncOptions(logicOptions, NULL);
            }
        }
    }*/
}

void dMatchMakerGameButton_SetGame(MatchMakerGameButton* __this, GameListing gameListing, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dMatchMakerGameButton_SetGame executed", false);
    /*if (State.PanicMode || !State.ShowLobbyInfo) return MatchMakerGameButton_SetGame(__this, gameListing, method);
    MatchMakerGameButton_SetGame(__this, gameListing, method);
    auto platform = gameListing.Platform;
    std::string platformId = "Unknown";
    switch (platform) {
    case Platforms__Enum::StandaloneEpicPC:
        platformId = "Epic Games";
        break;
    case Platforms__Enum::StandaloneSteamPC:
        platformId = "Steam";
        break;
    case Platforms__Enum::StandaloneMac:
        platformId = "Mac";
        break;
    case Platforms__Enum::StandaloneWin10:
        platformId = "Microsoft Store";
        break;
    case Platforms__Enum::StandaloneItch:
        platformId = "itch.io";
        break;
    case Platforms__Enum::IPhone:
        platformId = "iOS/iPadOS";
        break;
    case Platforms__Enum::Android:
        platformId = "Android";
        break;
    case Platforms__Enum::Switch:
        platformId = "Nintendo Switch";
        break;
    case Platforms__Enum::Xbox:
        platformId = "Xbox";
        break;
    case Platforms__Enum::Playstation:
        platformId = "Playstation";
        break;
    default:
        platformId = "Unknown";
        break;
    }
    std::string lobbyCode = IsStreamerMode() ? "" : convert_from_string(InnerNet_GameCode_IntToGameName(gameListing.GameId, NULL));
    int LobbyTime = (std::max)(0, int(gameListing.Age));
    std::string lobbyTimeDisplay = "";
    if (State.ShowLobbyTimer) {
        lobbyTimeDisplay = std::format(" ~ <#0f0>Age: {}:{}{}</color>", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
    }
    std::string hostName = std::format("<size=50%>{} <#fb0>{}</color>\n<#b0f>{}</color>{}</size>", convert_from_string(gameListing.TrueHostName), lobbyCode, platformId, lobbyTimeDisplay, ServerMode);
    TMP_Text_set_text((TMP_Text*)__this->fields.NameText, convert_to_string(hostName), NULL);
    TMP_Text_set_text((TMP_Text*)__this->fields.SmallNameText, convert_to_string(hostName), NULL);
    */ // Deprecated
}

void dGameContainer_SetupGameInfo(GameContainer* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dGameContainer_SetupGameInfo executed", false);

    // Cache code- host mapping for all visible lobbies
    {
        auto& gl = __this->fields.gameListing;
        std::string code = convert_from_string(InnerNet_GameCode_IntToGameName(gl.GameId, NULL));
        std::string host = convert_from_string(gl.TrueHostName);
        if (!code.empty() && !host.empty())
            State.LobbyHostCache[code] = host;
    }

    if (State.PanicMode || !State.ShowLobbyInfo) return GameContainer_SetupGameInfo(__this, method);
    GameContainer_SetupGameInfo(__this, method);
    auto gameListing = __this->fields.gameListing;

    uint32_t ip = gameListing.IP;
    std::string ipAddress = std::format("{}.{}.{}.{}",
        (ip & 0xFF),
        (ip >> 8) & 0xFF,
        (ip >> 16) & 0xFF,
        (ip >> 24) & 0xFF
    );
    uint16_t port = gameListing.Port;
    std::string ipPortDisplay = State.UseCustomServer ? std::format("\n<#0fb>IP: {}:{}</color>", ipAddress, port) : "";

    auto platform = gameListing.Platform;
    std::string platformId = "Unknown";
    switch (platform) {
    case Platforms__Enum::StandaloneEpicPC:
        platformId = "Epic Games";
        break;
    case Platforms__Enum::StandaloneSteamPC:
        platformId = "Steam";
        break;
    case Platforms__Enum::StandaloneMac:
        platformId = "Mac";
        break;
    case Platforms__Enum::StandaloneWin10:
        platformId = "Microsoft Store";
        break;
    case Platforms__Enum::StandaloneItch:
        platformId = "itch.io";
        break;
    case Platforms__Enum::IPhone:
        platformId = "iOS/iPadOS";
        break;
    case Platforms__Enum::Android:
        platformId = "Android";
        break;
    case Platforms__Enum::Switch:
        platformId = "Nintendo Switch";
        break;
    case Platforms__Enum::Xbox:
        platformId = "Xbox";
        break;
    case Platforms__Enum::Playstation:
        platformId = "Playstation";
        break;
    default:
        platformId = "Unknown";
        break;
    }
    std::string lobbyCode = IsStreamerMode() ? "******" : convert_from_string(InnerNet_GameCode_IntToGameName(gameListing.GameId, NULL));
    int LobbyTime = (std::max)(0, int(gameListing.Age));

    std::string ageCol = getHexCodeFromImVec4(State.AgeColor);
    std::string daterCol = getHexCodeFromImVec4(State.DaterNamesColor);
    std::string nameCheckerCol = getHexCodeFromImVec4(State.NameCheckerColor);
    std::string lobbyCodeCol = getHexCodeFromImVec4(State.LobbyCodeColor);
    std::string platformCol = getHexCodeFromImVec4(State.PlatformColor);
    std::string hostCol = getHexCodeFromImVec4(State.HostColor);

    std::string lobbyTimeDisplay = std::format("\n{}Age: {}:{}{}</color>", ageCol, int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
    std::string playerCountCol = "<#0f0>";
    if (gameListing.PlayerCount == 4) playerCountCol = "<#ff0>";
    if (gameListing.PlayerCount < 4) playerCountCol = "<#f00>";
    std::string playerCount = playerCountCol + convert_from_string(TMP_Text_get_text((TMP_Text*)__this->fields.capacity, NULL)) + "</color>";
    std::string trueHostName = convert_from_string(gameListing.TrueHostName);
    if (IsDater(trueHostName, gameListing.MaxPlayers)) trueHostName = daterCol + trueHostName + "</color>"; // Color for daters
    const std::unordered_set<std::string> BannedNamesSet(State.LockedNames.begin(), State.LockedNames.end());
    if (BannedNamesSet.find(trueHostName) != BannedNamesSet.end()) trueHostName = nameCheckerCol + trueHostName + "</color>"; // Yellow color for banned names
    std::string separator = "<#0000>000000000000000</color>"; // The crewmate icon gets aligned properly with this

    std::string size = State.UseCustomServer ? "<size=35%>" : "<size=40%>";

    std::string playerCountDisplay = std::format("{}{}\n{}{}</color>\n{}\n{}{}</color>\n{}{}</color>{}{}\n{}</size>",
        size, separator, hostCol, trueHostName, playerCount, lobbyCodeCol, lobbyCode, platformCol, platformId, ipPortDisplay, lobbyTimeDisplay, separator);
    TMP_Text_set_text((TMP_Text*)__this->fields.capacity, convert_to_string(playerCountDisplay), NULL);
}

void dGameStartManager_Update(GameStartManager* __this, MethodInfo* method) {
    if (State.ShowHookLogs) Log.HookDebug("Hook dGameStartManager_Update executed", false);
    try {
        State.LobbyTimer -= Time_get_deltaTime(NULL);
        std::string LobbyCode = convert_from_string(InnerNet_GameCode_IntToGameName((*Game::pAmongUsClient)->fields._.GameId, NULL));
        
        std::string lobbyTimeDisplay = "";
        int LobbyTime = (int)State.LobbyTimer;
        if (LobbyTime >= 0 && LobbyCode != "LWQQQQ" && LobbyCode != "QQQQQQ") {
            if (!State.PanicMode && State.ShowLobbyTimer && IsHost()) {
                std::string formattedLobbyTime = std::format("{}{}:{}{}", State.JoinedAsHost ? "" : "~", int(LobbyTime / 60), LobbyTime % 60 < 10 ? "0" : "", LobbyTime % 60);
                std::string lobbyTimeColor = "<#0f0>";
                if (LobbyTime <= 60)
                    lobbyTimeColor = "<#f00>";
                else if (LobbyTime <= 180)
                    lobbyTimeColor = "<#ff0>";

                lobbyTimeDisplay = State.LobbyTimer < 0 ? "" : std::format("\n<size=50%>{}Lobby Closes in{} {}</color></size>",
                    lobbyTimeColor, State.JoinedAsHost ? ":" : " (approx.):", formattedLobbyTime);
            }
            /*std::string glitchDisplay = "";
            if (!State.PanicMode && State.ShowLobbyInfo) {
                std::string codeEnding = LobbyCode.substr(LobbyCode.length() - 4);
                if (glitchEndings.find(codeEnding) != glitchEndings.end()) glitchDisplay = " * ";
            }*/

            TMP_Text_set_fontStyle((TMP_Text*)__this->fields.GameRoomNameCode, FontStyles__Enum::Normal, NULL);

            if (State.HideCode && IsStreamerMode() && !State.PanicMode && LobbyCode != "") {
                std::string customCode = State.HideCode && IsStreamerMode() ? State.customCode : "******";
                if (State.RgbLobbyCode)
                    TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(State.rgbCode + /*glitchDisplay +*/ customCode + lobbyTimeDisplay), NULL);
                else
                    TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(/*glitchDisplay +*/ customCode + lobbyTimeDisplay), NULL);
            }
            else {
                if (State.RgbLobbyCode && !State.PanicMode)
                    TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(State.rgbCode + /*glitchDisplay +*/ LobbyCode + lobbyTimeDisplay), NULL);
                else
                    TMP_Text_set_text((TMP_Text*)__this->fields.GameRoomNameCode, convert_to_string(LobbyCode + /*glitchDisplay +*/ lobbyTimeDisplay), NULL);
            }
        }

        auto transform = Component_get_transform((Component_1*)__this, NULL);
        if (transform != NULL) {
            float camHeight = State.FollowerCam != NULL ? Camera_get_orthographicSize(State.FollowerCam, NULL) : 3.f;
            auto sgaTransform = Transform_FindChild(transform, convert_to_string("StartGameArea"), NULL);

            if (sgaTransform != NULL) {
                auto hostInfoPanel = Transform_FindChild(sgaTransform, convert_to_string("Host Info"), NULL);
                auto hostButton = Transform_FindChild(sgaTransform, convert_to_string("StartButton (Host)"), NULL);
                auto clientButton = Transform_FindChild(sgaTransform, convert_to_string("StartButton (Client)"), NULL);
                auto startingButton = Transform_FindChild(sgaTransform, convert_to_string("GameStarting"), NULL);

                auto hostButtonPos = Transform_get_localPosition(hostButton, NULL);
                auto clientButtonPos = Transform_get_localPosition(clientButton, NULL);
                auto startingButtonPos = Transform_get_localPosition(startingButton, NULL);

                // center the buttons and move them with the camera height
                Transform_set_localPosition(hostButton, { 0.3006f, 3.2736f - camHeight, hostButtonPos.z }, NULL);
                Transform_set_localPosition(clientButton, { -0.131f, 3.309f - camHeight, clientButtonPos.z }, NULL);
                Transform_set_localPosition(startingButton, { -0.131f, 3.309f - camHeight, startingButtonPos.z }, NULL);

                static std::string aspectPositionTypeName = translate_type_name("AspectPosition, Assembly-CSharp");
                Type* aspectPositionType = app::Type_GetType(convert_to_string(aspectPositionTypeName), NULL);

                auto hostInfoAspectPosition = (AspectPosition*)Component_GetComponent((Component_1*)hostInfoPanel, aspectPositionType, NULL);
                if (hostInfoAspectPosition != NULL) {
                    hostInfoAspectPosition->fields.DistanceFromEdge.x = 0.095f;
                }
            }
        }
    }
    catch (...) {
        LOG_ERROR("Exception occurred in GameStartManager_Update (LobbyBehaviour)");
    }
    State.IsStartCountdownActive = __this->fields.startState == GameStartManager_StartingStates__Enum::Countdown;
    if (IsHost() && State.IsStartCountdownActive && State.CancelingStartGame) {
        GameStartManager_ResetStartState(__this, NULL);
    }
    State.CancelingStartGame = false;
    bool buttonCheck = __this->fields.LastPlayerCount >= __this->fields.MinPlayers;
    __this->fields.MinPlayers = !State.PanicMode && State.AlwaysAllowStart ? (std::min)(__this->fields.LastPlayerCount, 4) : 4;
    if (buttonCheck != (__this->fields.LastPlayerCount >= __this->fields.MinPlayers)) {
        PassiveButton_SetButtonEnableState((PassiveButton*)__this->fields.StartButton, __this->fields.LastPlayerCount >= __this->fields.MinPlayers, NULL);
        __this->fields.LastPlayerCount++;
        GameStartManager_Update(__this, method);
        __this->fields.LastPlayerCount--;
    }
    GameStartManager_Update(__this, method);
    if (__this->fields.LastPlayerCount < 1)
        TMP_Text_set_text((TMP_Text*)__this->fields.PlayerCounter, convert_to_string("<#0f0>4/15</color>"), NULL); // this is the fallback used by the game
    else
        TMP_Text_set_text((TMP_Text*)__this->fields.PlayerCounter, convert_to_string(std::format("{}/{}", __this->fields.LastPlayerCount, GameOptions().GetMaxPlayers())), NULL);
    TMP_Text_set_color((TMP_Text*)__this->fields.PlayerCounter, Color(
        (float)(__this->fields.LastPlayerCount <= __this->fields.MinPlayers),
        (float)(__this->fields.LastPlayerCount >= __this->fields.MinPlayers), 0.f, 1.f), NULL);
}

static int findGameOffset = 0;

void dFindAGameManager_Update(FindAGameManager* __this, MethodInfo* method) {
    FindAGameManager_Update(__this, method);
    // Useful for later
}

void dGameStartManager_ReallyBegin(GameStartManager* __this, bool neverShow, MethodInfo* method) {
    GameStartManager_ReallyBegin(__this, neverShow, method);
    if (IsHost() && State.ModifyStartCountdown) {
        State.StartCountdown = std::clamp(State.StartCountdown, 1, !State.SafeMode ? 127 : 5);
        __this->fields.countDownTimer = State.StartCountdown + 0.0001f;
        // The game adds 0.0001f to the countdown timer, so we add it here too to keep it consistent
    }
}

void ApplyCosmeticPreset(const Settings::CosmeticPreset& p) {
    std::queue<RPCInterface*>* queue = IsInGame() ? &State.rpcQueue : (IsInLobby() ? &State.lobbyRpcQueue : nullptr);
    if (queue == nullptr) return;
    queue->push(new RpcSetColor((uint8_t)p.ColorId));
    if (!p.HatId.empty()) queue->push(new RpcSetHat(convert_to_string(p.HatId)));
    if (!p.SkinId.empty()) queue->push(new RpcSetSkin(convert_to_string(p.SkinId)));
    if (!p.VisorId.empty()) queue->push(new RpcSetVisor(convert_to_string(p.VisorId)));
    if (!p.PetId.empty()) queue->push(new RpcSetPet(convert_to_string(p.PetId)));
    if (!p.NamePlateId.empty()) queue->push(new RpcSetNamePlate(convert_to_string(p.NamePlateId)));
}

void ApplyHostPreset(const Settings::HostPreset& p) {
    if (!IsHost() || !GameOptions().HasOptions()) return;
    GameOptions()
        .SetFloat(app::FloatOptionNames__Enum::PlayerSpeedMod, p.PlayerSpeed)
        .SetFloat(app::FloatOptionNames__Enum::CrewLightMod, p.CrewmateVision)
        .SetFloat(app::FloatOptionNames__Enum::ImpostorLightMod, p.ImpostorVision)
        .SetFloat(app::FloatOptionNames__Enum::KillCooldown, p.KillCooldown)
        .SetInt(app::Int32OptionNames__Enum::KillDistance, p.KillDistance)
        .SetInt(app::Int32OptionNames__Enum::NumImpostors, p.NumImpostors)
        .SetInt(app::Int32OptionNames__Enum::MaxPlayers, p.MaxPlayers)
        .SetByte(app::ByteOptionNames__Enum::MapId, p.MapId)
        .SetBool(app::BoolOptionNames__Enum::VisualTasks, p.VisualTasks)
        .SetBool(app::BoolOptionNames__Enum::AnonymousVotes, p.AnonymousVotes)
        .SetInt(app::Int32OptionNames__Enum::NumEmergencyMeetings, p.NumEmergencyMeetings)
        .SetInt(app::Int32OptionNames__Enum::EmergencyCooldown, p.EmergencyCooldown)
        .SetInt(app::Int32OptionNames__Enum::DiscussionTime, p.DiscussionTime)
        .SetInt(app::Int32OptionNames__Enum::VotingTime, p.VotingTime)
        .SetInt(app::Int32OptionNames__Enum::TaskBarMode, p.TaskBarMode)
        .SetInt(app::Int32OptionNames__Enum::NumCommonTasks, p.NumCommonTasks)
        .SetInt(app::Int32OptionNames__Enum::NumLongTasks, p.NumLongTasks)
        .SetInt(app::Int32OptionNames__Enum::NumShortTasks, p.NumShortTasks)
        .SetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, p.ShapeshifterCooldown)
        .SetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, p.ShapeshifterCooldown)
        .SetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration, p.ShapeshifterDuration)
        .SetBool(app::BoolOptionNames__Enum::ShapeshifterLeaveSkin, p.ShapeshifterLeaveSkin)
        .SetFloat(app::FloatOptionNames__Enum::GuardianAngelCooldown, p.GuardianAngelCooldown)
        .SetBool(app::BoolOptionNames__Enum::ImpostorsCanSeeProtect, p.GuardianAngelProtectVisible)
        .SetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, p.GuardianAngelProtectDuration)
        .SetFloat(app::FloatOptionNames__Enum::ScientistCooldown, p.ScientistCooldown)
        .SetFloat(app::FloatOptionNames__Enum::ScientistBatteryCharge, p.ScientistBatteryCharge)
        .SetFloat(app::FloatOptionNames__Enum::EngineerCooldown, p.EngineerCooldown)
        .SetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime, p.EngineerInVentMaxTime)
        .SetFloat(app::FloatOptionNames__Enum::PhantomCooldown, p.PhantomCooldown)
        .SetFloat(app::FloatOptionNames__Enum::PhantomDuration, p.PhantomDuration)
        .SetFloat(app::FloatOptionNames__Enum::TrackerCooldown, p.TrackerCooldown)
        .SetFloat(app::FloatOptionNames__Enum::TrackerDuration, p.TrackerDuration)
        .SetFloat(app::FloatOptionNames__Enum::TrackerDelay, p.TrackerDelay)
        .SetFloat(app::FloatOptionNames__Enum::NoisemakerAlertDuration, p.NoisemakerAlertDuration)
        .SetBool(app::BoolOptionNames__Enum::NoisemakerImpostorAlert, p.NoisemakerImpostorAlert)
        .SetFloat(app::FloatOptionNames__Enum::ViperDissolveTime, p.ViperDissolveTime)
        .SetFloat(app::FloatOptionNames__Enum::DetectiveSuspectLimit, p.DetectiveSuspectLimit);
    auto roleOpts = GameOptions().GetRoleOptions();
    for (auto& [role, rp] : p.RoleRates)
        roleOpts.SetRoleRate((app::RoleTypes__Enum)role, rp.Count, rp.Chance);
    if (GameOptionsManager_get_Instance && GameManager_get_Instance && GameManager_get_LogicOptions && LogicOptions_SyncOptions) {
        auto* gm = GameManager_get_Instance(NULL);
        if (gm) { auto* lo = GameManager_get_LogicOptions(gm, NULL); if (lo) LogicOptions_SyncOptions(lo, NULL); }
    }
}

void RequestApplyHostPreset(int idx) {
    s_pendingApplyHostPresetIndex = idx;
}
