#include "pch-il2cpp.h"
#include "state.hpp"
#include <iostream>
#include <fstream>
#include "main.h"
#include "utility.h"
#include "logger.h"

Settings State;

void Settings::Load() {
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
        JSON_TRYGET("SelectedConfigInt", this->selectedConfigInt);
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
        if (this->ShowMenuOnStartup)
            JSON_TRYGET("ShowMenu", this->ShowMenu);
        JSON_TRYGET("KeyBinds", this->KeyBinds);
#ifdef _DEBUG
        JSON_TRYGET("ShowDebug", this->showDebugTab);
#endif
        JSON_TRYGET("dpiScale", this->dpiScale);
        JSON_TRYGET("RgbTheme", this->RgbMenuTheme);
        JSON_TRYGET("GradientTheme", this->GradientMenuTheme);
        JSON_TRYGET("MatchBackgroundWithTheme", this->MatchBackgroundWithTheme);
        JSON_TRYGET("SetName", this->SetName);
        JSON_TRYGET("MenuThemeColor_R", this->MenuThemeColor.x);
        JSON_TRYGET("MenuThemeColor_G", this->MenuThemeColor.y);
        JSON_TRYGET("MenuThemeColor_B", this->MenuThemeColor.z);
        JSON_TRYGET("MenuThemeColor_A", this->MenuThemeColor.w);
        JSON_TRYGET("MenuGradientColor1_R", this->MenuGradientColor1.x);
        JSON_TRYGET("MenuGradientColor1_G", this->MenuGradientColor1.y);
        JSON_TRYGET("MenuGradientColor1_B", this->MenuGradientColor1.z);
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
        JSON_TRYGET("UseGuestFriendCode", this->UseGuestFriendCode);
        JSON_TRYGET("GuestFriendCode", this->GuestFriendCode);
        JSON_TRYGET("FakeFriendCode", this->FakeFriendCode);
        JSON_TRYGET("SpoofPlatform", this->SpoofPlatform);
        JSON_TRYGET("FakePlatform", this->FakePlatform);
        JSON_TRYGET("SpoofGuestAccount", this->SpoofGuestAccount);
        //JSON_TRYGET("SpoofModdedHost", this->SpoofModdedHost); haven't figured this out

        JSON_TRYGET("NoAbilityCD", this->NoAbilityCD);
        JSON_TRYGET("DarkMode", this->DarkMode);
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
        JSON_TRYGET("FakeRole", this->FakeRole);
        JSON_TRYGET("AutoFakeRole", this->AutoFakeRole);
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

        if (this->ShowMenuOnStartup)
            JSON_TRYGET("ShowReplay", this->ShowReplay);
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
        JSON_TRYGET("ReadAndSendAumChat", this->ReadAndSendAumChat);
        JSON_TRYGET("CustomName", this->CustomName);
        JSON_TRYGET("RgbName", this->RgbName);
        JSON_TRYGET("ResizeName", this->ResizeName);
        JSON_TRYGET("NameSize", this->NameSize);
        JSON_TRYGET("ItalicName", this->ItalicName);
        JSON_TRYGET("UnderlineName", this->UnderlineName);
        JSON_TRYGET("StrikethroughName", this->StrikethroughName);
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
        JSON_TRYGET("NoClip", this->NoClip);
        JSON_TRYGET("KillInLobbies", this->KillInLobbies);
        JSON_TRYGET("KillInVanish", this->KillInVanish);
        JSON_TRYGET("GodMode", this->GodMode);

        JSON_TRYGET("AdjustByDPI", this->AdjustByDPI);

        JSON_TRYGET("RevealVotes", this->RevealVotes);
        JSON_TRYGET("ShowProtections", this->ShowProtections);

        JSON_TRYGET("CustomImpostorAmount", this->CustomImpostorAmount);
        JSON_TRYGET("ImpostorCount", this->ImpostorCount);

        if (this->ShowMenuOnStartup)
            JSON_TRYGET("ShowConsole", this->ShowConsole);
        JSON_TRYGET("ShowUnityLogs", this->ShowUnityLogs);
        //JSON_TRYGET("ShowHookLogs", this->ShowHookLogs);

        JSON_TRYGET("RevealAnonymousVotes", this->RevealAnonymousVotes);

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
        JSON_TRYGET("SickoDetection", this->SickoDetection);
        JSON_TRYGET("DisableHostAnticheat", this->DisableHostAnticheat);
        JSON_TRYGET("TournamentMode", this->TournamentMode);
        JSON_TRYGET("SpectatorMode", this->SpectatorMode);

        JSON_TRYGET("Enable_SMAC", this->Enable_SMAC);
        JSON_TRYGET("SMAC_Punishment", this->SMAC_Punishment);
        JSON_TRYGET("SMAC_HostPunishment", this->SMAC_HostPunishment);
        JSON_TRYGET("SMAC_AddToBlacklist", this->SMAC_AddToBlacklist);
        JSON_TRYGET("SMAC_PunishBlacklist", this->SMAC_PunishBlacklist);
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
        
        JSON_TRYGET("WhitelistFriendCodes", this->WhitelistFriendCodes);
        JSON_TRYGET("BlacklistFriendCodes", this->BlacklistFriendCodes);
    } catch (...) {
        Log.Info("Unable to load " + std::format("sicko-config/{}.json", this->selectedConfig));
    }

    //Do not do any IL2CPP stuff here!  The constructors of most classes have not run yet!
}

void Settings::Save() {
    auto path = getModulePath(hModule);
    std::filesystem::create_directory(path.parent_path() / "sicko-config");

    auto configPath = path.parent_path() / "sicko-selected-config.json";

    if (this->selectedConfig != "") {
        try {
            nlohmann::ordered_json j = nlohmann::ordered_json{
                { "SelectedConfig", this->selectedConfig },
                { "SelectedConfigInt", this->selectedConfigInt },
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
                { "UseGuestFriendCode", this->UseGuestFriendCode },
                { "GuestFriendCode", this->GuestFriendCode },
                { "FakeFriendCode", this->FakeFriendCode },
                { "SpoofPlatform", this->SpoofPlatform },
                { "FakePlatform", this->FakePlatform },
                { "SpoofGuestAccount", this->SpoofGuestAccount },
                //{ "SpoofModdedHost", this->SpoofModdedHost }, haven't figured this out

                { "NoAbilityCD", this->NoAbilityCD },
                { "DarkMode", this->DarkMode },
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
                { "FakeRole", this->FakeRole },
                { "AutoFakeRole", this->AutoFakeRole },
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
                { "ReadAndSendAumChat", this->ReadAndSendAumChat },
                { "CustomName", this->CustomName },
                { "RgbName", this->RgbName },
                { "ResizeName", this->ResizeName },
                { "NameSize", this->NameSize },
                { "ItalicName", this->ItalicName },
                { "UnderlineName", this->UnderlineName },
                { "StrikethroughName", this->StrikethroughName },
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
                { "NoClip", this->NoClip },
                { "KillInLobbies", this->KillInLobbies },
                { "KillInVanish", this->KillInVanish },
                { "GodMode", this->GodMode },

                { "RevealVotes", this->RevealVotes },
                { "RevealAnonymousVotes", this->RevealAnonymousVotes },
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
                { "SickoDetection", this->SickoDetection },
                { "DisableHostAnticheat", this->DisableHostAnticheat },
                { "TournamentMode", this->TournamentMode },
                { "SpectatorMode", this->SpectatorMode },

                { "Enable_SMAC", this->Enable_SMAC },
                { "SMAC_Punishment", this->SMAC_Punishment },
                { "SMAC_HostPunishment", this->SMAC_HostPunishment },
                { "SMAC_AddToBlacklist", this->SMAC_AddToBlacklist },
                { "SMAC_PunishBlacklist", this->SMAC_PunishBlacklist },
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
                { "WhitelistFriendCodes", this->WhitelistFriendCodes },
                { "BlacklistFriendCodes", this->BlacklistFriendCodes },
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