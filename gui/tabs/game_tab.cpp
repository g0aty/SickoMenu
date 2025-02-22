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
            if (ImGui::Button("Add Word")) {
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

        if (openGeneral) {
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

            ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7)* State.dpiScale);

            if (IsHost() || !State.SafeMode) {
                CustomListBoxInt(" ", &State.SelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);
            }
            else {
                if (State.SelectedColorId >= (int)COLORS.size()) State.SelectedColorId = 0;
                CustomListBoxInt(" ", &State.SelectedColorId, COLORS, 85.0f * State.dpiScale);
            }
            ImGui::SameLine();
            if (ImGui::Button("Random Color"))
            {
                State.SelectedColorId = GetRandomColorId();
            }

            if (IsInGame() || IsInLobby()) {
                ImGui::SameLine();
                if (ImGui::Button("Set Color"))
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

            if (ImGui::Button("Join Lobby")) {
                AmongUsClient_CoJoinOnlineGameFromCode(*Game::pAmongUsClient,
                    GameCode_GameNameToInt(convert_to_string(State.AutoJoinLobbyCode), NULL),
                    NULL);
            }*/

            if (IsInGame() || IsInLobby()) ImGui::SameLine();
            if ((IsInGame() || IsInLobby()) && ImGui::Button("Reset Appearance"))
            {
                ControlAppearance(false);
            }


            if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && (IsHost() || !State.SafeMode) && ImGui::Button("Kill Everyone")) {
                for (auto player : GetAllPlayerControl()) {
                    if (IsInGame())
                        State.rpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                    else if (IsInLobby())
                        State.lobbyRpcQueue.push(new RpcMurderPlayer(*Game::pLocalPlayer, player));
                }
            }
            if (IsInLobby()) ImGui::SameLine();
            if (IsInLobby() && !State.SafeMode && ImGui::Button("Allow Everyone to NoClip")) {
                for (auto p : GetAllPlayerControl()) {
                    if (p != *Game::pLocalPlayer) State.lobbyRpcQueue.push(new RpcMurderLoop(*Game::pLocalPlayer, p, 1, true));
				}
                State.NoClip = true;
                ShowHudNotification("Allowed everyone to NoClip!");
            }
            /*if (IsHost() && (IsInGame() || IsInLobby()) && ImGui::Button("Spawn Dummy")) {
                if (IsInGame()) State.rpcQueue.push(new RpcSpawnDummy());
                if (IsInLobby()) State.lobbyRpcQueue.push(new RpcSpawnDummy());
            }*/
            if (IsInGame() || IsInLobby()) {
                ImGui::SameLine();
                if (ImGui::Button(IsHost() ? "Protect Everyone" : "Visual Protect Everyone")) {
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
            if (IsInGame()) ImGui::SameLine();
            if (IsInGame() && (IsHost() || !State.SafeMode) && ToggleButton("Spam Report", &State.SpamReport)) {
                State.Save();
            }

            if ((IsInGame() || (IsInLobby() && State.KillInLobbies)) && (IsHost() || !State.SafeMode)) {
                if (ImGui::Button("Kill All Crewmates")) {
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
                if (ImGui::Button("Kill All Impostors")) {
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
                    if (ImGui::Button("Suicide Crewmates")) {
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
                    if (ImGui::Button("Suicide Impostors")) {
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
            }

            if (IsInGame() || IsInLobby()) {
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && ImGui::Button("Scan Everyone")) {
                    for (auto p : GetAllPlayerControl()) {
                        if (IsInGame()) State.rpcQueue.push(new RpcForceScanner(p, true));
                        else State.lobbyRpcQueue.push(new RpcForceScanner(p, true));
                    }
                }
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks) && ImGui::Button("Stop Scanning Everyone")) {
                    for (auto p : GetAllPlayerControl()) {
                        if (IsInGame()) State.rpcQueue.push(new RpcForceScanner(p, false));
                        else State.lobbyRpcQueue.push(new RpcForceScanner(p, false));
                    }
                }
                if (!State.SafeMode && GameOptions().GetBool(BoolOptionNames__Enum::VisualTasks)) ImGui::SameLine();
                if (IsInGame() && !State.InMeeting && ImGui::Button("Kick Everyone From Vents")) {
                    State.rpcQueue.push(new RpcBootAllVents());
                }
                if (State.InMeeting) ImGui::SameLine();
                if ((IsHost() || !State.SafeMode) && State.InMeeting && ImGui::Button("End Meeting")) {
                    State.rpcQueue.push(new RpcEndMeeting());
                    State.InMeeting = false;
                }

                if (!State.SafeMode && ToggleButton("Force Name for Everyone", &State.ForceNameForEveryone)) {
                    State.Save();
                }
                if (!State.SafeMode && InputString("Username", &State.hostUserName)) {
                    State.Save();
                }

                if (!State.SafeMode) CustomListBoxInt(" ­", &State.HostSelectedColorId, HOSTCOLORS, 85.0f * State.dpiScale);

                if (!State.SafeMode && ToggleButton("Force Color for Everyone", &State.ForceColorForEveryone)) {
                    State.Save();
                }
            }
        }

        if (openChat) {
            if (InputStringMultiline("\n\n\n\n\nChat Message", &State.chatMessage)) {
                State.Save();
            }
            if ((IsInGame() || IsInLobby()) && State.ChatCooldown >= 3.f && State.chatMessage.size() <= 120) {
                ImGui::SameLine();
                if (ImGui::Button("Send"))
                {
                    auto player = (!State.SafeMode && State.playerToChatAs.has_value()) ?
                        State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
                    if (IsInGame()) {
                        State.rpcQueue.push(new RpcSendChat(player, State.chatMessage));
                        State.MessageSent = true;
                    }
                    else if (IsInLobby()) {
                        State.lobbyRpcQueue.push(new RpcSendChat(player, State.chatMessage));
                        State.MessageSent = true;
                    }
                }
            }
            if ((IsInGame() || IsInLobby()) && State.ReadAndSendAumChat) ImGui::SameLine();
            if (State.ReadAndSendAumChat && (IsInGame() || IsInLobby()) && ImGui::Button("Send to AUM"))
            {
                auto player = (!State.SafeMode && State.playerToChatAs.has_value()) ?
                    State.playerToChatAs.validate().get_PlayerControl() : *Game::pLocalPlayer;
                if (IsInGame()) {
                    State.rpcQueue.push(new RpcForceAumChat(PlayerSelection(player), State.chatMessage, true));
                }
                else if (IsInLobby()) {
                    State.lobbyRpcQueue.push(new RpcForceAumChat(PlayerSelection(player), State.chatMessage, true));
                }
            }

            if (ToggleButton("Spam", &State.ChatSpam))
            {
                State.Save();
            }
            if ((IsHost() || !State.SafeMode) && State.ChatSpamMode) ImGui::SameLine();
            if ((IsHost() || !State.SafeMode) && State.ChatSpamMode && ToggleButton("Spam by Everyone", &State.ChatSpamEveryone))
            {
                State.Save();
            }
            if (IsHost() || !State.SafeMode) {
                if (CustomListBoxInt("Chat Spam Mode", &State.ChatSpamMode,
                    { State.SafeMode ? "With Message (Self-Spam ONLY)" : "With Message", "Blank Chat", State.SafeMode ? "Self Message + Blank Chat" : "Message + Blank Chat" })) State.Save();
            }

            if (std::find(State.ChatPresets.begin(), State.ChatPresets.end(), State.chatMessage) == State.ChatPresets.end() && ImGui::Button("Add Message as Preset")) {
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
                if (ImGui::Button("Set as Chat Message"))
                {
                    State.chatMessage = msg;
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove"))
                    State.ChatPresets.erase(State.ChatPresets.begin() + selectedPresetIndex);
            }
        }

        if (openAnticheat) {
            if (ToggleButton("Enable Anticheat (SMAC)", &State.Enable_SMAC)) State.Save();
            if (IsHost()) CustomListBoxInt("Host Punishment­", &State.SMAC_HostPunishment, SMAC_HOST_PUNISHMENTS, 85.0f * State.dpiScale);
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
                if (newBFriendCode != "" && ImGui::Button("Add")) {
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
                    if (ImGui::Button("Delete"))
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
                if (newWFriendCode != "" && ImGui::Button("Add\n")) {
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
                    if (ImGui::Button("Delete\n"))
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
                if (ImGui::Button("Add Word")) {
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
                    if (ImGui::Button("Remove"))
                        State.SMAC_BadWords.erase(State.SMAC_BadWords.begin() + selectedWordIndex);
                }
            }
        }

        if (openDestruct) {
            if (!IsInLobby() && !IsInGame()) ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), ("Only available in game/lobby!"));
            if (ToggleButton("Ignore Whitelisted Players", &State.Destruct_IgnoreWhitelist)) {
                State.Save();
            }
            if (ToggleButton("Overload Everyone [Slow]", &State.OverloadEveryone)) {
                State.Save();
            }
            if (ToggleButton("Lag Everyone [Fast]", &State.LagEveryone)) {
                State.Save();
            }
            if (ToggleButton("Attempt to Crash Lobby", &State.CrashSpamReport)) {
                State.Save();
            }
            if (IsHost() && ImGui::Button("Remove Map")) {
                State.taskRpcQueue.push(new DestroyMap());
            }
            if (State.CrashSpamReport) ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ("When the game starts, the lobby is destroyed"));
        }

        if (openOptions) {
            if (GameOptions().HasOptions()) {
                GameOptions options;
                std::string hostText = std::format("Host: {}", RemoveHtmlTags(GetHostUsername()));
                ImGui::Text(const_cast<char*>(hostText.c_str()));

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

                    ImGui::Dummy(ImVec2(3, 3)* State.dpiScale);
                    ImGui::Separator();
                    ImGui::Dummy(ImVec2(3, 3)* State.dpiScale);

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
