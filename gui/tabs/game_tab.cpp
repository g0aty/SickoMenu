#include "pch-il2cpp.h"
#include "game_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "utility.h"
#include "state.hpp"
#include "logger.h"
/*#include <hunspell/hunspell.hxx>
#include <sstream>
#include <string>
#include <vector>
#include "imgui.h"

class SpellChecker {
public:
    SpellChecker(const std::string& affPath, const std::string& dicPath) {
        if (!Hunspell::isAvailable()) {
            throw std::runtime_error("Hunspell is not available.");
        }
        spell = new Hunspell(affPath.c_str(), dicPath.c_str());
        if (!spell->load()) {
            delete spell;
            throw std::runtime_error("Failed to load Hunspell dictionary.");
        }
    }

    ~SpellChecker() {
        delete spell;
    }

    bool isCorrect(const std::string& word) const {
        return spell->spell(word.c_str());
    }

private:
    Hunspell* spell;
};

void HighlightMisspelledWords(SpellChecker& checker, const std::string& text) {
    std::istringstream iss(text);
    std::string word;

    while (iss >> word) {

        bool isCorrect = checker.isCorrect(word);

        if (!isCorrect) {

            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", word.c_str());
        } else {

            ImGui::Text("%s ", word.c_str());
        }
    }
}

void RenderMenu() {
    try {
        SpellChecker spellChecker("en_US.aff", "en_US.dic");

        std::string chatMessage = "Ths is a smaple text with sme misspelled wrds.";

        if (ToggleButton("Blocked Words", &State.SMAC_CheckBadWords)) State.Save();
        if (State.SMAC_CheckBadWords) {
            HighlightMisspelledWords(spellChecker, chatMessage);

            static std::string newWord = "";
            InputString("New Word", &newWord, ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::SameLine();
            if (AnimatedButton("Add Word")) {
                State.SMAC_BadWords.push_back(newWord);
                State.Save();
                newWord = "";
            }


        }
    } catch (const std::exception& e) {

        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error: %s", e.what());
    }
}


bool ToggleButton(const char* label, bool* p_value) {
    return ImGui::Checkbox(label, p_value);
}

void InputString(const char* label, std::string* str, int flags = 0) {
    ImGui::InputText(label, &(*str)[0], str->capacity() + 1, flags);
}


struct State {
    bool SMAC_CheckBadWords;
    void Save() {}
    static std::vector<std::string> SMAC_BadWords;
};

std::vector<std::string> State::SMAC_BadWords;

int main() {


    while (true) {
        RenderMenu();


    }

    return 0;
}

*/
namespace GameTab {
    enum Groups {
        General,
        Chat,
        Anticheat,
        Destruct,
        Options
    };

    static bool openGeneral = true;
    static bool openChat = false;
    static bool openAnticheat = false;
    static bool openDestruct = false;
    static bool openOptions = false;

    void CloseOtherGroups(Groups group) {
        openGeneral = group == Groups::General;
        openChat = group == Groups::Chat;
        openAnticheat = group == Groups::Anticheat;
        openDestruct = group == Groups::Destruct;
        openOptions = group == Groups::Options;
    }

    void Render() {
        ImGui::SameLine(100 * State.dpiScale);
        ImGui::BeginChild("###Game", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        if (TabGroup("General", openGeneral)) {
            CloseOtherGroups(Groups::General);
        }
        ImGui::SameLine();
        if (TabGroup("Chat", openChat)) {
            CloseOtherGroups(Groups::Chat);
        }
        ImGui::SameLine();
        if (TabGroup("Anticheat", openAnticheat)) {
            CloseOtherGroups(Groups::Anticheat);
        }
        ImGui::SameLine();
        if (TabGroup("Destruct", openDestruct)) {
            CloseOtherGroups(Groups::Destruct);
        }

        if (GameOptions().HasOptions() && (IsInGame() || IsInLobby())) {
            ImGui::SameLine();
            if (TabGroup("Options", openOptions)) {
                CloseOtherGroups(Groups::Options);
            }
        }

        enum WarnViewType {
            WarnView_List = 0,
            WarnView_Manual,
            WarnView_COUNT
        };

        static int selectedWarnView = 0;
        const char* warnViewModes[WarnView_COUNT] = {
            "List View",
            "Manual Warn"
        };

        if (openGeneral) {
            ImGui::Dummy(ImVec2(2, 2) * State.dpiScale);
            if (SteppedSliderFloat("Player Speed Multiplier", &State.PlayerSpeed, 0.f, 10.f, 0.05f, "%.2fx", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
                State.PrevPlayerSpeed = State.PlayerSpeed;
            }
            if (SteppedSliderFloat("Kill Distance", &State.KillDistance, 0.f, 20.f, 0.1f, "%.1f m", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput)) {
                State.PrevKillDistance = State.KillDistance;
            }
            /*if (GameOptions().GetGameMode() == GameModes__Enum::Normal) {
                if (CustomListBoxInt("Task Bar Updates", &State.TaskBarUpdates, TASKBARUPDATES, 225 * State.dpiScale))
                    State.PrevTaskBarUpdates = State.TaskBarUpdates;
            }*/
            if (ToggleButton("No Ability Cooldown", &State.NoAbilityCD)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Multiply Speed", &State.MultiplySpeed)) {
                State.Save();
            }
            ImGui::SameLine();
            if (ToggleButton("Modify Kill Distance", &State.ModifyKillDistance)) {
                State.Save();
            }

            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

            if (IsHost() || !State.SafeMode) {
                CustomListBoxInt(" ", &State.SelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);
            }
            else {
                if (State.SelectedColorId >= (int)COLORS.size()) State.SelectedColorId = 0;
                CustomListBoxInt(" ", &State.SelectedColorId, COLORS, 85.0f * State.dpiScale);
            }
            ImGui::SameLine();
            if (AnimatedButton("Random Color"))
            {
                State.SelectedColorId = GetRandomColorId();
            }

            if (IsInGame() || IsInLobby()) {
                ImGui::SameLine();
                if (AnimatedButton("Set Color"))
                {
                    if (IsHost() || !State.SafeMode) {
                        if (IsInGame())
                            State.rpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new RpcForceColor(*Game::pLocalPlayer, State.SelectedColorId));
                    }
                    else if (IsColorAvailable(State.SelectedColorId)) {
                        if (IsInGame())
                            State.rpcQueue.push(new RpcSetColor(State.SelectedColorId));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new RpcSetColor(State.SelectedColorId));
                    }
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Snipe Color", &State.SnipeColor)) {
                State.Save();
            }

            if (ToggleButton("Console", &State.ShowConsole)) {
                State.Save();
            }

            /*if (ToggleButton("Auto-Join", &State.AutoJoinLobby))
                State.Save();
            ImGui::SameLine();
            if (InputString("Lobby Code", &State.AutoJoinLobbyCode))
                State.Save();

            if (AnimatedButton("Join Lobby")) {
                AmongUsClient_CoJoinOnlineGameFromCode(*Game::pAmongUsClient,
                    GameCode_GameNameToInt(convert_to_string(State.AutoJoinLobbyCode), NULL),
                    NULL);
            }*/

            if (IsInGame() || IsInLobby()) ImGui::SameLine();
            if ((IsInGame() || IsInLobby()) && AnimatedButton("Reset Appearance"))
            {
                ControlAppearance(false);
            }


            if (IsInGame() && (IsHost() || !State.SafeMode) && AnimatedButton("Kill Everyone")) {
                for (auto player : GetAllPlayerControl()) {
                    if (IsInGame() && (IsHost() || !State.SafeMode)) {
                        if (IsInGame())
                            State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                    }
                    else {
                        if (IsInGame())
                            State.rpcQueue.push(new FakeMurderPlayer(*Game::pLocalPlayer, player));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new FakeMurderPlayer(*Game::pLocalPlayer, player));
                    }
                }
            }
            if (IsInLobby() && !State.SafeMode) ImGui::SameLine();
            if (IsInLobby() && !State.SafeMode && AnimatedButton("Allow Everyone to NoClip")) {
                for (auto p : GetAllPlayerControl()) {
                    if (p != *Game::pLocalPlayer) State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, p, 1, true));
                }
                State.NoClip = true;
                ShowHudNotification("Allowed everyone to NoClip!");
            }
            /*if (IsHost() && (IsInGame() || IsInLobby()) && AnimatedButton("Spawn Dummy")) {
                auto outfit = GetPlayerOutfit(GetPlayerData(*Game::pLocalPlayer));
                if (IsInGame()) State.rpcQueue.push(new RpcSpawnDummy(outfit->fields.ColorId, convert_from_string(outfit->fields.PlayerName)));
                if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSpawnDummy(outfit->fields.ColorId, convert_from_string(outfit->fields.PlayerName)));
            }*/
            if ((IsInGame() || IsInLobby()) && ((IsHost() && IsInGame()) || !State.SafeMode)) {
                ImGui::SameLine();
                if (AnimatedButton(IsHost() ? "Protect Everyone" : "Visual Protect Everyone")) {
                    for (auto player : GetAllPlayerControl()) {
                        uint8_t colorId = GetPlayerOutfit(GetPlayerData(player))->fields.ColorId;
                        if (IsInGame())
                            State.rpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, PlayerSelection(player), colorId));
                        else if (IsInLobby())
                            State.lobbyRpcQueue.push(new RpcProtectPlayer(*Game::pLocalPlayer, PlayerSelection(player), colorId));
                    }
                }
            }

            if (IsInGame() && ToggleButton("Disable Venting", &State.DisableVents)) {
                State.Save();
            }
            if (IsInGame() && (IsHost() || !State.SafeMode)) ImGui::SameLine();
            if (IsInGame() && (IsHost() || !State.SafeMode) && ToggleButton("Spam Report", &State.SpamReport)) {
                State.Save();
            }

            if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && (IsHost() || !State.SafeMode)) {
                if (AnimatedButton("Kill All Crewmates")) {
                    for (auto player : GetAllPlayerControl()) {
                        if (!PlayerIsImpostor(GetPlayerData(player))) {
                            if (IsInGame())
                                State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                            else if (IsInLobby())
                                State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                        }
                    }
                }
                ImGui::SameLine();
                if (AnimatedButton("Kill All Impostors")) {
                    for (auto player : GetAllPlayerControl()) {
                        if (PlayerIsImpostor(GetPlayerData(player))) {
                            if (IsInGame())
                                State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player,
                                    player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
                            else if (IsInLobby())
                                State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player,
                                    player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
                        }
                    }
                }
                if (!State.SafeMode) {
                    ImGui::SameLine();
                    if (AnimatedButton("Suicide Crewmates")) {
                        for (auto player : GetAllPlayerControl()) {
                            if (!PlayerIsImpostor(GetPlayerData(player))) {
                                if (IsInGame())
                                    State.rpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
                                else if (IsInLobby())
                                    State.lobbyRpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
                            }
                        }
                    }
                    ImGui::SameLine();
                    if (AnimatedButton("Suicide Impostors")) {
                        for (auto player : GetAllPlayerControl()) {
                            if (PlayerIsImpostor(GetPlayerData(player))) {
                                if (IsInGame())
                                    State.rpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
                                else if (IsInLobby())
                                    State.lobbyRpcQueue.push(new RpcMurderPlayer(player, player,
                                        player->fields.protectedByGuardianId < 0 || State.BypassAngelProt));
                            }
                        }
                    }
                }

                static int ventId = 0;
                if (IsInGame() && (IsHost() || !State.SafeMode)) {
                    std::vector<const char*> allVents;
                    switch (State.mapType) {
                    case Settings::MapType::Ship:
                        allVents = SHIPVENTS;
                        break;
                    case Settings::MapType::Hq:
                        allVents = HQVENTS;
                        break;
                    case Settings::MapType::Pb:
                        allVents = PBVENTS;
                        break;
                    case Settings::MapType::Airship:
                        allVents = AIRSHIPVENTS;
                        break;
                    case Settings::MapType::Fungle:
                        allVents = FUNGLEVENTS;
                        break;
                    }
                    ventId = std::clamp(ventId, 0, (int)allVents.size() - 1);

                    ImGui::SetNextItemWidth(100 * State.dpiScale);
                    CustomListBoxInt("Vent", &ventId, allVents);
                    ImGui::SameLine();
                    if (AnimatedButton("Teleport All to Vent")) {
                        for (auto p : GetAllPlayerControl()) {
                            State.rpcQueue.push(new RpcBootFromVent(p, (State.mapType == Settings::MapType::Hq) ? ventId + 1 : ventId)); //MiraHQ vents start from 1 instead of 0
                        }
                    }
                }
            }

            if (IsInGame() || IsInLobby()) {
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && AnimatedButton("Scan Everyone")) {
                    for (auto p : GetAllPlayerControl()) {
                        if (IsInGame()) State.rpcQueue.push(new RpcForceScanner(p, true));
                        else State.lobbyRpcQueue.push(new RpcForceScanner(p, true));
                    }
                }
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && AnimatedButton("Stop Scanning Everyone")) {
                    for (auto p : GetAllPlayerControl()) {
                        if (IsInGame()) State.rpcQueue.push(new RpcForceScanner(p, false));
                        else State.lobbyRpcQueue.push(new RpcForceScanner(p, false));
                    }
                }
                if (IsInGame() && !State.InMeeting && !State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
                if (IsInGame() && !State.InMeeting && AnimatedButton("Kick Everyone From Vents")) {
                    State.rpcQueue.push(new RpcBootAllVents());
                }
                if ((IsHost() || !State.SafeMode) && State.InMeeting) ImGui::SameLine();
                if ((IsHost() || !State.SafeMode) && State.InMeeting && AnimatedButton("End Meeting")) {
                    State.rpcQueue.push(new RpcEndMeeting());
                    State.InMeeting = false;
                }

                if (!State.SafeMode && !IsHost()) {
                    if (AnimatedButton("Set Name for Everyone")) {
                        for (auto p : GetAllPlayerControl()) {
                            if (IsInGame()) State.rpcQueue.push(new RpcForceName(p, std::format("{}<size=0><{}></size>", State.hostUserName, p->fields.PlayerId)));
                            if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceName(p, std::format("{}<size=0><{}></size>", State.hostUserName, p->fields.PlayerId)));
                        }
                    }
                    ImGui::SameLine();
                    if (ToggleButton("Force Name for Everyone", &State.ForceNameForEveryone)) {
                        State.Save();
                    }

                    if (InputString("Username", &State.hostUserName)) {
                        State.Save();
                    }

                    if (AnimatedButton("Set Color for Everyone")) {
                        for (auto p : GetAllPlayerControl()) {
                            if (IsInGame()) State.rpcQueue.push(new RpcForceColor(p, State.HostSelectedColorId));
                            if (IsInLobby()) State.lobbyRpcQueue.push(new RpcForceColor(p, State.HostSelectedColorId));
                        }
                    }
                    ImGui::SameLine();
                    if (ToggleButton("Force Color for Everyone", &State.ForceColorForEveryone)) {
                        State.Save();
                    }

                    if (CustomListBoxInt(" ­", &State.HostSelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale)) State.Save();
                }
            }
        }

        if (openChat) {
            bool msgAllowed = IsChatValid(State.chatMessage);
            if (!msgAllowed) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.f, 0.f, State.MenuThemeColor.w));
                if (InputStringMultiline("\n\n\n\n\nChat Message", &State.chatMessage)) State.Save();
                ImGui::PopStyleColor();
            }
            else if (InputStringMultiline("\n\n\n\n\nChat Message", &State.chatMessage)) State.Save();
            if ((IsInGame() || IsInLobby()) && State.ChatCooldown >= 3.f && IsChatValid(State.chatMessage)) {
                ImGui::SameLine();
                if (AnimatedButton("Send"))
                {
                    auto player = (!State.SafeMode && State.playerToChatAs.has_value()) ?
                        State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
                    if (IsInGame()) State.rpcQueue.push(new RpcSendChat(player, State.chatMessage));
                    else if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSendChat(player, State.chatMessage));
                    State.MessageSent = true;
                }
            }
            if ((IsInGame() || IsInLobby()) && State.ReadAndSendSickoChat) ImGui::SameLine();
            if (State.ReadAndSendSickoChat && (IsInGame() || IsInLobby()) && AnimatedButton("Send to AUM"))
            {
                auto player = (!State.SafeMode && State.playerToChatAs.has_value()) ?
                    State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
                if (IsInGame()) {
                    State.rpcQueue.push(new RpcForceSickoChat(PlayerSelection(player), State.chatMessage, true));
                }
                else if (IsInLobby()) {
                    State.lobbyRpcQueue.push(new RpcForceSickoChat(PlayerSelection(player), State.chatMessage, true));
                }
            }

            if (ToggleButton("Spam", &State.ChatSpam))
            {
                if (State.BrainrotEveryone) State.BrainrotEveryone = false;
                if (State.RizzUpEveryone) State.RizzUpEveryone = false;
                State.Save();
            }
            if (((IsHost() && IsInGame()) || !State.SafeMode) && State.ChatSpamMode) ImGui::SameLine();
            if ((IsHost() || !State.SafeMode) && State.ChatSpamMode && ToggleButton("Spam by Everyone", &State.ChatSpamEveryone))
            {
                State.Save();
            }
            if (IsHost() || !State.SafeMode) {
                if (CustomListBoxInt("Chat Spam Mode", &State.ChatSpamMode,
                    { State.SafeMode ? "With Message (Self-Spam ONLY)" : "With Message", "Blank Chat", State.SafeMode ? "Self Message + Blank Chat" : "Message + Blank Chat" })) State.Save();
            }

            if (std::find(State.ChatPresets.begin(), State.ChatPresets.end(), State.chatMessage) == State.ChatPresets.end() && AnimatedButton("Add Message as Preset")) {
                State.ChatPresets.push_back(State.chatMessage);
                State.Save();
            }
            if (!(IsHost() || !State.SafeMode) && State.chatMessage.size() > 120) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Message will be detected by anticheat.");
            }
            if (!State.ChatPresets.empty()) {
                static int selectedPresetIndex = 0;
                selectedPresetIndex = std::clamp(selectedPresetIndex, 0, (int)State.ChatPresets.size() - 1);
                std::vector<const char*> presetVector(State.ChatPresets.size(), nullptr);
                for (size_t i = 0; i < State.ChatPresets.size(); i++) {
                    presetVector[i] = State.ChatPresets[i].c_str();
                }
                CustomListBoxInt("Message to Send/Remove", &selectedPresetIndex, presetVector);
                auto msg = State.ChatPresets[selectedPresetIndex];
                if (AnimatedButton("Set as Chat Message"))
                {
                    State.chatMessage = msg;
                }
                ImGui::SameLine();
                if (AnimatedButton("Remove"))
                    State.ChatPresets.erase(State.ChatPresets.begin() + selectedPresetIndex);
            }
        }

        if (openAnticheat) {
            if (ToggleButton("Enable Anticheat (SMAC)", &State.Enable_SMAC)) State.Save();
            if (IsHost()) CustomListBoxInt("Host Punishment ", &State.SMAC_HostPunishment, SMAC_HOST_PUNISHMENTS, 85.0f * State.dpiScale);
            else CustomListBoxInt("Regular Punishment", &State.SMAC_Punishment, SMAC_PUNISHMENTS, 85.0f * State.dpiScale);

            if (ToggleButton("Add Cheaters to Blacklist", &State.SMAC_AddToBlacklist)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Punish Blacklist", &State.SMAC_PunishBlacklist)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Ignore Whitelist", &State.SMAC_IgnoreWhitelist)) State.Save();
            if (State.SMAC_PunishBlacklist) {
                ImGui::Text("Blacklist");
                if (State.BlacklistFriendCodes.empty())
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No users in blacklist!");
                static std::string newBFriendCode = "";
                InputString("New Friend Code", &newBFriendCode, ImGuiInputTextFlags_EnterReturnsTrue);
                if (newBFriendCode != "") ImGui::SameLine();
                if (newBFriendCode != "" && AnimatedButton("Add")) {
                    State.BlacklistFriendCodes.push_back(newBFriendCode);
                    State.Save();
                    newBFriendCode = "";
                }

                if (!State.BlacklistFriendCodes.empty()) {
                    static int selectedBCodeIndex = 0;
                    selectedBCodeIndex = std::clamp(selectedBCodeIndex, 0, (int)State.BlacklistFriendCodes.size() - 1);
                    std::vector<const char*> bCodeVector(State.BlacklistFriendCodes.size(), nullptr);
                    for (size_t i = 0; i < State.BlacklistFriendCodes.size(); i++) {
                        bCodeVector[i] = State.BlacklistFriendCodes[i].c_str();
                    }
                    CustomListBoxInt("Player to Delete", &selectedBCodeIndex, bCodeVector);
                    ImGui::SameLine();
                    if (AnimatedButton("Delete"))
                        State.BlacklistFriendCodes.erase(State.BlacklistFriendCodes.begin() + selectedBCodeIndex);
                }
            }
            if (State.SMAC_IgnoreWhitelist) {
                ImGui::Text("Whitelist");
                if (State.WhitelistFriendCodes.empty())
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No users in whitelist!");
                static std::string newWFriendCode = "";
                InputString("New Friend Code\n", &newWFriendCode, ImGuiInputTextFlags_EnterReturnsTrue);
                if (newWFriendCode != "") ImGui::SameLine();
                if (newWFriendCode != "" && AnimatedButton("Add\n")) {
                    State.WhitelistFriendCodes.push_back(newWFriendCode);
                    State.Save();
                    newWFriendCode = "";
                }

                if (!State.WhitelistFriendCodes.empty()) {
                    static int selectedWCodeIndex = 0;
                    selectedWCodeIndex = std::clamp(selectedWCodeIndex, 0, (int)State.WhitelistFriendCodes.size() - 1);
                    std::vector<const char*> wCodeVector(State.WhitelistFriendCodes.size(), nullptr);
                    for (size_t i = 0; i < State.WhitelistFriendCodes.size(); i++) {
                        wCodeVector[i] = State.WhitelistFriendCodes[i].c_str();
                    }
                    CustomListBoxInt("Player to Delete\n", &selectedWCodeIndex, wCodeVector);
                    ImGui::SameLine();
                    if (AnimatedButton("Delete\n"))
                        State.WhitelistFriendCodes.erase(State.WhitelistFriendCodes.begin() + selectedWCodeIndex);
                }
            }
            ImGui::Text("Detect Actions:");
            if (ToggleButton("AUM/KillNetwork Usage", &State.SMAC_CheckAUM)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("SickoMenu Usage", &State.SMAC_CheckSicko)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Names", &State.SMAC_CheckBadNames)) State.Save();

            if (ToggleButton("Abnormal Set Color", &State.SMAC_CheckColor)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Set Cosmetics", &State.SMAC_CheckCosmetics)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Chat Note", &State.SMAC_CheckChatNote)) State.Save();

            if (ToggleButton("Abnormal Scanner", &State.SMAC_CheckScanner)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Animation", &State.SMAC_CheckAnimation)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Setting Tasks", &State.SMAC_CheckTasks)) State.Save();

            if (ToggleButton("Abnormal Murders", &State.SMAC_CheckMurder)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Shapeshift", &State.SMAC_CheckShapeshift)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Vanish", &State.SMAC_CheckVanish)) State.Save();

            if (ToggleButton("Abnormal Meetings/Body Reports", &State.SMAC_CheckReport)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Player Levels (0 to ignore)", &State.SMAC_CheckLevel)) State.Save();
            if (State.SMAC_CheckLevel && ImGui::InputInt("Level >=", &State.SMAC_HighLevel)) {
                State.Save();
            }
            if (State.SMAC_CheckLevel && ImGui::InputInt("Level <=", &State.SMAC_LowLevel)) {
                State.Save();
            }

            if (ToggleButton("Abnormal Venting", &State.SMAC_CheckVent)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Sabotages", &State.SMAC_CheckSabotage)) State.Save();
            ImGui::SameLine();
            if (ToggleButton("Abnormal Chat", &State.SMAC_CheckChat)) State.Save();

            if (ToggleButton("Blocked Words", &State.SMAC_CheckBadWords)) State.Save();
            if (State.SMAC_CheckBadWords) {
                if (State.SMAC_BadWords.empty())
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No bad words added!");
                static std::string newWord = "";
                InputString("New Word", &newWord, ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::SameLine();
                if (AnimatedButton("Add Word")) {
                    State.SMAC_BadWords.push_back(newWord);
                    State.Save();
                    newWord = "";
                }
                if (!State.SMAC_BadWords.empty()) {
                    static int selectedWordIndex = 0;
                    selectedWordIndex = std::clamp(selectedWordIndex, 0, (int)State.SMAC_BadWords.size() - 1);
                    std::vector<const char*> wordVector(State.SMAC_BadWords.size(), nullptr);
                    for (size_t i = 0; i < State.SMAC_BadWords.size(); i++) {
                        wordVector[i] = State.SMAC_BadWords[i].c_str();
                    }
                    CustomListBoxInt("Word to Remove", &selectedWordIndex, wordVector);
                    ImGui::SameLine();
                    if (AnimatedButton("Remove"))
                        State.SMAC_BadWords.erase(State.SMAC_BadWords.begin() + selectedWordIndex);
                }
            }
        }

        if (openDestruct) {
            /*if (ToggleButton("Ignore Whitelisted Players [Exploits]", &State.Destruct_IgnoreWhitelist)) {
                State.Save();
            }*/
            if (ToggleButton("Ignore Whitelisted Players [Ban/Kick]", &State.Ban_IgnoreWhitelist)) {
                State.Save();
            }
            if (IsInLobby() && ToggleButton("Attempt to Crash Lobby", &State.CrashSpamReport)) {
                State.Save();
            }
            if (State.CrashSpamReport) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("When the game starts, the lobby is destroyed"));
            if (State.AprilFoolsMode) {
                ImGui::TextColored(ImVec4(0.79f, 0.03f, 1.f, 1.f), State.DiddyPartyMode ? "Diddy Party Mode" : (IsChatCensored() || IsStreamerMode() ? "F***son Mode" : "Fuckson Mode"));
                if (ToggleButton("Mog Everyone [Sigma]", &State.BrainrotEveryone)) {
                    if (State.ChatSpam) State.ChatSpam = false;
                    if (State.RizzUpEveryone) State.RizzUpEveryone = false;
                    State.Save();
                }
                if (State.DiddyPartyMode && ToggleButton("Rizz Up Everyone [Skibidi]", &State.RizzUpEveryone)) {
                    if (State.ChatSpam) State.ChatSpam = false;
                    if (State.BrainrotEveryone) State.BrainrotEveryone = false;
                    State.Save();
                }
            }
            if (IsHost()) {
                ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
                if (((IsInGame() && Object_1_IsNotNull((Object_1*)*Game::pShipStatus)) || (IsInLobby() && Object_1_IsNotNull((Object_1*)*Game::pLobbyBehaviour)))
                    && AnimatedButton(IsInLobby() ? "Remove Lobby" : "Remove Map")) {
                    State.taskRpcQueue.push(new DestroyMap());
                }
                ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
                if (ToggleButton("Ban Everyone", &State.BanEveryone)) {
                    State.Save();
                }
                if (ToggleButton("Kick Everyone", &State.KickEveryone)) {
                    State.Save();
                }
                ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
                if (IsInGame() && ToggleButton("Kick AFK Players", &State.KickAFK)) {
                    State.Save();
                }
                ImGui::SameLine();
                if (State.KickAFK && ToggleButton("Activate AFK Notifications", &State.NotificationsAFK)) {
                    State.Save();
                }
                std::string header = "Anti AFK ~ Advanced Options";
                if (!IsInGame()) {
                    header += " [GAME-MATCH]";
                }
                ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
                if (ImGui::CollapsingHeader(header.c_str()))
                {
                    if (SteppedSliderFloat("Time Before Kick", &State.TimerAFK, 40.f, 350.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        State.Save();
                    }
                    if (SteppedSliderFloat("Extra Time at The Meeting", &State.AddExtraTime, 15.f, 120.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        State.Save();
                    }
                    if (SteppedSliderFloat("Minimum Sec to Extra Time", &State.ExtraTimeThreshold, 5.f, 60.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        State.Save();
                    }
                    if (State.NotificationsAFK && SteppedSliderFloat("Warn-AFK Notifications Time", &State.NotificationTimeWarn, 5.f, 60.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        State.Save();
                    }
                }
                ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                ImGui::Separator();
                ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                if (ToggleButton("Whitelisted Players Only", &State.KickByWhitelist)) {
                    State.Save();
                }
                if (State.KickByWhitelist) ImGui::SameLine();
                if (State.KickByWhitelist && ToggleButton("Enable WL Notifications", &State.WhitelistNotifications)) {
                    State.Save();
                }
                ImGui::Dummy(ImVec2(15, 15) * State.dpiScale);
                if (ToggleButton("Ban Auto-Rejoin Players", &State.BanLeavers)) {
                    State.Save();
                }
                ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
                if (ImGui::CollapsingHeader("BA-RP ~ Advanced Options"))
                {
                    if (SteppedSliderFloat("Maximum Rejoins", &State.LeaveCount, 1.f, 15.f, 1.f, "%.0f", ImGuiSliderFlags_NoInput)) {
                        State.Save();
                    }
                    ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
                    if (ToggleButton("Blacklist Auto-Rejoin Players", &State.BL_AutoLeavers)) {
                        State.Save();
                    }
                }
                ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                ImGui::Separator();
                ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                if (ToggleButton("Kick By Name-Checker", &State.KickByLockedName)) {
                    State.Save();
                }
                if (State.KickByLockedName) ImGui::SameLine();
                if (State.KickByLockedName && ToggleButton("Show Player Data Notifications", &State.ShowPDataByNC)) {
                    State.Save();
                }
                if (State.KickByLockedName) {
                    ImGui::Text("Blocked Names");
                    if (State.LockedNames.empty())
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No users in Name-Checker!");
                    static std::string newName = "";
                    InputString("New Nickname", &newName, ImGuiInputTextFlags_EnterReturnsTrue);
                    if (newName != "") ImGui::SameLine();
                    if (newName != "" && AnimatedButton("Add")) {
                        State.LockedNames.push_back(newName);
                        State.Save();
                        newName = "";
                    }

                    if (!State.LockedNames.empty()) {
                        static int selectedName = 0;
                        selectedName = std::clamp(selectedName, 0, (int)State.LockedNames.size() - 1);
                        std::vector<const char*> bNameVector(State.LockedNames.size(), nullptr);
                        for (size_t i = 0; i < State.LockedNames.size(); i++) {
                            bNameVector[i] = State.LockedNames[i].c_str();
                        }
                        CustomListBoxInt("Nickname to Delete", &selectedName, bNameVector);
                        ImGui::SameLine();
                        if (AnimatedButton("Delete"))
                            State.LockedNames.erase(State.LockedNames.begin() + selectedName);
                    }
                }
                ImGui::Dummy(ImVec2(15, 15) * State.dpiScale);
                ImGui::BeginGroup();
                if (ToggleButton("Kick Warned Players", &State.KickWarned)) {
                    State.Save();
                }
                if (ToggleButton("Ban Warned Players", &State.BanWarned)) {
                    State.Save();
                }

                ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);

                ImGui::PushItemWidth(80);
                ImGui::InputInt("Max Warns", &State.MaxWarns);
                if (State.minFpsThreshold < 1)
                    State.minFpsThreshold = 1;
                ImGui::PopItemWidth();
                ImGui::EndGroup();
            }
            if (IsHost()) ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::PushItemWidth(150);
            if (!IsHost()) ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
            ImGui::Combo("Warn View Mode", &selectedWarnView, warnViewModes, WarnView_COUNT);
            ImGui::PopItemWidth();


            if (selectedWarnView == WarnView_List) {
                if (!State.WarnedFriendCodes.empty()) {
                    ImGui::Text("Warned Players");

                    std::string localFC = "";
                    if (Game::pLocalPlayer && *Game::pLocalPlayer) {
                        localFC = convert_from_string((*Game::pLocalPlayer)->fields.FriendCode);
                    }

                    std::vector<std::string> warnedList;
                    std::vector<std::string> fcKeys;

                    for (const auto& [fc, count] : State.WarnedFriendCodes) {
                        if (count <= 0 || fc == localFC)
                            continue;

                        warnedList.push_back(std::format("{} ({} warn{})", fc, count, count == 1 ? "" : "s"));
                        fcKeys.push_back(fc);
                    }

                    if (!warnedList.empty()) {
                        static int selectedWarned = 0;
                        selectedWarned = std::clamp(selectedWarned, 0, (int)warnedList.size() - 1);

                        std::vector<const char*> warnedCStrs;
                        for (const auto& entry : warnedList) warnedCStrs.push_back(entry.c_str());

                        ImGui::PushItemWidth(200);
                        CustomListBoxInt("Warned FriendCodes", &selectedWarned, warnedCStrs);
                        ImGui::PopItemWidth();

                        ImGui::SameLine();
                        if (ImGui::Button("Remove")) {
                            if (selectedWarned >= 0 && selectedWarned < (int)fcKeys.size()) {
                                std::string fc = fcKeys[selectedWarned];
                                State.WarnedFriendCodes.erase(fc);
                                State.WarnReasons.erase(fc);
                                selectedWarned = 0;
                                State.Save();
                            }
                        }

                        std::string selectedFc = fcKeys[selectedWarned];
                        auto& warnReasons = State.WarnReasons[selectedFc];

                        if (!warnReasons.empty()) {
                            ImGui::Text("Warn Reasons:");

                            static int selectedReason = 0;
                            selectedReason = std::clamp(selectedReason, 0, (int)warnReasons.size() - 1);

                            std::vector<std::string> numberedReasons;
                            numberedReasons.reserve(warnReasons.size());
                            for (size_t i = 0; i < warnReasons.size(); ++i) {
                                numberedReasons.push_back(std::format("[{}] {}", i + 1, warnReasons[i]));
                            }

                            std::vector<const char*> reasonCStrs;
                            for (const auto& str : numberedReasons) reasonCStrs.push_back(str.c_str());

                            ImGui::PushItemWidth(200);
                            ImGui::ListBox("##WarnReasonList", &selectedReason, reasonCStrs.data(), (int)reasonCStrs.size());
                            ImGui::PopItemWidth();

                            ImGui::SameLine();
                            if (ImGui::Button("Delete")) {
                                if (selectedReason >= 0 && selectedReason < (int)warnReasons.size()) {
                                    warnReasons.erase(warnReasons.begin() + selectedReason);
                                    selectedReason = 0;

                                    if (--State.WarnedFriendCodes[selectedFc] <= 0) {
                                        State.WarnedFriendCodes.erase(selectedFc);
                                        State.WarnReasons.erase(selectedFc);
                                        selectedWarned = 0;
                                    }

                                    State.Save();
                                }
                            }
                        }
                    }
                    else {
                        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No warned players.");
                    }
                }
            }
            else if (selectedWarnView == WarnView_Manual) {
                static std::string friendCodeToWarn;
                static std::string warnReason;

                ImGui::PushItemWidth(200);
                InputString("FriendCode", &friendCodeToWarn);
                InputString("Reason", &warnReason);
                ImGui::PopItemWidth();

                if (ImGui::Button("Submit Warn") && !friendCodeToWarn.empty() && !warnReason.empty()) {
                    State.WarnedFriendCodes[friendCodeToWarn]++;
                    State.WarnReasons[friendCodeToWarn].push_back(warnReason);
                    State.Save();

                    friendCodeToWarn.clear();
                    warnReason.clear();
                }
            }

            ImGui::EndGroup();
        }

        if (openOptions) {
            if ((IsInGame() || IsInLobby()) && GameOptions().HasOptions()) {
                GameOptions options;
                /*std::string hostText = std::format("Host: {}", RemoveHtmlTags(GetHostUsername()));
                ImGui::Text(const_cast<char*>(hostText.c_str()));*/

                if (options.GetGameMode() == GameModes__Enum::Normal)
                {
                    auto allPlayers = GetAllPlayerControl();
                    RoleRates roleRates = RoleRates(options, (int)allPlayers.size());
                    // this should be all the major ones. if people want more they're simple enough to add.
                    ImGui::Text("Visual Tasks: %s", (options.GetBool(app::BoolOptionNames__Enum::VisualTasks) ? "On" : "Off"));
                    switch (options.GetInt(app::Int32OptionNames__Enum::TaskBarMode)) {
                    case 0:
                        ImGui::Text("Task Bar Updates: Always");
                        break;
                    case 1:
                        ImGui::Text("Task Bar Updates: Meetings");
                        break;
                    case 2:
                        ImGui::Text("Task Bar Updates: Never");
                        break;
                    default:
                        ImGui::Text("Task Bar Updates: Other");
                        break;
                    }
                    ImGui::Text("Confirm Ejects: %s", (options.GetBool(app::BoolOptionNames__Enum::ConfirmImpostor) ? "On" : "Off"));
                    switch (options.GetInt(app::Int32OptionNames__Enum::KillDistance)) {
                    case 0:
                        ImGui::Text("Kill Distance: Short");
                        break;
                    case 1:
                        ImGui::Text("Kill Distance: Medium");
                        break;
                    case 2:
                        ImGui::Text("Kill Distance: Long");
                        break;
                    default:
                        ImGui::Text("Kill Distance: Other");
                        break;
                    }

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Engineers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Engineer));
                    ImGui::Text("Engineer Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Engineer));
                    ImGui::Text("Engineer Vent Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EngineerCooldown, 1.0F));
                    ImGui::Text("Engineer Duration in Vent: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EngineerInVentMaxTime, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Scientists: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Scientist));
                    ImGui::Text("Scientist Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Scientist));
                    ImGui::Text("Scientist Vitals Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ScientistCooldown, 1.0F));
                    ImGui::Text("Scientist Battery Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ScientistBatteryCharge, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Guardian Angels: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::GuardianAngel));
                    ImGui::Text("Guardian Angel Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::GuardianAngel));
                    ImGui::Text("Guardian Angel Protect Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::GuardianAngelCooldown, 1.0F));
                    ImGui::Text("Guardian Angel Protection Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ProtectionDurationSeconds, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Shapeshifters: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Shapeshifter));
                    ImGui::Text("Shapeshifter Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Shapeshifter));
                    ImGui::Text("Shapeshifter Shift Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterCooldown, 1.0F));
                    ImGui::Text("Shapeshifter Shift Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::ShapeshifterDuration, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Noisemakers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Noisemaker));
                    ImGui::Text("Noisemaker Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Noisemaker));
                    ImGui::Text("Noisemaker Alert Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::NoisemakerAlertDuration, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Trackers: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Tracker));
                    ImGui::Text("Tracker Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Tracker));
                    ImGui::Text("Tracking Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerDuration, 1.0F));
                    ImGui::Text("Tracking Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerCooldown, 1.0F));
                    ImGui::Text("Tracking Delay: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::TrackerDelay, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Max Phantoms: %d", roleRates.GetRoleCount(app::RoleTypes__Enum::Phantom));
                    ImGui::Text("Phantom Chance: %d%", options.GetRoleOptions().GetChancePerGame(RoleTypes__Enum::Phantom));
                    ImGui::Text("Phantom Vanish Cooldown: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::PhantomCooldown, 1.0F));
                    ImGui::Text("Phantom Vanish Duration: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::PhantomDuration, 1.0F));
                }
                else if (options.GetGameMode() == GameModes__Enum::HideNSeek) {

                    int ImpostorId = options.GetInt(app::Int32OptionNames__Enum::ImpostorPlayerID);
                    if (ImpostorId < 0) {
                        ImGui::Text("Impostor: Round-robin");
                    }
                    else {
                        std::string ImpostorName = std::format("Selected Impostor: {}", convert_from_string(NetworkedPlayerInfo_get_PlayerName(GetPlayerDataById(ImpostorId), nullptr)));
                        ImGui::Text(const_cast<char*>(ImpostorName.c_str()));
                    }
                    ImGui::Text("Flashlight Mode: %s", (options.GetBool(app::BoolOptionNames__Enum::UseFlashlight) ? "On" : "Off"));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Vent Uses: %d", options.GetInt(app::Int32OptionNames__Enum::CrewmateVentUses));
                    ImGui::Text("Duration in Vent: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::CrewmateTimeInVent, 1.0F));

                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

                    ImGui::Text("Hiding Time: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::EscapeTime, 1.0F));
                    ImGui::Text("Final Hiding Time: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::FinalEscapeTime, 1.0F));
                    ImGui::Text("Final Impostor Speed: %.2f s", options.GetFloat(app::FloatOptionNames__Enum::SeekerFinalSpeed, 1.0F));
                }
            }
            else CloseOtherGroups(Groups::General);
        }
        ImGui::EndChild();
    }
}
