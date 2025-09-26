#include "pch-il2cpp.h"
#include "menu.hpp"
#include "imgui/imgui.h"
#include "tabs/about_tab.h"
#include "tabs/doors_tab.h"
#include "tabs/game_tab.h"
#include "tabs/host_tab.h"
#include "tabs/players_tab.h"
#include "tabs/radar_tab.h"
#include "tabs/replay_tab.h"
#include "tabs/esp_tab.h"
#include "tabs/sabotage_tab.h"
#include "tabs/self_tab.h"
#include "tabs/settings_tab.h"
#include "tabs/tasks_tab.h"
#ifdef _DEBUG
#include "tabs/debug_tab.h"
#endif
#include "state.hpp"
#include "gui-helpers.hpp"
#include <map>
#include <vector>
#include <string>

namespace Menu {
	static bool openAbout = false;
	static bool openSettings = false;
	static bool openGame = false;
	static bool openSelf = false;
	static bool openRadar = false;
	static bool openReplay = false;
	static bool openEsp = false;
	static bool openPlayers = false;
	static bool openTasks = false;
	static bool openSabotage = false;
	static bool openDoors = false;
	static bool openHost = false;
#ifdef _DEBUG
	static bool openDebug = false;
#endif
	static std::string searchQuery = (std::string)"";

	std::map<std::string, std::vector<std::string>> categories = {
		{"Settings", {"Show Keybinds", "Allow Activating Keybinds while Chatting", "Always Show Menu on Startup", "Panic Warning",
					  "Config Name", "Load Config", "Save Config", "Adjust by DPI", "Menu Scale", "Menu Theme Color", "Gradient Theme", "Match Background with Theme",
					  "RGB Menu Theme", "Reset Menu Theme", "Opacity", "Show Debug Tab", "Username", "Set as Account Name", "Automatically Set Name", "Custom Code",
					  "Replace Streamer Mode Lobby Code", "RGB Lobby Code", "Unlock Cosmetics", "Safe Mode", "Allow other SickoMenu users to see you're using SickoMenu",
					  "Spoof Guest Account", "Use Custom Guest Friend Code", "Spoof Level", "Spoof Platform", "Disable Host Anticheat (+25 Mode)", "FPS"}},
		{"Game", {"Player Speed Multiplier", "Kill Distance", "No Ability Cooldown", "Multiply Speed", "Modify Kill Distance", "Random Color", "Set Color", "Snipe Color", "Console",
				  "Reset Appearance", "Kill Everyone", "Protect Everyone", "Disable Venting", "Spam Report", "Kill All Crewmates", "Kill All Impostors", "Kick Everyone From Vents",
				  "Chat Message", "Send", "Send to AUM", "Spam", "Chat Presets", "Attempt to Crash", "Overload Everyone", "Lag Everyone", "Enable Anticheat (SMAC)",
				  "Whitelist", "Blacklist"}},
		{"Self", {"Max Vision", "Wallhack", "Disable HUD", "Freecam", "Zoom", "Always show Chat Button", "Allow Ctrl+(C/V) in Chat", "Read Messages by Ghosts",
				  "Read and Send SickoChat", "Custom Name", "Custom Name for Everyone", "Server-sided Custom Name", "Reveal Roles", "Abbrv. Role", "Player Colored Dots Next To Names",
				  "Show Player Info in Lobby", "Reveal Votes", "See Ghosts", "See Protections", "See Kill Cooldown", "Disable Kill Animation", "Dark Mode",
				  "Show Host", "Hide Watermark", "Show Vote Kicks", "Show FPS",
				  "Unlock Vents", "Move While in Vent & Shapeshifting", "Always Move", "No Shapeshift Animation", "Copy Lobby Code on Disconnect", "NoClip",
				  "Allow Killing in Lobbies", "Kill Other Impostors", "Infinite Kill Range", "Bypass Guardian Angel Protections", "Autokill", "Do Tasks as Impostor",
				  "Fake Alive", "God Mode", "Teleport", "Rotate everyone", "Select Role", "Set Role", "Set Fake Role", "Automatically Set Fake Role",
				  "Show Lobby Info", "See Phantoms", "Report Body on Murder", "Prevent Self-Report",
				  "Cycler", "Cycle in Meeting", "Cycle Between Players", "Confuser (Randomize Appearance at Will"}},
		{"Radar", {"Show Radar", "Show Dead Bodies", "Show Ghosts", "Right Click to Teleport", "Hide Radar During Meetings", "Draw Player Icons", "Lock Radar Position", "Show Border"}},
		{"Replay", {"Show Replay", "Show Only last seconds", "Clear after meeting"}},
		{"ESP", {"Enable", "Show Ghosts", "Hide During Meetings", "Show Boxes", "Show Tracers", "Show Distances", "Role-based"}},
		{"Players", {"Players"}},
		{"Tasks", {"Complete All Tasks", "Play Medbay Scan Animation"}},
		{"Sabotage", {"Disable Sabotage", "Auto Repair Sabotages", "Repair Sabotage", "Sabotage All", "Random Sabotage", "Sabotage Lights", "Sabotage Reactor", "Sabotage Oxygen", "Sabotage Comms", "Disable Lights",
					  "Activate Mushroom Mixup"}},
		{"Doors", {"Close All Doors", "Close Room Door", "Pin All Doors", "Unpin All Doors", "Auto Open Doors"}},
		{"Host", {"Custom Impostor Amount", "Impostor Count", "Force Start of Game", "Disable Meetings", "Disable Sabotages", "Disable Game Ending", "End Game", "Force Color for Everyone",
				  "Force Name for Everyone", "Spam Moving Platform", "Unlock Kill Button", "Allow Killing in Lobbies", "Kill While Vanished", "Game Mode", "Show Lobby Timer", "Auto Start Game", "Spectator Mode"}},
				  {"IRL update", {"BRAINWASH", "SEX", "FUCK", "get gud", "free knife", "PAINKILLER", "get doctor degree", "Become Diddy"}},
#ifdef _DEBUG
		{"Debug", {"Enable Occlusion Culling", "Force Load Settings", "Force Save Settings", "Clear RPC Queues", "Log Unity Debug Messages", "Log Hook Debug Messages", "Colors", "Profiler",
				   "Experiments", "Enable Anticheat (SMAC)", "Point System (Only for Hosting)", "April fools mode", "Fuckson Mode", "FuckHudson Mode"}},
#endif
				   // Add more settings here as needed
	};

	void CloseAllOtherTabs(Tabs openTab) {
		openAbout = openTab == Tabs::About;
		openSettings = openTab == Tabs::Settings;
		openGame = openTab == Tabs::Game;
		openSelf = openTab == Tabs::Self;
		openRadar = openTab == Tabs::Radar;
		openReplay = openTab == Tabs::Replay;
		openEsp = openTab == Tabs::Esp;
		openPlayers = openTab == Tabs::Players;
		openTasks = openTab == Tabs::Tasks;
		openSabotage = openTab == Tabs::Sabotage;
		openDoors = openTab == Tabs::Doors;
		openHost = openTab == Tabs::Host;
#ifdef _DEBUG
		openDebug = openTab == Tabs::Debug;
#endif
	}

	void Init() {
		ImGui::SetNextWindowSize(ImVec2(600, 450) * State.dpiScale, ImGuiCond_None);
		ImGui::SetNextWindowBgAlpha(State.MenuThemeColor.w);
	}

	bool init = false;
	bool firstRender = true;
	bool isPanicWarning = false;

	std::string ToLower(const std::string& str) {
		std::string lowerStr = str;
		std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
		return lowerStr;
	}

	static std::string StrRev(std::string str) {
		std::string new_str = str;
		std::reverse(new_str.begin(), new_str.end());
		return new_str;
	}

	void RenderSearchResults() {
		if (searchQuery.size() == 0) return;

		std::string lowerQuery = ToLower(searchQuery);

		std::vector<std::string> searchResults = {};

		for (const auto& category : categories) {
			for (const auto& setting : category.second) {
				if (ToLower(setting).find(lowerQuery) != std::string::npos) {
					searchResults.push_back(category.first);
					break;
				}
			}
		}
		ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 0.f), "space");
		if (searchResults.size() == 0) BoldText("No results.");
		else {
			BoldText(("Search Result" + std::string(searchResults.size() == 1 ? "" : "s")).c_str());
			for (std::string i : searchResults) {
				ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.f), i.c_str());
			}
		}
	}

	void Render() {
		try {
			if (!init)
				Menu::Init();
			std::string modText = std::format("SickoMenu {}", State.SickoVersion);
			ImGui::Begin("SickoMenu", &State.ShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse);
			static ImVec4 titleCol = State.MenuThemeColor;
			if (State.RgbMenuTheme)
				titleCol = State.RgbColor;
			else
				titleCol = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
			titleCol.w = 1.f;
			ImGui::TextColored(titleCol, modText.c_str());
			ImVec4 DiddyCol = ImVec4(0.79f, 0.03f, 1.f, 1.f);
			if (State.AprilFoolsMode) {
				ImGui::SameLine(0.f, 0.f);
				if (State.DiddyPartyMode) ImGui::TextColored(DiddyCol, " [Diddy Party Mode]");
				else ImGui::TextColored(DiddyCol, IsChatCensored() || IsStreamerMode() ? " [F***son Mode]" : " [Fuckson Mode]");
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - 19 * State.dpiScale);
			if (AnimatedButton("-")) State.ShowMenu = false; //minimize button
			//ImGui::BeginTabBar("AmongUs#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);
			ImGui::BeginChild("###SickoMenu", ImVec2(90 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
			// Search field
			ImGui::SetNextItemWidth(90 * State.dpiScale); // Adjust the width of the input box
			if (InputStringWithHint("##Search", "Search...", &searchQuery) && State.AprilFoolsMode) {
				if (ToLower(searchQuery) == StrRev("nosduh")) {
					State.AprilFoolsMode = !State.AprilFoolsMode;
					if (!State.AprilFoolsMode) State.DiddyPartyMode = false;
				}
				if (ToLower(searchQuery) == StrRev("yddid")) {
					State.DiddyPartyMode = !State.DiddyPartyMode;
				}
			}
			if (ImGui::Selectable("About", openAbout)) {
				CloseAllOtherTabs(Tabs::About);
			}
			if (ImGui::Selectable("Settings", openSettings)) {
				CloseAllOtherTabs(Tabs::Settings);
			}
			if (ImGui::Selectable("Game", openGame)) {
				CloseAllOtherTabs(Tabs::Game);
			}
			if (ImGui::Selectable("Self", openSelf)) {
				CloseAllOtherTabs(Tabs::Self);
			}
			if (ImGui::Selectable("Radar", openRadar)) {
				CloseAllOtherTabs(Tabs::Radar);
			}
			if (ImGui::Selectable("Replay", openReplay)) {
				CloseAllOtherTabs(Tabs::Replay);
			}
			if (ImGui::Selectable("ESP", openEsp)) {
				CloseAllOtherTabs(Tabs::Esp);
			}
			if ((IsInGame() || IsInLobby()) && ImGui::Selectable("Players", openPlayers)) {
				CloseAllOtherTabs(Tabs::Players);
			}
			if ((IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.Tasks != NULL) && ImGui::Selectable("Tasks", openTasks)) {
				CloseAllOtherTabs(Tabs::Tasks);
			}
			if (IsInGame() && ShipStatus__TypeInfo->static_fields->Instance != NULL && ImGui::Selectable("Sabotage", openSabotage)) {
				CloseAllOtherTabs(Tabs::Sabotage);
			}
			if ((IsInGame() && !State.mapDoors.empty()) && ImGui::Selectable("Doors", openDoors)) {
				CloseAllOtherTabs(Tabs::Doors);
			}
			if (IsHost() && ImGui::Selectable("Host", openHost)) {
				CloseAllOtherTabs(Tabs::Host);
			}
#ifdef _DEBUG
			if (State.showDebugTab && ImGui::Selectable("Debug", openDebug)) {
				CloseAllOtherTabs(Tabs::Debug);
			}
#endif
			RenderSearchResults();

			ImVec4 PanicCol = ImVec4(1.f, 0.f, 0.f, 1.f);
			ImVec4 GreenCol = ImVec4(0.f, 1.f, 0.f, 1.f);
			if (!isPanicWarning) {
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 90 * State.dpiScale, ImGui::GetWindowHeight() - 20 * State.dpiScale));
				if (!State.AprilFoolsMode && ColoredButton(PanicCol, "Disable Menu")) {
					isPanicWarning = State.PanicWarning;
					if (!State.PanicWarning) State.PanicMode = true;
				}
				if (State.AprilFoolsMode && ColoredButton(DiddyCol,State.DiddyPartyMode ? "Rizz Up Diddy" : 
						StrRev(std::format("nosduH {}F", IsChatCensored() || IsStreamerMode() ? "***" : "kcu")).c_str())) {
					isPanicWarning = State.PanicWarning;
					if (!State.PanicWarning) State.PanicMode = true;
				}
			}
			else {
				bool panicKeybind = State.KeyBinds.Toggle_Sicko != 0x00;
				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 90 * State.dpiScale,
					ImGui::GetWindowHeight() - 65 * State.dpiScale));
				if (!panicKeybind) {
					ImGui::TextColored(PanicCol, "No Panic");
					ImGui::TextColored(PanicCol, "Keybind!");
				}
				else {
					ImGui::TextColored(PanicCol, ("Press " + (std::string)KeyBinds::ToString(State.KeyBinds.Toggle_Sicko)).c_str());
					ImGui::TextColored(PanicCol, ("to re-enable!"));
				}
				ImGui::TextColored(PanicCol, "Continue?");
				if (ColoredButton(PanicCol, "Yes")) {
					isPanicWarning = false;
					State.PanicMode = true;
				}
				ImGui::SameLine();
				if (ColoredButton(GreenCol, "No")) {
					isPanicWarning = false;
				}
			}

			if (firstRender) {
				firstRender = false;
				CloseAllOtherTabs(Tabs::About); //welcome the user on startup
			}
			//ImGui::EndTabBar();
			ImGui::EndChild();

			//open respective tabs
			if (openAbout) AboutTab::Render();
			else {
				if (!State.HasOpenedMenuBefore) State.HasOpenedMenuBefore = true;
			}
			if (openSettings) SettingsTab::Render();
			if (openGame) GameTab::Render();
			if (openSelf) SelfTab::Render();
			if (openRadar) RadarTab::Render();
			if (openReplay) ReplayTab::Render();
			if (openEsp) EspTab::Render();
			if (openPlayers) {
				if (IsInGame() || IsInLobby()) PlayersTab::Render();
				else {
					CloseAllOtherTabs(Tabs::Game);
					GameTab::Render();
				}
			}
			if (openTasks) {
				if (IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.Tasks != NULL) TasksTab::Render();
				else {
					CloseAllOtherTabs(Tabs::Game);
					GameTab::Render();
				}
			}
			if (openSabotage) {
				if (IsInGame()) SabotageTab::Render();
				else {
					CloseAllOtherTabs(Tabs::Game);
					GameTab::Render();
				}
			}
			if (openDoors) {
				if (IsInGame() && !State.mapDoors.empty()) DoorsTab::Render();
				else {
					CloseAllOtherTabs(Tabs::Game);
					GameTab::Render();
				}
			}
			if (openHost) {
				if (IsHost()) HostTab::Render();
				else {
					CloseAllOtherTabs(Tabs::Game);
					GameTab::Render();
				}
			}
#ifdef _DEBUG
			if (openDebug) {
				if (State.showDebugTab) DebugTab::Render();
				else {
					CloseAllOtherTabs(Tabs::Game);
					GameTab::Render();
				}
			}
#endif

			ImGui::End();
		}
		catch (...) {
			LOG_ERROR("Exception occurred when rendering menu");
		}
	}
}
