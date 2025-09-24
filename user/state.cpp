#include "pch-il2cpp.h"
#include "state.hpp"
#include <iostream>
#include <fstream>
#include "main.h"
#include "utility.h"
#include "logger.h"

Settings State;

void Settings::Load() {
    this->SickoVersion = "v4.4";

    auto path = getModulePath(hModule);
    auto configPath = path.parent_path() / "sicko-selected-config.json";

    if (!std::filesystem::exists(configPath))
        return;

    try {
        std::ifstream inConfig(configPath);
        nlohmann::ordered_json j = nlohmann::ordered_json::parse(inConfig, NULL, false);

#define JSON_TRYGET(key, value) \
        try { \
            j.at(key).get_to(value); \
        } catch (nlohmann::detail::out_of_range& e) { \
            Log.Info(e.what()); \
        }

        if (State.selectedConfig != "") JSON_TRYGET("SelectedConfig", this->selectedConfig);
    }
    catch (...) {
        //Log.Info("Unable to load sicko-selected-config.json");
    }

    auto settingsPath = path.parent_path() / std::format("sicko-config/{}.json", this->selectedConfig);

    if (!std::filesystem::exists(settingsPath))
        return;

    try {
        std::ifstream inSettings(settingsPath);
        nlohmann::ordered_json j = nlohmann::ordered_json::parse(inSettings, NULL, false);

#define JSON_TRYGET(key, value) \
        try { \
            j.at(key).get_to(value); \
        } catch (nlohmann::detail::out_of_range& e) { \
            Log.Info(e.what()); \
        }

        JSON_TRYGET("HasOpenedMenuBefore", this->HasOpenedMenuBefore);
        JSON_TRYGET("ShowMenuOnStartup", this->ShowMenuOnStartup);
        if (this->ShowMenuOnStartup) JSON_TRYGET("ShowMenu", this->ShowMenu);
        JSON_TRYGET("KeyBinds", this->KeyBinds);
#ifdef _DEBUG
        JSON_TRYGET("ShowDebug", this->showDebugTab);
#endif
        JSON_TRYGET("dpiScale", this->dpiScale);
        this->dpiChanged = true;
        JSON_TRYGET("RgbTheme", this->RgbMenuTheme);
        JSON_TRYGET("GradientTheme", this->GradientMenuTheme);
        JSON_TRYGET("MatchBackgroundWithTheme", this->MatchBackgroundWithTheme);
        JSON_TRYGET("SetName", this->SetName);
        JSON_TRYGET("LightMode", this->LightMode);
        JSON_TRYGET("MenuThemeColor_R", this->MenuThemeColor.x);
        JSON_TRYGET("MenuThemeColor_G", this->MenuThemeColor.y);
        JSON_TRYGET("MenuThemeColor_B", this->MenuThemeColor.z);
        JSON_TRYGET("MenuThemeColor_A", this->MenuThemeColor.w);
        JSON_TRYGET("MenuGradientColor1_R", this->MenuGradientColor1.x);
        JSON_TRYGET("MenuGradientColor1_G", this->MenuGradientColor1.y);
        JSON_TRYGET("MenuGradientColor1_B", this->MenuGradientColor1.z);
        JSON_TRYGET("MenuGradientColor1_A", this->MenuGradientColor1.w);
        JSON_TRYGET("MenuGradientColor1_A", this->MenuGradientColor1.w);
        JSON_TRYGET("MenuGradientColor2_R", this->MenuGradientColor2.x);
        JSON_TRYGET("MenuGradientColor2_G", this->MenuGradientColor2.y);
        JSON_TRYGET("MenuGradientColor2_B", this->MenuGradientColor2.z);
        JSON_TRYGET("MenuGradientColor2_A", this->MenuGradientColor2.w);
        JSON_TRYGET("UnlockCosmetics", this->UnlockCosmetics);
        JSON_TRYGET("GameFPS", this->GameFPS);
        JSON_TRYGET("ShowKeybinds", this->ShowKeybinds);
        JSON_TRYGET("KeybindsWhileChatting", this->KeybindsWhileChatting);
        JSON_TRYGET("SpoofLevel", this->SpoofLevel);
        JSON_TRYGET("FakeLevel", this->FakeLevel);
        JSON_TRYGET("SpoofFriendCode", this->SpoofFriendCode);
        JSON_TRYGET("UseNewFriendCode", this->UseNewFriendCode);
        //JSON_TRYGET("GuestPuid", this->GuestPuid);
        //JSON_TRYGET("UseGuestPuid", this->UseGuestPuid);
        JSON_TRYGET("NewFriendCode", this->NewFriendCode);
        JSON_TRYGET("FakeFriendCode", this->FakeFriendCode);
        JSON_TRYGET("SpoofPlatform", this->SpoofPlatform);
        JSON_TRYGET("FakePlatform", this->FakePlatform);
        JSON_TRYGET("SpoofGuestAccount", this->SpoofGuestAccount);
        JSON_TRYGET("SpoofAUVersion", this->SpoofAUVersion);
        JSON_TRYGET("FakeAUVersion", this->FakeAUVersion);
        JSON_TRYGET("PanicWarning", this->PanicWarning);
        JSON_TRYGET("DisableAnimations", this->DisableAnimations);
        JSON_TRYGET("AnimationSpeed", this->AnimationSpeed);
        JSON_TRYGET("RoundingRadiusMultiplier", this->RoundingRadiusMultiplier);
		this->RoundingRadiusMultiplier = std::clamp(this->RoundingRadiusMultiplier, 0.f, 2.f);
        JSON_TRYGET("ExtraCommands", this->ExtraCommands);

        JSON_TRYGET("NoAbilityCD", this->NoAbilityCD);
        JSON_TRYGET("DarkMode", this->DarkMode);
        JSON_TRYGET("CustomGameTheme", this->CustomGameTheme);
        JSON_TRYGET("GameTextColor_R", this->GameTextColor.x);
        JSON_TRYGET("GameTextColor_G", this->GameTextColor.y);
        JSON_TRYGET("GameTextColor_B", this->GameTextColor.z);
        JSON_TRYGET("GameBgColor_R", this->GameBgColor.x);
        JSON_TRYGET("GameBgColor_G", this->GameBgColor.y);
        JSON_TRYGET("GameBgColor_B", this->GameBgColor.z);
        JSON_TRYGET("SeeVanishedPlayers", this->SeeVanishedPlayers);
        JSON_TRYGET("SelectedColorId", this->SelectedColorId);
        JSON_TRYGET("SnipeColor", this->SnipeColor);
        JSON_TRYGET("CycleBetweenPlayers", this->CycleBetweenPlayers);
        //JSON_TRYGET("CycleBetweenOutfits", this->CycleBetweenOutfits);
        //JSON_TRYGET("ChangeBodyType", this->ChangeBodyType);
        //JSON_TRYGET("BodyType", this->BodyType);
        JSON_TRYGET("CycleInMeeting", this->CycleInMeeting);
        JSON_TRYGET("CycleTimer", this->CycleTimer);
        JSON_TRYGET("CyclerUserNames", this->cyclerUserNames);
        //JSON_TRYGET("playerOutfits", this->playerOutfits);
        JSON_TRYGET("HostUsername", this->hostUserName);
        JSON_TRYGET("ChatMessage", this->chatMessage);
        JSON_TRYGET("CycleName", this->CycleName);
        JSON_TRYGET("CycleColor", this->RandomColor);
        JSON_TRYGET("CycleHat", this->RandomHat);
        JSON_TRYGET("CycleVisor", this->RandomVisor);
        JSON_TRYGET("CycleSkin", this->RandomSkin);
        JSON_TRYGET("CyclePet", this->RandomPet);
        JSON_TRYGET("CycleNamePlate", this->RandomNamePlate);
        JSON_TRYGET("PlayerSpeed", this->PlayerSpeed);
        JSON_TRYGET("MultiplySpeed", this->MultiplySpeed);
        JSON_TRYGET("KillDistance", this->KillDistance);
        JSON_TRYGET("ModifyKillDistance", this->ModifyKillDistance);
        JSON_TRYGET("ModifyTaskBarUpdates", this->ModifyTaskBarUpdates);
        JSON_TRYGET("UserName", this->userName);
        JSON_TRYGET("ShowGhosts", this->ShowGhosts);
        JSON_TRYGET("ShowPhantoms", this->ShowPhantoms);
        JSON_TRYGET("ShowPlayersInVents", this->ShowPlayersInVents);
        JSON_TRYGET("FakeRole", this->FakeRole);
        JSON_TRYGET("AutoFakeRole", this->AutoFakeRole);

        JSON_TRYGET("NoGameEnd", this->NoGameEnd);
        JSON_TRYGET("DisableMeetings", this->DisableMeetings);
        JSON_TRYGET("DisableSabotages", this->DisableSabotages);
        JSON_TRYGET("DisableAllVotekicks", this->DisableAllVotekicks);
        JSON_TRYGET("DisableRoleManager", this->DisableRoleManager);

        JSON_TRYGET("ShowRadar", this->ShowRadar);
        JSON_TRYGET("ShowRadar_DeadBodies", this->ShowRadar_DeadBodies);
        JSON_TRYGET("ShowRadar_Ghosts", this->ShowRadar_Ghosts);
        JSON_TRYGET("HideRadar_During_Meetings", this->HideRadar_During_Meetings);
        JSON_TRYGET("ShowRadar_RightClickTP", this->ShowRadar_RightClickTP);
        JSON_TRYGET("LockRadar", this->LockRadar);
        JSON_TRYGET("RadarColor_R", this->SelectedColor.x);
        JSON_TRYGET("RadarColor_G", this->SelectedColor.y);
        JSON_TRYGET("RadarColor_B", this->SelectedColor.z);
        JSON_TRYGET("RadarColor_A", this->SelectedColor.w);
        JSON_TRYGET("RadarDrawIcons", this->RadarDrawIcons);
        JSON_TRYGET("RadarVisorRoleColor", this->RadarVisorRoleColor);
        JSON_TRYGET("RadarBorder", this->RadarBorder);
        JSON_TRYGET("RadarExtraWidth", this->RadarExtraWidth);
        JSON_TRYGET("RadarExtraHeight", this->RadarExtraHeight);

        if (this->ShowMenuOnStartup) JSON_TRYGET("ShowReplay", this->ShowReplay);
        JSON_TRYGET("ReplayColor_R", this->SelectedReplayMapColor.x);
        JSON_TRYGET("ReplayColor_G", this->SelectedReplayMapColor.y);
        JSON_TRYGET("ReplayColor_B", this->SelectedReplayMapColor.z);
        JSON_TRYGET("ReplayColor_A", this->SelectedReplayMapColor.w);
        JSON_TRYGET("ReplayShowOnlyLastSeconds", this->Replay_ShowOnlyLastSeconds);
        JSON_TRYGET("ReplayLastSecondsValue", this->Replay_LastSecondsValue);
        JSON_TRYGET("ReplayClearAfterMeeting", this->Replay_ClearAfterMeeting);

        JSON_TRYGET("ShowEsp", this->ShowEsp);
        JSON_TRYGET("ShowEsp_Ghosts", this->ShowEsp_Ghosts);
        JSON_TRYGET("ShowEsp_Box", this->ShowEsp_Box);
        JSON_TRYGET("ShowEsp_Tracers", this->ShowEsp_Tracers);
        JSON_TRYGET("ShowEsp_Distance", this->ShowEsp_Distance);
        JSON_TRYGET("HideEsp_During_Meetings", this->HideEsp_During_Meetings);
        JSON_TRYGET("ShowEsp_RoleBased", this->ShowEsp_RoleBased);
        JSON_TRYGET("ShowEsp_Crew", this->ShowEsp_Crew);
        JSON_TRYGET("ShowEsp_Imp", this->ShowEsp_Imp);

        JSON_TRYGET("MaxVision", this->MaxVision);
        JSON_TRYGET("Wallhack", this->Wallhack);
        JSON_TRYGET("FreeCamSpeed", this->FreeCamSpeed);
        JSON_TRYGET("ZoomLevel", this->CameraHeight);
        JSON_TRYGET("UnlockVents", this->UnlockVents);
        JSON_TRYGET("UnlockKillButton", this->UnlockKillButton);
        JSON_TRYGET("ChatPaste", this->ChatPaste);
        JSON_TRYGET("RevealRoles", this->RevealRoles);
        JSON_TRYGET("AbbreviatedRoleNames", this->AbbreviatedRoleNames);
        JSON_TRYGET("PlayerColoredDots", this->PlayerColoredDots);
        JSON_TRYGET("ShowPlayerInfo", this->ShowPlayerInfo);
        JSON_TRYGET("ShowLobbyInfo", this->ShowLobbyInfo);
        JSON_TRYGET("ChatAlwaysActive", this->ChatAlwaysActive);
        JSON_TRYGET("ReadGhostMessages", this->ReadGhostMessages);
        JSON_TRYGET("ReadAndSendSickoChat", this->ReadAndSendSickoChat);
        JSON_TRYGET("CustomName", this->CustomName);
        JSON_TRYGET("RgbName", this->RgbName);
        JSON_TRYGET("UsePrefixAndSuffix", this->UsePrefixAndSuffix);
        JSON_TRYGET("PrefixAndSuffixNewLines", this->PrefixAndSuffixNewLines);
        JSON_TRYGET("NamePrefix", this->NamePrefix);
        JSON_TRYGET("NameSuffix", this->NameSuffix);
        JSON_TRYGET("Font", this->Font);
        JSON_TRYGET("FontType", this->FontType);
        JSON_TRYGET("ChatFont", this->ChatFont);
        JSON_TRYGET("ChatFontType", this->ChatFontType);
        //JSON_TRYGET("Material", this->Material);
        //JSON_TRYGET("MaterialType", this->MaterialType);
        JSON_TRYGET("ResizeName", this->ResizeName);
        JSON_TRYGET("IndentName", this->IndentName);
        JSON_TRYGET("CspaceName", this->CspaceName);
        JSON_TRYGET("MspaceName", this->MspaceName);
        JSON_TRYGET("VoffsetName", this->VoffsetName);
        JSON_TRYGET("RotateName", this->RotateName);
        JSON_TRYGET("NameSize", this->NameSize);
        JSON_TRYGET("NameIndent", this->NameIndent);
        JSON_TRYGET("NameCspace", this->NameIndent);
        JSON_TRYGET("ItalicName", this->ItalicName);
        JSON_TRYGET("UnderlineName", this->UnderlineName);
        JSON_TRYGET("StrikethroughName", this->StrikethroughName);
        JSON_TRYGET("BoldName", this->BoldName);
        JSON_TRYGET("NobrName", this->BoldName);
        JSON_TRYGET("ColoredName", this->ColoredName);
        JSON_TRYGET("NameColor1_R", this->NameColor1.x);
        JSON_TRYGET("NameColor1_G", this->NameColor1.y);
        JSON_TRYGET("NameColor1_B", this->NameColor1.z);
        JSON_TRYGET("NameColor1_A", this->NameColor1.w);
        JSON_TRYGET("NameColor2_R", this->NameColor2.x);
        JSON_TRYGET("NameColor2_G", this->NameColor2.y);
        JSON_TRYGET("NameColor2_B", this->NameColor2.z);
        JSON_TRYGET("NameColor2_A", this->NameColor2.w);
        JSON_TRYGET("AutoOpenDoors", this->AutoOpenDoors);
        JSON_TRYGET("MoveInVentAndShapeshift", this->MoveInVentAndShapeshift);
        JSON_TRYGET("AlwaysMove", this->AlwaysMove);
        JSON_TRYGET("AnimationlessShapeshift", this->AnimationlessShapeshift);
        JSON_TRYGET("DisableKillAnimation", this->DisableKillAnimation);
        JSON_TRYGET("KillImpostors", this->KillImpostors);
        JSON_TRYGET("KillInVanish", this->KillInVanish);
        JSON_TRYGET("BypassAngelProt", this->BypassAngelProt);
        JSON_TRYGET("InfiniteKillRange", this->InfiniteKillRange);
        JSON_TRYGET("AutoKill", this->AutoKill);
        JSON_TRYGET("FakeAlive", this->FakeAlive);
        JSON_TRYGET("ShowHost", this->ShowHost);
        JSON_TRYGET("HideWatermark", this->HideWatermark);
        JSON_TRYGET("ShowVoteKicks", this->ShowVoteKicks);
        JSON_TRYGET("ShowFps", this->ShowFps);
        JSON_TRYGET("DoTasksAsImpostor", this->DoTasksAsImpostor);
        JSON_TRYGET("AutoCopyLobbyCode", this->AutoCopyLobbyCode);
        JSON_TRYGET("DisableLobbyMusic", this->DisableLobbyMusic);
        JSON_TRYGET("ReportOnMurder", this->ReportOnMurder);
        JSON_TRYGET("PreventSelfReport", this->PreventSelfReport);
        //JSON_TRYGET("AutoRejoin", this->AutoRejoin);
        JSON_TRYGET("OldStylePingText", this->OldStylePingText);
        JSON_TRYGET("NoSeekerAnim", this->NoSeekerAnim);
        JSON_TRYGET("BetterChatNotifications", this->BetterChatNotifications);
        JSON_TRYGET("BetterLobbyCodeInput", this->BetterLobbyCodeInput);
        JSON_TRYGET("BetterMessageSounds", this->BetterMessageSounds);
        JSON_TRYGET("NoClip", this->NoClip);
        JSON_TRYGET("KillInLobbies", this->KillInLobbies);
        JSON_TRYGET("KillInVanish", this->KillInVanish);
        JSON_TRYGET("GodMode", this->GodMode);

        JSON_TRYGET("AdjustByDPI", this->AdjustByDPI);

        JSON_TRYGET("RevealVotes", this->RevealVotes);
        JSON_TRYGET("ShowProtections", this->ShowProtections);

        JSON_TRYGET("CustomImpostorAmount", this->CustomImpostorAmount);
        JSON_TRYGET("ImpostorCount", this->ImpostorCount);

        if (this->ShowMenuOnStartup) JSON_TRYGET("ShowConsole", this->ShowConsole);
        JSON_TRYGET("ShowUnityLogs", this->ShowUnityLogs);
        //JSON_TRYGET("ShowHookLogs", this->ShowHookLogs);

        JSON_TRYGET("RevealAnonymousVotes", this->RevealAnonymousVotes);
        JSON_TRYGET("ShowChatTimer", this->ShowChatTimer);
        JSON_TRYGET("ExtendChatLimit", this->ExtendChatLimit);
        JSON_TRYGET("ExtendChatHistory", this->ExtendChatHistory);

        JSON_TRYGET("ShiftRightClickTP", this->ShiftRightClickTP);
        JSON_TRYGET("RotateRadius", this->RotateRadius);
        JSON_TRYGET("RelativeTeleport", this->RelativeTeleport);
        JSON_TRYGET("ShowKillCD", this->ShowKillCD);

        JSON_TRYGET("Confuser", this->confuser);
        JSON_TRYGET("ConfuseOnJoin", this->confuseOnJoin);
        JSON_TRYGET("ConfuseOnStart", this->confuseOnStart);
        JSON_TRYGET("ConfuseOnKill", this->confuseOnKill);
        JSON_TRYGET("ConfuseOnVent", this->confuseOnVent);
        JSON_TRYGET("ConfuseOnMeeting", this->confuseOnMeeting);

        JSON_TRYGET("CyclerNameGeneration", this->cyclerNameGeneration);
        JSON_TRYGET("ConfuserNameGeneration", this->confuserNameGeneration);

        JSON_TRYGET("CustomCode", this->customCode);
        JSON_TRYGET("HideCode", this->HideCode);
        JSON_TRYGET("RgbLobbyCode", this->RgbLobbyCode);

        JSON_TRYGET("ShowLobbyTimer", this->ShowLobbyTimer);
        JSON_TRYGET("ModDetection", this->ModDetection);
        JSON_TRYGET("BroadcastedMod", this->BroadcastedMod);
        JSON_TRYGET("DisableHostAnticheat", this->DisableHostAnticheat);
        JSON_TRYGET("TournamentMode", this->TournamentMode);
        JSON_TRYGET("SpectatorMode", this->SpectatorMode);
        JSON_TRYGET("AlwaysAllowStart", this->AlwaysAllowStart);
        JSON_TRYGET("ModifyStartCountdown", this->ModifyStartCountdown);
        JSON_TRYGET("StartCountdown", this->StartCountdown);

        JSON_TRYGET("Enable_SMAC", this->Enable_SMAC);
        JSON_TRYGET("SMAC_Punishment", this->SMAC_Punishment);
        JSON_TRYGET("SMAC_HostPunishment", this->SMAC_HostPunishment);
        JSON_TRYGET("SMAC_AddToBlacklist", this->SMAC_AddToBlacklist);
        JSON_TRYGET("SMAC_PunishBlacklist", this->SMAC_PunishBlacklist);
        JSON_TRYGET("SMAC_IgnoreWhitelist", this->SMAC_IgnoreWhitelist);
        JSON_TRYGET("SMAC_CheckAUM", this->SMAC_CheckAUM);
        JSON_TRYGET("SMAC_CheckSicko", this->SMAC_CheckSicko);
        JSON_TRYGET("SMAC_CheckBadNames", this->SMAC_CheckBadNames);
        JSON_TRYGET("SMAC_CheckColor", this->SMAC_CheckColor);
        JSON_TRYGET("SMAC_CheckCosmetics", this->SMAC_CheckCosmetics);
        JSON_TRYGET("SMAC_CheckChatNote", this->SMAC_CheckChatNote);
        JSON_TRYGET("SMAC_CheckScanner", this->SMAC_CheckScanner);
        JSON_TRYGET("SMAC_CheckAnimation", this->SMAC_CheckAnimation);
        JSON_TRYGET("SMAC_CheckTasks", this->SMAC_CheckTasks);
        JSON_TRYGET("SMAC_CheckRole", this->SMAC_CheckRole);
        JSON_TRYGET("SMAC_CheckChat", this->SMAC_CheckChat);
        JSON_TRYGET("SMAC_CheckMeeting", this->SMAC_CheckMeeting);
        JSON_TRYGET("SMAC_CheckReport", this->SMAC_CheckReport);
        JSON_TRYGET("SMAC_CheckMurder", this->SMAC_CheckMurder);
        JSON_TRYGET("SMAC_CheckShapeshift", this->SMAC_CheckShapeshift);
        JSON_TRYGET("SMAC_CheckVanish", this->SMAC_CheckVanish);
        JSON_TRYGET("SMAC_CheckLevel", this->SMAC_CheckLevel);
        JSON_TRYGET("SMAC_CheckVent", this->SMAC_CheckVent);
        JSON_TRYGET("SMAC_CheckSabotage", this->SMAC_CheckSabotage);
        JSON_TRYGET("SMAC_HighLevel", this->SMAC_HighLevel);
        JSON_TRYGET("SMAC_LowLevel", this->SMAC_LowLevel);
        JSON_TRYGET("SMAC_CheckBadWords", this->SMAC_CheckBadWords);
        JSON_TRYGET("SMAC_BadWords", this->SMAC_BadWords);
        JSON_TRYGET("ChatPresets", this->ChatPresets);

        JSON_TRYGET("Destruct_IgnoreWhitelist", this->Destruct_IgnoreWhitelist);
        JSON_TRYGET("Ban_IgnoreWhitelist", this->Ban_IgnoreWhitelist);
        JSON_TRYGET("TimerAFK", this->TimerAFK);
        JSON_TRYGET("AddExtraTime", this->AddExtraTime);
        JSON_TRYGET("ExtraTimeThreshold", this->ExtraTimeThreshold);
        JSON_TRYGET("NotificationTimeWarn", this->NotificationTimeWarn);
        JSON_TRYGET("BypassVisualTasks", this->BypassVisualTasks);
        JSON_TRYGET("AutoHostRole", this->AutoHostRole);
        JSON_TRYGET("HostRoleToSet", this->HostRoleToSet);

        JSON_TRYGET("WhitelistFriendCodes", this->WhitelistFriendCodes);
        JSON_TRYGET("BlacklistFriendCodes", this->BlacklistFriendCodes);
        JSON_TRYGET("WarnedFriendCodes", this->WarnedFriendCodes);
        JSON_TRYGET("WarnReasons", this->WarnReasons);
        JSON_TRYGET("LockedNames", this->LockedNames);

        JSON_TRYGET("DisableMedbayScan", this->DisableMedbayScan);

        JSON_TRYGET("CrewmateGhostColor_R", this->CrewmateGhostColor.x);
        JSON_TRYGET("CrewmateGhostColor_G", this->CrewmateGhostColor.y);
        JSON_TRYGET("CrewmateGhostColor_B", this->CrewmateGhostColor.z);
        JSON_TRYGET("CrewmateGhostColor_A", this->CrewmateGhostColor.w);
        JSON_TRYGET("CrewmateColor_R", this->CrewmateColor.x);
        JSON_TRYGET("CrewmateColor_G", this->CrewmateColor.y);
        JSON_TRYGET("CrewmateColor_B", this->CrewmateColor.z);
        JSON_TRYGET("CrewmateColor_A", this->CrewmateColor.w);
        JSON_TRYGET("EngineerColor_R", this->EngineerColor.x);
        JSON_TRYGET("EngineerColor_G", this->EngineerColor.y);
        JSON_TRYGET("EngineerColor_B", this->EngineerColor.z);
        JSON_TRYGET("EngineerColor_A", this->EngineerColor.w);
        JSON_TRYGET("GuardianAngelColor_R", this->GuardianAngelColor.x);
        JSON_TRYGET("GuardianAngelColor_G", this->GuardianAngelColor.y);
        JSON_TRYGET("GuardianAngelColor_B", this->GuardianAngelColor.z);
        JSON_TRYGET("GuardianAngelColor_A", this->GuardianAngelColor.w);
        JSON_TRYGET("GuardianAngelColor_R", this->ScientistColor.x);
        JSON_TRYGET("GuardianAngelColor_G", this->ScientistColor.y);
        JSON_TRYGET("GuardianAngelColor_B", this->ScientistColor.z);
        JSON_TRYGET("GuardianAngelColor_A", this->ScientistColor.w);
        JSON_TRYGET("ScientistColor_R", this->ScientistColor.x);
        JSON_TRYGET("ScientistColor_G", this->ScientistColor.y);
        JSON_TRYGET("ScientistColor_B", this->ScientistColor.z);
        JSON_TRYGET("ScientistColor_A", this->ScientistColor.w);
        JSON_TRYGET("ImpostorColor_R", this->ImpostorColor.x);
        JSON_TRYGET("ImpostorColor_G", this->ImpostorColor.y);
        JSON_TRYGET("ImpostorColor_B", this->ImpostorColor.z);
        JSON_TRYGET("ImpostorColor_A", this->ImpostorColor.w);
        JSON_TRYGET("ShapeshifterColor_R", this->ShapeshifterColor.x);
        JSON_TRYGET("ShapeshifterColor_G", this->ShapeshifterColor.y);
        JSON_TRYGET("ShapeshifterColor_B", this->ShapeshifterColor.z);
        JSON_TRYGET("ShapeshifterColor_A", this->ShapeshifterColor.w);
        JSON_TRYGET("ImpostorGhostColor_R", this->ImpostorGhostColor.x);
        JSON_TRYGET("ImpostorGhostColor_G", this->ImpostorGhostColor.y);
        JSON_TRYGET("ImpostorGhostColor_B", this->ImpostorGhostColor.z);
        JSON_TRYGET("ImpostorGhostColor_A", this->ImpostorGhostColor.w);
        JSON_TRYGET("NoisemakerColor_R", this->NoisemakerColor.x);
        JSON_TRYGET("NoisemakerColor_G", this->NoisemakerColor.y);
        JSON_TRYGET("NoisemakerColor_B", this->NoisemakerColor.z);
        JSON_TRYGET("NoisemakerColor_A", this->NoisemakerColor.w);
        JSON_TRYGET("TrackerColor_R", this->TrackerColor.x);
        JSON_TRYGET("TrackerColor_G", this->TrackerColor.y);
        JSON_TRYGET("TrackerColor_B", this->TrackerColor.z);
        JSON_TRYGET("TrackerColor_A", this->TrackerColor.w);
        JSON_TRYGET("PhantomColor_R", this->PhantomColor.x);
        JSON_TRYGET("PhantomColor_G", this->PhantomColor.y);
        JSON_TRYGET("PhantomColor_B", this->PhantomColor.z);
        JSON_TRYGET("PhantomColor_A", this->PhantomColor.w);

        JSON_TRYGET("HostColor_R", this->HostColor.x);
        JSON_TRYGET("HostColor_G", this->HostColor.y);
        JSON_TRYGET("HostColor_B", this->HostColor.z);
        JSON_TRYGET("HostColor_A", this->HostColor.w);
        JSON_TRYGET("PlayerIdColor_R", this->PlayerIdColor.x);
        JSON_TRYGET("PlayerIdColor_G", this->PlayerIdColor.y);
        JSON_TRYGET("PlayerIdColor_B", this->PlayerIdColor.z);
        JSON_TRYGET("PlayerIdColor_A", this->PlayerIdColor.w);
        JSON_TRYGET("LevelColor_R", this->LevelColor.x);
        JSON_TRYGET("LevelColor_G", this->LevelColor.y);
        JSON_TRYGET("LevelColor_B", this->LevelColor.z);
        JSON_TRYGET("LevelColor_A", this->LevelColor.w);
        JSON_TRYGET("PlatformColor_R", this->PlatformColor.x);
        JSON_TRYGET("PlatformColor_G", this->PlatformColor.y);
        JSON_TRYGET("PlatformColor_B", this->PlatformColor.z);
        JSON_TRYGET("PlatformColor_A", this->PlatformColor.w);
        JSON_TRYGET("ModUsageColor_R", this->ModUsageColor.x);
        JSON_TRYGET("ModUsageColor_G", this->ModUsageColor.y);
        JSON_TRYGET("ModUsageColor_B", this->ModUsageColor.z);
        JSON_TRYGET("ModUsageColor_A", this->ModUsageColor.w);
        JSON_TRYGET("NameCheckerColor_R", this->NameCheckerColor.x);
        JSON_TRYGET("NameCheckerColor_G", this->NameCheckerColor.y);
        JSON_TRYGET("NameCheckerColor_B", this->NameCheckerColor.z);
        JSON_TRYGET("NameCheckerColor_A", this->NameCheckerColor.w);
        JSON_TRYGET("FriendCodeColor_R", this->FriendCodeColor.x);
        JSON_TRYGET("FriendCodeColor_G", this->FriendCodeColor.y);
        JSON_TRYGET("FriendCodeColor_B", this->FriendCodeColor.z);
        JSON_TRYGET("FriendCodeColor_A", this->FriendCodeColor.w);
        JSON_TRYGET("DaterNamesColor_R", this->DaterNamesColor.x);
        JSON_TRYGET("DaterNamesColor_G", this->DaterNamesColor.y);
        JSON_TRYGET("DaterNamesColor_B", this->DaterNamesColor.z);
        JSON_TRYGET("DaterNamesColor_A", this->DaterNamesColor.w);
        JSON_TRYGET("LobbyCodeColor_R", this->LobbyCodeColor.x);
        JSON_TRYGET("LobbyCodeColor_G", this->LobbyCodeColor.y);
        JSON_TRYGET("LobbyCodeColor_B", this->LobbyCodeColor.z);
        JSON_TRYGET("LobbyCodeColor_A", this->LobbyCodeColor.w);
        JSON_TRYGET("AgeColor_R", this->AgeColor.x);
        JSON_TRYGET("AgeColor_G", this->AgeColor.y);
        JSON_TRYGET("AgeColor_B", this->AgeColor.z);
        JSON_TRYGET("AgeColor_A", this->AgeColor.w);

        JSON_TRYGET("TempBanEnabled", this->TempBanEnabled);

        // Temp-Ban: Loading
        if (j.contains("TempBannedFriendCodes") && j["TempBannedFriendCodes"].is_object()) {
            for (auto& [fc, info] : j["TempBannedFriendCodes"].items()) {
                if (info.contains("until")) {
                    if (info["until"].is_string()) {
                        std::string ts = info["until"].get<std::string>();
                        std::tm tm{};
                        std::istringstream ss(ts);
                        ss >> std::get_time(&tm, "%y/%m/%d %H:%M:%S");
                        if (!ss.fail()) {
                            std::time_t t = std::mktime(&tm);
                            this->TempBannedFCs[fc] = std::chrono::system_clock::from_time_t(t);
                        }
                    }
                    else if (info["until"].is_number_integer()) {
                        int64_t until = info["until"].get<int64_t>();
                        this->TempBannedFCs[fc] = std::chrono::system_clock::time_point(std::chrono::seconds(until));
                    }
                }
            }
        }
    }
    catch (...) {
        Log.Info("Unable to load " + std::format("sicko-config/{}.json", this->selectedConfig));
    }

    //Do not do any IL2CPP stuff here!  The constructors of most classes have not run yet!
}

void Settings::SaveConfig() {
    auto path = getModulePath(hModule);
    std::filesystem::create_directory(path.parent_path() / "sicko-config");

    auto configPath = path.parent_path() / "sicko-selected-config.json";

    if (this->selectedConfig != "") {
        try {
            nlohmann::ordered_json j = nlohmann::ordered_json{
                { "SelectedConfig", this->selectedConfig },
            };

            std::ofstream outConfig(configPath);
            outConfig << std::setw(4) << j << std::endl;
        }
        catch (...) {
            //Log.Info("Unable to save sicko-selected-config.json");
        }
    }
}

void Settings::Save() {
    auto path = getModulePath(hModule);
    std::filesystem::create_directory(path.parent_path() / "sicko-config");

    auto configPath = path.parent_path() / "sicko-selected-config.json";

    if (this->selectedConfig != "") {
        try {
            nlohmann::ordered_json j = nlohmann::ordered_json{
                { "SelectedConfig", this->selectedConfig },
            };

            std::ofstream outConfig(configPath);
            outConfig << std::setw(4) << j << std::endl;
        }
        catch (...) {
            //Log.Info("Unable to save sicko-selected-config.json");
        }
        auto settingsPath = path.parent_path() /
            std::format("sicko-config/{}.json", GetAllConfigs().size() != 0 ? this->selectedConfig : "default");

        try {
            nlohmann::ordered_json j = nlohmann::ordered_json{
                { "HasOpenedMenuBefore", this->HasOpenedMenuBefore },
                { "ShowMenuOnStartup", this->ShowMenuOnStartup },
                { "ShowMenu", this->ShowMenu },
                { "KeyBinds", this->KeyBinds },
        #ifdef _DEBUG
                { "ShowDebug", this->showDebugTab },
        #endif
                { "dpiScale", this->dpiScale },
                { "RgbTheme", this->RgbMenuTheme },
                { "GradientTheme", this->GradientMenuTheme },
                { "MatchBackgroundWithTheme", this->MatchBackgroundWithTheme },
                { "SetName", this->SetName },
                { "LightMode", this->LightMode },
                { "MenuThemeColor_R", this->MenuThemeColor.x },
                { "MenuThemeColor_G", this->MenuThemeColor.y },
                { "MenuThemeColor_B", this->MenuThemeColor.z },
                { "MenuThemeColor_A", this->MenuThemeColor.w },
                { "MenuGradientColor1_R", this->MenuGradientColor1.x },
                { "MenuGradientColor1_G", this->MenuGradientColor1.y },
                { "MenuGradientColor1_B", this->MenuGradientColor1.z },
                { "MenuGradientColor1_A", this->MenuGradientColor1.w },
                { "MenuGradientColor2_R", this->MenuGradientColor2.x },
                { "MenuGradientColor2_G", this->MenuGradientColor2.y },
                { "MenuGradientColor2_B", this->MenuGradientColor2.z },
                { "MenuGradientColor2_A", this->MenuGradientColor2.w },
                { "UnlockCosmetics", this->UnlockCosmetics },
                { "GameFPS", this->GameFPS },
                { "ShowKeybinds", this->ShowKeybinds },
                { "KeybindsWhileChatting", this->KeybindsWhileChatting },
                { "SpoofLevel", this->SpoofLevel },
                { "FakeLevel", this->FakeLevel },
                { "SpoofFriendCode", this->SpoofFriendCode },
                { "UseNewFriendCode", this->UseNewFriendCode },
                { "NewFriendCode", this->NewFriendCode },
                //{ "UseGuestPuid", this->UseGuestPuid },
                //{ "GuestPuid", this->GuestPuid },
                { "FakeFriendCode", this->FakeFriendCode },
                { "FakePuid", this->FakePuid },
                { "StealedFC", this->StealedFC },
                { "StealedPUID", this->StealedPUID },
                { "SpoofPlatform", this->SpoofPlatform },
                { "FakePlatform", this->FakePlatform },
                { "SpoofGuestAccount", this->SpoofGuestAccount },
                { "SpoofAUVersion", this->SpoofAUVersion },
                { "FakeAUVersion", this->FakeAUVersion },
                { "PanicWarning", this->PanicWarning },
                { "DisableAnimations", this->DisableAnimations },
                { "AnimationSpeed", this->AnimationSpeed },
                { "RoundingRadiusMultiplier", this->RoundingRadiusMultiplier },
                { "ExtraCommands", this->ExtraCommands },

                { "NoAbilityCD", this->NoAbilityCD },
                { "DarkMode", this->DarkMode },
                { "CustomGameTheme", this->CustomGameTheme },
                { "GameTextColor_R", this->GameTextColor.x },
                { "GameTextColor_G", this->GameTextColor.y },
                { "GameTextColor_B", this->GameTextColor.z },
                { "GameBgColor_R", this->GameBgColor.x },
                { "GameBgColor_G", this->GameBgColor.y },
                { "GameBgColor_B", this->GameBgColor.z },
                { "SeeVanishedPlayers", this->SeeVanishedPlayers },
                { "SelectedColorId", this->SelectedColorId },
                { "SnipeColor", this->SnipeColor },
                { "CycleBetweenPlayers", this->CycleBetweenPlayers },
                //{ "CycleBetweenOutfits", this->CycleBetweenOutfits },
                //{ "ChangeBodyType", this->ChangeBodyType },
                //{ "BodyType", this->BodyType },
                { "CycleInMeeting", this->CycleInMeeting },
                { "CycleTimer", this->CycleTimer },
                { "CyclerUserNames", this->cyclerUserNames },
                //{ "playerOutfits", this->playerOutfits },
                { "HostUsername", this->hostUserName },
                { "ChatMessage", this->chatMessage },
                { "CycleName", this->CycleName },
                { "CycleColor", this->RandomColor },
                { "CycleHat", this->RandomHat },
                { "CycleVisor", this->RandomVisor },
                { "CycleSkin", this->RandomSkin },
                { "CyclePet", this->RandomPet },
                { "CycleNamePlate", this->RandomNamePlate },

                { "PlayerSpeed", this->PlayerSpeed },
                { "MultiplySpeed", this->MultiplySpeed },
                { "KillDistance", this->KillDistance },
                { "ModifyKillDistance", this->ModifyKillDistance },
                { "ModifyTaskBarUpdates", this->ModifyTaskBarUpdates },
                { "UserName", this->userName },
                { "ShowGhosts", this->ShowGhosts },
                { "ShowPhantoms", this->ShowPhantoms },
                { "ShowPlayersInVents", this->ShowPlayersInVents },
                { "FakeRole", this->FakeRole },
                { "AutoFakeRole", this->AutoFakeRole },

                { "NoGameEnd", this->NoGameEnd },
                { "DisableMeetings", this->DisableMeetings },
                { "DisableSabotages", this->DisableSabotages },
                { "DisableAllVotekicks", this->DisableAllVotekicks },
                { "DisableRoleManager", this->DisableRoleManager },

                { "ShowRadar", this->ShowRadar },
                { "ShowRadar_DeadBodies", this->ShowRadar_DeadBodies },
                { "ShowRadar_Ghosts", this->ShowRadar_Ghosts },
                { "HideRadar_During_Meetings", this->HideRadar_During_Meetings },
                { "LockRadar", this->LockRadar },
                { "ShowRadar_RightClickTP", this->ShowRadar_RightClickTP },
                { "RadarColor_R", this->SelectedColor.x },
                { "RadarColor_G", this->SelectedColor.y },
                { "RadarColor_B", this->SelectedColor.z },
                { "RadarColor_A", this->SelectedColor.w },
                { "RadarDrawIcons", this->RadarDrawIcons },
                { "RadarVisorRoleColor", this->RadarVisorRoleColor },
                { "RadarBorder", this->RadarBorder },
                { "RadarExtraWidth", this->RadarExtraWidth },
                { "RadarExtraHeight", this->RadarExtraHeight },

                { "ShowReplay", this->ShowReplay },
                { "ReplayColor_R", this->SelectedReplayMapColor.x },
                { "ReplayColor_G", this->SelectedReplayMapColor.y },
                { "ReplayColor_B", this->SelectedReplayMapColor.z },
                { "ReplayColor_A", this->SelectedReplayMapColor.w },
                { "ReplayShowOnlyLastSeconds", this->Replay_ShowOnlyLastSeconds },
                { "ReplayLastSecondsValue", this->Replay_LastSecondsValue },
                { "ReplayClearAfterMeeting", this->Replay_ClearAfterMeeting },

                { "ShowEsp", this->ShowEsp },
                { "ShowEsp_Ghosts", this->ShowEsp_Ghosts },
                { "ShowEsp_Box", this->ShowEsp_Box },
                { "ShowEsp_Tracers", this->ShowEsp_Tracers },
                { "ShowEsp_Distance", this->ShowEsp_Distance },
                { "HideEsp_During_Meetings", this->HideEsp_During_Meetings },
                { "ShowEsp_RoleBased", this->ShowEsp_RoleBased },
                { "ShowEsp_Crew", this->ShowEsp_Crew },
                { "ShowEsp_Imp", this->ShowEsp_Imp },

                { "MaxVision", this->MaxVision },
                { "Wallhack", this->Wallhack },
                { "FreeCamSpeed", this->FreeCamSpeed },
                { "ZoomLevel", this->CameraHeight },
                { "UnlockVents", this->UnlockVents },
                { "UnlockKillButton", this->UnlockKillButton },
                { "ChatPaste", this->ChatPaste },
                { "RevealRoles", this->RevealRoles },
                { "AbbreviatedRoleNames", this->AbbreviatedRoleNames },
                { "PlayerColoredDots", this->PlayerColoredDots },
                { "ShowPlayerInfo", this->ShowPlayerInfo },
                { "ShowLobbyInfo", this->ShowLobbyInfo },
                { "ChatAlwaysActive", this->ChatAlwaysActive },
                { "ReadGhostMessages", this->ReadGhostMessages },
                { "ReadAndSendSickoChat", this->ReadAndSendSickoChat },
                { "CustomName", this->CustomName },
                { "RgbName", this->RgbName },
                { "UsePrefixAndSuffix", this->UsePrefixAndSuffix },
                { "PrefixAndSuffixNewLines", this->PrefixAndSuffixNewLines },
                { "NamePrefix", this->NamePrefix },
                { "NameSuffix", this->NameSuffix },
                { "Font", this->Font },
                { "FontType", this->FontType },
                { "ChatFont", this->ChatFont },
                { "ChatFontType", this->ChatFontType },
                //{ "Material", this->Material },
                //{ "MaterialType", this->MaterialType },
                { "ResizeName", this->ResizeName },
                { "IndentName", this->IndentName },
                { "CspaceName", this->CspaceName },
                { "MspaceName", this->MspaceName },
                { "VoffsetName", this->VoffsetName },
                { "RotateName", this->RotateName },
                { "NameSize", this->NameSize },
                { "NameIndent", this->NameIndent },
                { "NameCspace", this->NameCspace },
                { "NameMspace", this->NameMspace },
                { "NameVoffset", this->NameVoffset },
                { "NameRotate", this->NameRotate },
                { "ItalicName", this->ItalicName },
                { "UnderlineName", this->UnderlineName },
                { "StrikethroughName", this->StrikethroughName },
                { "BoldName", this->BoldName },
                { "NobrName", this->NobrName },
                { "ColoredName", this->ColoredName },
                { "NameColor1_R", this->NameColor1.x },
                { "NameColor1_G", this->NameColor1.y },
                { "NameColor1_B", this->NameColor1.z },
                { "NameColor1_A", this->NameColor1.w },
                { "NameColor2_R", this->NameColor2.x },
                { "NameColor2_G", this->NameColor2.y },
                { "NameColor2_B", this->NameColor2.z },
                { "NameColor2_A", this->NameColor2.w },
                { "AutoOpenDoors", this->AutoOpenDoors },
                { "MoveInVentAndShapeshift", this->MoveInVentAndShapeshift },
                { "AlwaysMove", this->AlwaysMove },
                { "AnimationlessShapeshift", this->AnimationlessShapeshift },
                { "DisableKillAnimation", this->DisableKillAnimation },
                { "KillImpostors", this->KillImpostors },
                { "KillInVanish", this->KillInVanish },
                { "BypassAngelProt", this->BypassAngelProt },
                { "InfiniteKillRange", this->InfiniteKillRange },
                { "AutoKill", this->AutoKill },
                { "FakeAlive", this->FakeAlive },
                { "HideWatermark", this->HideWatermark },
                { "ShowHost", this->ShowHost },
                { "ShowVoteKicks", this->ShowVoteKicks },
                { "ShowFps", this->ShowFps },
                { "DoTasksAsImpostor", this->DoTasksAsImpostor },
                { "AutoCopyLobbyCode", this->AutoCopyLobbyCode },
                { "DisableLobbyMusic", this->DisableLobbyMusic },
                { "ReportOnMurder", this->ReportOnMurder },
                { "PreventSelfReport", this->PreventSelfReport },
                //{ "AutoRejoin", this->AutoRejoin },
                { "OldStylePingText", this->OldStylePingText },
                { "NoSeekerAnim", this->NoSeekerAnim },
                { "BetterChatNotifications", this->BetterChatNotifications },
                { "BetterLobbyCodeInput", this->BetterLobbyCodeInput },
                { "BetterMessageSounds", this->BetterMessageSounds },
                { "NoClip", this->NoClip },
                { "KillInLobbies", this->KillInLobbies },
                { "KillInVanish", this->KillInVanish },
                { "GodMode", this->GodMode },

                { "RevealVotes", this->RevealVotes },
                { "RevealAnonymousVotes", this->RevealAnonymousVotes },
                { "ShowChatTimer", this->ShowChatTimer },
                { "ExtendChatLimit", this->ExtendChatLimit },
                { "ExtendChatHistory", this->ExtendChatHistory },
                { "AdjustByDPI", this->AdjustByDPI },
                { "ShowProtections", this->ShowProtections },

                { "CustomImpostorAmount", this->CustomImpostorAmount },
                { "ImpostorCount", this->ImpostorCount },

                { "ShowConsole", this->ShowConsole },
                { "ShowUnityLogs", this->ShowUnityLogs },
                //{ "ShowHookLogs", this->ShowHookLogs },

                { "ShiftRightClickTP", this->ShiftRightClickTP },
                { "RotateRadius", this->RotateRadius },
                { "RelativeTeleport", this->RelativeTeleport },
                { "ShowKillCD", this->ShowKillCD },

                { "Confuser", this->confuser },
                { "ConfuseOnJoin", this->confuseOnJoin },
                { "ConfuseOnStart", this->confuseOnStart },
                { "ConfuseOnKill", this->confuseOnKill },
                { "ConfuseOnVent", this->confuseOnVent },
                { "ConfuseOnMeeting", this->confuseOnMeeting },

                { "CyclerNameGeneration", this->cyclerNameGeneration },
                { "ConfuserNameGeneration", this->confuserNameGeneration },

                { "CustomCode", this->customCode },
                { "HideCode", this->HideCode },
                { "RgbLobbyCode", this->RgbLobbyCode },

                { "ShowLobbyTimer", this->ShowLobbyTimer },
                { "AutoStartGame", this->AutoStartGame },
                { "AutoStartTimer", this->AutoStartTimer },
                { "ModDetection", this->ModDetection },
                { "BroadcastedMod", this->BroadcastedMod },
                { "DisableHostAnticheat", this->DisableHostAnticheat },
                { "TournamentMode", this->TournamentMode },
                { "SpectatorMode", this->SpectatorMode },
                { "AlwaysAllowStart", this->AlwaysAllowStart },
                { "ModifyStartCountdown", this->ModifyStartCountdown },
                { "StartCountdown", this->StartCountdown },

                { "Enable_SMAC", this->Enable_SMAC },
                { "SMAC_Punishment", this->SMAC_Punishment },
                { "SMAC_HostPunishment", this->SMAC_HostPunishment },
                { "SMAC_AddToBlacklist", this->SMAC_AddToBlacklist },
                { "SMAC_PunishBlacklist", this->SMAC_PunishBlacklist },
                { "SMAC_IgnoreWhitelist", this->SMAC_IgnoreWhitelist },
                { "SMAC_CheckAUM", this->SMAC_CheckAUM },
                { "SMAC_CheckSicko", this->SMAC_CheckSicko },
                { "SMAC_CheckBadNames", this->SMAC_CheckBadNames },
                { "SMAC_CheckColor", this->SMAC_CheckColor },
                { "SMAC_CheckCosmetics", this->SMAC_CheckCosmetics },
                { "SMAC_CheckChatNote", this->SMAC_CheckChatNote },
                { "SMAC_CheckScanner", this->SMAC_CheckScanner },
                { "SMAC_CheckAnimation", this->SMAC_CheckAnimation },
                { "SMAC_CheckTasks", this->SMAC_CheckTasks },
                { "SMAC_CheckRole", this->SMAC_CheckRole },
                { "SMAC_CheckChat", this->SMAC_CheckChat },
                { "SMAC_CheckMeeting", this->SMAC_CheckMeeting },
                { "SMAC_CheckReport", this->SMAC_CheckReport },
                { "SMAC_CheckMurder", this->SMAC_CheckMurder },
                { "SMAC_CheckShapeshift", this->SMAC_CheckShapeshift },
                { "SMAC_CheckVanish", this->SMAC_CheckVanish },
                { "SMAC_CheckLevel", this->SMAC_CheckLevel },
                { "SMAC_CheckVent", this->SMAC_CheckVent },
                { "SMAC_CheckSabotage", this->SMAC_CheckSabotage },
                { "SMAC_HighLevel", this->SMAC_HighLevel },
                { "SMAC_LowLevel", this->SMAC_LowLevel },
                { "SMAC_CheckBadWords", this->SMAC_CheckBadWords },
                { "SMAC_BadWords", this->SMAC_BadWords },
                { "ChatPresets", this->ChatPresets },
                { "Destruct_IgnoreWhitelist", this->Destruct_IgnoreWhitelist },
                { "Ban_IgnoreWhitelist", this->Ban_IgnoreWhitelist },
                { "TimerAFK", this->TimerAFK },
                { "AddExtraTime", this->AddExtraTime },
                { "ExtraTimeThreshold", this->ExtraTimeThreshold },
                { "NotificationTimeWarn", this->NotificationTimeWarn },
                { "BypassVisualTasks", this->BypassVisualTasks },
                { "AutoHostRole", this->AutoHostRole },
                { "HostRoleToSet", this->HostRoleToSet },

                { "WhitelistFriendCodes", this->WhitelistFriendCodes },
                { "BlacklistFriendCodes", this->BlacklistFriendCodes },
                { "WarnedFriendCodes", this->WarnedFriendCodes },
                { "WarnReasons", this->WarnReasons },
                { "LockedNames", this->LockedNames },

                { "DisableMedbayScan", this->DisableMedbayScan },

                { "CrewmateGhostColor_R", this->CrewmateGhostColor.x },
                { "CrewmateGhostColor_G", this->CrewmateGhostColor.y },
                { "CrewmateGhostColor_B", this->CrewmateGhostColor.z },
                { "CrewmateGhostColor_A", this->CrewmateGhostColor.w },
                { "CrewmateColor_R", this->CrewmateColor.x },
                { "CrewmateColor_G", this->CrewmateColor.y },
                { "CrewmateColor_B", this->CrewmateColor.z },
                { "CrewmateColor_A", this->CrewmateColor.w },
                { "EngineerColor_R", this->EngineerColor.x },
                { "EngineerColor_G", this->EngineerColor.y },
                { "EngineerColor_B", this->EngineerColor.z },
                { "EngineerColor_A", this->EngineerColor.w },
                { "GuardianAngelColor_R", this->GuardianAngelColor.x },
                { "GuardianAngelColor_G", this->GuardianAngelColor.y },
                { "GuardianAngelColor_B", this->GuardianAngelColor.z },
                { "GuardianAngelColor_A", this->GuardianAngelColor.w },
                { "GuardianAngelColor_R", this->ScientistColor.x },
                { "GuardianAngelColor_G", this->ScientistColor.y },
                { "GuardianAngelColor_B", this->ScientistColor.z },
                { "GuardianAngelColor_A", this->ScientistColor.w },
                { "ScientistColor_R", this->ScientistColor.x },
                { "ScientistColor_G", this->ScientistColor.y },
                { "ScientistColor_B", this->ScientistColor.z },
                { "ScientistColor_A", this->ScientistColor.w },
                { "ImpostorColor_R", this->ImpostorColor.x },
                { "ImpostorColor_G", this->ImpostorColor.y },
                { "ImpostorColor_B", this->ImpostorColor.z },
                { "ImpostorColor_A", this->ImpostorColor.w },
                { "ShapeshifterColor_R", this->ShapeshifterColor.x },
                { "ShapeshifterColor_G", this->ShapeshifterColor.y },
                { "ShapeshifterColor_B", this->ShapeshifterColor.z },
                { "ShapeshifterColor_A", this->ShapeshifterColor.w },
                { "ImpostorGhostColor_R", this->ImpostorGhostColor.x },
                { "ImpostorGhostColor_G", this->ImpostorGhostColor.y },
                { "ImpostorGhostColor_B", this->ImpostorGhostColor.z },
                { "ImpostorGhostColor_A", this->ImpostorGhostColor.w },
                { "NoisemakerColor_R", this->NoisemakerColor.x },
                { "NoisemakerColor_G", this->NoisemakerColor.y },
                { "NoisemakerColor_B", this->NoisemakerColor.z },
                { "NoisemakerColor_A", this->NoisemakerColor.w },
                { "TrackerColor_R", this->TrackerColor.x },
                { "TrackerColor_G", this->TrackerColor.y },
                { "TrackerColor_B", this->TrackerColor.z },
                { "TrackerColor_A", this->TrackerColor.w },
                { "PhantomColor_R", this->PhantomColor.x },
                { "PhantomColor_G", this->PhantomColor.y },
                { "PhantomColor_B", this->PhantomColor.z },
                { "PhantomColor_A", this->PhantomColor.w },
                { "HostColor_R", this->HostColor.x },
                { "HostColor_G", this->HostColor.y },
                { "HostColor_B", this->HostColor.z },
                { "HostColor_A", this->HostColor.w },
                { "PlayerIdColor_R", this->PlayerIdColor.x },
                { "PlayerIdColor_G", this->PlayerIdColor.y },
                { "PlayerIdColor_B", this->PlayerIdColor.z },
                { "PlayerIdColor_A", this->PlayerIdColor.w },
                { "LevelColor_R", this->LevelColor.x },
                { "LevelColor_G", this->LevelColor.y },
                { "LevelColor_B", this->LevelColor.z },
                { "LevelColor_A", this->LevelColor.w },
                { "PlatformColor_R", this->PlatformColor.x },
                { "PlatformColor_G", this->PlatformColor.y },
                { "PlatformColor_B", this->PlatformColor.z },
                { "PlatformColor_A", this->PlatformColor.w },
                { "ModUsageColor_R", this->ModUsageColor.x },
                { "ModUsageColor_G", this->ModUsageColor.y },
                { "ModUsageColor_B", this->ModUsageColor.z },
                { "ModUsageColor_A", this->ModUsageColor.w },
                { "NameCheckerColor_R", this->NameCheckerColor.x },
                { "NameCheckerColor_G", this->NameCheckerColor.y },
                { "NameCheckerColor_B", this->NameCheckerColor.z },
                { "NameCheckerColor_A", this->NameCheckerColor.w },
                { "FriendCodeColor_R", this->FriendCodeColor.x },
                { "FriendCodeColor_G", this->FriendCodeColor.y },
                { "FriendCodeColor_B", this->FriendCodeColor.z },
                { "FriendCodeColor_A", this->FriendCodeColor.w },
                { "DaterNamesColor_R", this->DaterNamesColor.x },
                { "DaterNamesColor_G", this->DaterNamesColor.y },
                { "DaterNamesColor_B", this->DaterNamesColor.z },
                { "DaterNamesColor_A", this->DaterNamesColor.w },
                { "LobbyCodeColor_R", this->LobbyCodeColor.x },
                { "LobbyCodeColor_G", this->LobbyCodeColor.y },
                { "LobbyCodeColor_B", this->LobbyCodeColor.z },
                { "LobbyCodeColor_A", this->LobbyCodeColor.w },
                { "AgeColor_R", this->AgeColor.x },
                { "AgeColor_G", this->AgeColor.y },
                { "AgeColor_B", this->AgeColor.z },
                { "AgeColor_A", this->AgeColor.w },

                { "TempBanEnabled", this->TempBanEnabled },

                // Temp-Ban: Saving
                { "TempBannedFriendCodes", [&]() {
                    nlohmann::ordered_json banList = nlohmann::ordered_json::object();
                    for (const auto& [fc, until] : this->TempBannedFCs) {
                        std::time_t tt = std::chrono::system_clock::to_time_t(until);
                        std::tm tm{};
                        localtime_s(&tm, &tt);

                        char buffer[32];
                        std::strftime(buffer, sizeof(buffer), "%y/%m/%d %H:%M:%S", &tm);

                        banList[fc] = {
                            { "until", std::string(buffer) }
                        };
                    }
                    return banList;
                }() }
            };

            std::ofstream outSettings(settingsPath);
            outSettings << std::setw(4) << j << std::endl;
        }
        catch (...) {
            Log.Info("Unable to save " + std::format("sicko-config/{}.json", this->selectedConfig));
        }

        /*std::filesystem::path friendsPath = path.parent_path() / "friends.json";
        try
        {
            nlohmann::ordered_json j = nlohmann::ordered_json{
                { "Friends", this->Friends },
            };

            std::ofstream outFriends(friendsPath);
            outFriends << std::setw(4) << j << std::endl;
        }
        catch (...)
        {
            Log.Info("Unable to save friends.json");
        }*/
    }
}

void Settings::Delete() {
    auto path = getModulePath(hModule);

    auto configPath = path.parent_path() / std::format("sicko-config/{}.json", this->selectedConfig);

    std::filesystem::remove(configPath);
}
