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
	// static std::string searchQuery = (std::string)"";

	struct SearchEntry {
		std::string Name;
		std::string SubGroup; // empty if the tab has no sub-groups or setting isn't mapped
	};

	std::map<std::string, std::vector<SearchEntry>> categories = {
		{"Settings", {
			{"Show Keybinds", "Keybinds"}, {"Allow Activating Keybinds while Chatting", "Keybinds"},
			{"Always Show Menu on Startup", "General"}, {"Panic (Disable SickoMenu)", "General"},
			{"Config Name", "General"}, {"Load Config", "General"}, {"Save Config", "General"},
			{"Adjust by DPI", "Customization"}, {"Menu Scale", "Customization"}, {"Menu Theme Color", "Customization"},
			{"Gradient Theme", "Customization"}, {"Match Background with Theme", "Customization"},
			{"RGB Menu Theme", "Customization"}, {"Reset Menu Theme", "Customization"}, {"Opacity", "Customization"},
			{"Show Debug Tab", "General"}, {"Username", "Spoofing"}, {"Set as Account Name", "Spoofing"},
			{"Automatically Set Name", "Spoofing"}, {"Custom Code", "Customization"},
			{"Replace Streamer Mode Lobby Code", "Customization"}, {"RGB Lobby Code", "Customization"},
			{"Unlock Cosmetics", "Spoofing"}, {"Safe Mode", "General"},
			{"Allow other SickoMenu users to see you're using SickoMenu", "General"},
			{"Spoof Guest Account", "Spoofing"}, {"Use Custom Guest Friend Code", "Spoofing"},
			{"Spoof Level", "Spoofing"}, {"Spoof Platform", "Spoofing"},
			{"Disable Host Anticheat (+25 Mode)", "General"}, {"FPS", "General"}
		}},
		{"Game", {
			{"Player Speed Multiplier", "General"}, {"Kill Distance", "General"}, {"No Ability Cooldown", "General"},
			{"Multiply Speed", "General"}, {"Modify Kill Distance", "General"}, {"Random Color", "General"},
			{"Set Color", "General"}, {"Snipe Color", "General"}, {"Console", "General"},
			{"Reset Appearance", "General"}, {"Kill Everyone", "General"}, {"Protect Everyone", "General"},
			{"Disable Venting", "General"}, {"Spam Report", "General"}, {"Kill All Crewmates", "General"},
			{"Kill All Impostors", "General"}, {"Kick Everyone From Vents", "General"},
			{"Chat Message", "Chat"}, {"Send", "Chat"}, {"Send to AUM", "Chat"}, {"Spam", "Chat"},
			{"Chat Presets", "Chat"}, {"Attempt to Crash", "Utils"}, {"Overload Everyone", "Utils"},
			{"Lag Everyone", "Utils"}, {"Enable Anticheat (SMAC)", "Anticheat"},
			{"Whitelist", "Anticheat"}, {"Blacklist", "Anticheat"}
		}},
		{"Self", {
			{"Max Vision", "Visuals"}, {"Wallhack", "Visuals"}, {"Disable HUD", "Visuals"}, {"Freecam", "Visuals"},
			{"Zoom", "Visuals"}, {"Always show Chat Button", "Visuals"}, {"Allow Ctrl+(C/V) in Chat", "Visuals"},
			{"Read Messages by Ghosts", "Visuals"}, {"Read and Send SickoChat", "Visuals"}, {"Custom Name", "Visuals"},
			{"Custom Name for Everyone", "Visuals"}, {"Server-sided Custom Name", "Visuals"}, {"Reveal Roles", "Visuals"},
			{"Abbrv. Role", "Visuals"}, {"Player Colored Dots Next To Names", "Visuals"}, {"Show Player Info in Lobby", "Visuals"},
			{"Reveal Votes", "Visuals"}, {"See Ghosts", "Visuals"}, {"See Protections", "Visuals"}, {"See Kill Cooldown", "Visuals"},
			{"Disable Kill Animation", "Visuals"}, {"Dark Mode", "Visuals"}, {"Show Host", "Visuals"}, {"Hide Watermark", "Visuals"},
			{"Show Vote Kicks", "Visuals"}, {"Show FPS", "Visuals"}, {"Show Lobby Info", "Visuals"}, {"See Phantoms", "Visuals"},
			{"Unlock Vents", "Utils"}, {"Move While in Vent & Shapeshifting", "Utils"}, {"Always Move", "Utils"},
			{"No Shapeshift Animation", "Utils"}, {"Copy Lobby Code on Disconnect", "Utils"}, {"NoClip", "Utils"},
			{"Allow Killing in Lobbies", "Utils"}, {"Kill Other Impostors", "Utils"}, {"Infinite Kill Range", "Utils"},
			{"Bypass Guardian Angel Protections", "Utils"}, {"Autokill", "Utils"}, {"Do Tasks as Impostor", "Utils"},
			{"Fake Alive", "Utils"}, {"God Mode", "Utils"}, {"Teleport", "Utils"}, {"Rotate everyone", "Utils"},
			{"Select Role", "Utils"}, {"Set Role", "Utils"}, {"Set Fake Role", "Utils"}, {"Automatically Set Fake Role", "Utils"},
			{"Report Body on Murder", "Utils"}, {"Prevent Self-Report", "Utils"},
			{"Cycler", "Randomizers"}, {"Cycle in Meeting", "Randomizers"}, {"Cycle Between Players", "Randomizers"},
			{"Confuser (Randomize Appearance at Will", "Randomizers"}
		}},
		{"Radar", {
			{"Show Radar", ""}, {"Show Dead Bodies", ""}, {"Show Ghosts", ""}, {"Right Click to Teleport", ""},
			{"Hide Radar During Meetings", ""}, {"Draw Player Icons", ""}, {"Lock Radar Position", ""}, {"Show Border", ""}
		}},
		{"Replay", {
			{"Show Replay", ""}, {"Show Only last seconds", ""}, {"Clear after meeting", ""}
		}},
		{"ESP", {
			{"Enable", ""}, {"Show Ghosts", ""}, {"Hide During Meetings", ""}, {"Show Boxes", ""},
			{"Show Tracers", ""}, {"Show Distances", ""}, {"Role-based", ""}
		}},
		{"Players", {
			{"Players", ""}
		}},
		{"Tasks", {
			{"Complete All Tasks", ""}, {"Play Medbay Scan Animation", ""}
		}},
		{"Sabotage", {
			{"Disable Sabotage", ""}, {"Auto Repair Sabotages", ""}, {"Repair Sabotage", ""}, {"Sabotage All", ""},
			{"Random Sabotage", ""}, {"Sabotage Lights", ""}, {"Sabotage Reactor", ""}, {"Sabotage Oxygen", ""},
			{"Sabotage Comms", ""}, {"Disable Lights", ""}, {"Activate Mushroom Mixup", ""}
		}},
		{"Doors", {
			{"Close All Doors", ""}, {"Close Room Door", ""}, {"Pin All Doors", ""}, {"Unpin All Doors", ""}, {"Auto Open Doors", ""}
		}},
		{"Host", {
			{"Custom Impostor Amount", "Utils"}, {"Impostor Count", "Utils"}, {"Force Start of Game", "Utils"},
			{"Disable Meetings", "Utils"}, {"Disable Sabotages", "Utils"}, {"Disable Game Ending", "Utils"}, {"End Game", "Utils"},
			{"Force Color for Everyone", "Utils"}, {"Force Name for Everyone", "Utils"}, {"Spam Moving Platform", "Utils"},
			{"Unlock Kill Button", "Utils"}, {"Allow Killing in Lobbies", "Utils"}, {"Kill While Vanished", "Utils"},
			{"Game Mode", "Utils"}, {"Show Lobby Timer", "Utils"}, {"Auto Start Game", "Utils"}, {"Spectator Mode", "Utils"}
		}},
#ifdef _DEBUG
		{"Debug", {
			{"Enable Occlusion Culling", ""}, {"Force Load Settings", ""}, {"Force Save Settings", ""}, {"Clear RPC Queues", ""},
			{"Log Unity Debug Messages", ""}, {"Log Hook Debug Messages", ""}, {"Colors", ""}, {"Profiler", ""},
			{"Experiments", ""}, {"Enable Anticheat (SMAC)", ""}, {"Point System (Only for Hosting)", ""}
		}},
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

	Tabs CategoryNameToTab(const std::string& name) {
		if (name == "Settings") return Tabs::Settings;
		if (name == "Game") return Tabs::Game;
		if (name == "Self") return Tabs::Self;
		if (name == "Radar") return Tabs::Radar;
		if (name == "Replay") return Tabs::Replay;
		if (name == "ESP") return Tabs::Esp;
		if (name == "Players") return Tabs::Players;
		if (name == "Tasks") return Tabs::Tasks;
		if (name == "Sabotage") return Tabs::Sabotage;
		if (name == "Doors") return Tabs::Doors;
		if (name == "Host") return Tabs::Host;
#ifdef _DEBUG
		if (name == "Debug") return Tabs::Debug;
#endif
		return Tabs::About;
	}

	void OpenTabSubGroup(const std::string& tabName, const std::string& subGroup) {
		if (subGroup.empty()) return;
		if (tabName == "Self") SelfTab::OpenSubGroup(subGroup);
		else if (tabName == "Game") GameTab::OpenSubGroup(subGroup);
		else if (tabName == "Host") HostTab::OpenSubGroup(subGroup);
		else if (tabName == "Settings") SettingsTab::OpenSubGroup(subGroup);
	}

	void RenderSearchResults() {
		if (State.searchQuery.size() == 0) return;

		std::string lowerQuery = ToLower(State.searchQuery);

		// category name -> first matching sub-group found for it
		std::vector<std::pair<std::string, std::string>> searchResults = {};

		for (const auto& category : categories) {
			for (const auto& entry : category.second) {
				if (ToLower(entry.Name).find(lowerQuery) != std::string::npos) {
					searchResults.push_back({ category.first, entry.SubGroup });
					break;
				}
			}
		}
		ImGui::TextColored(ImVec4(0.f, 0.f, 0.f, 0.f), "space");
		if (searchResults.size() == 0) BoldText("No results.");
		else {
			BoldText(("Search Result" + std::string(searchResults.size() == 1 ? "" : "s")).c_str());
			for (auto& [tabName, subGroup] : searchResults) {
				std::string label = subGroup.empty() ? tabName : (tabName + " > " + subGroup);
				if (ImGui::Selectable((label + "##searchresult").c_str())) {
					CloseAllOtherTabs(CategoryNameToTab(tabName));
					OpenTabSubGroup(tabName, subGroup);
					State.searchQuery = "";
				}
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
			/*if (State.AprilFoolsMode) {
				ImGui::SameLine(0.f, 0.f);
				if (State.DiddyPartyMode) ImGui::TextColored(DiddyCol, " [Diddy Party Mode]");
				else ImGui::TextColored(DiddyCol, IsChatCensored() || IsStreamerMode() ? " [F***son Mode]" : " [Fuckson Mode]");
			}*/
			ImGui::SameLine(ImGui::GetWindowWidth() - 19 * State.dpiScale);
			if (AnimatedButton("-")) State.ShowMenu = false; //minimize button
			//ImGui::BeginTabBar("AmongUs#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);
			ImGui::BeginChild("###SickoMenu", ImVec2(90 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
			// Search field
			ImGui::SetNextItemWidth(90 * State.dpiScale); // Adjust the width of the input box
			if (InputStringWithHint("##Search", "Search...", &State.searchQuery)/* && State.AprilFoolsMode*/) {
				/*if (ToLower(searchQuery) == StrRev("nosduh")) {
					State.AprilFoolsMode = !State.AprilFoolsMode;
					if (!State.AprilFoolsMode) State.DiddyPartyMode = false;
				}
				if (ToLower(searchQuery) == StrRev("yddid")) {
					State.DiddyPartyMode = !State.DiddyPartyMode;
				}*/
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
			if (((IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.Tasks != NULL) || (IsInLobby() && IsHost())) && ImGui::Selectable("Tasks", openTasks)) {
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
				if (/*!State.AprilFoolsMode && */ColoredButton(PanicCol, "Disable Menu")) {
					isPanicWarning = State.PanicWarning;
					if (!State.PanicWarning) State.PanicMode = true;
				}
				/*if (State.AprilFoolsMode && ColoredButton(DiddyCol,State.DiddyPartyMode ? "Rizz Up Diddy" : 
						StrRev(std::format("nosduH {}F", IsChatCensored() || IsStreamerMode() ? "***" : "kcu")).c_str())) {
					isPanicWarning = State.PanicWarning;
					if (!State.PanicWarning) State.PanicMode = true;
				}*/
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
				if ((IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.Tasks != NULL) || (IsInLobby() && IsHost())) TasksTab::Render();
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
