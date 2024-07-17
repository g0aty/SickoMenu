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
		ImGui::SetNextWindowSize(ImVec2(600, 400) * State.dpiScale, ImGuiCond_None);
		ImGui::SetNextWindowBgAlpha(State.MenuThemeColor.w);
	}

	bool init = false;
	bool firstRender = true;
	void Render() {
		State.RgbNameColor += 0.025f;
		constexpr auto tau = 2.f * 3.14159265358979323846f;
		while (State.RgbNameColor > tau) State.RgbNameColor -= tau;
		const auto calculate = [](float value) {return std::sin(value) * .5f + .5f; };
		auto color_r = calculate(State.RgbNameColor + 0.f);
		auto color_g = calculate(State.RgbNameColor + 4.f);
		auto color_b = calculate(State.RgbNameColor + 2.f);
		State.rgbCode = std::format("<#{:02x}{:02x}{:02x}>", int(color_r * 255), int(color_g * 255), int(color_b * 255));

		if (State.RgbMenuTheme) {
			State.RgbColor.x = color_r;
			State.RgbColor.y = color_g;
			State.RgbColor.z = color_b;
		}

		static uint8_t gradientStep = 1;
		static bool gradientIncreasing = true;
		if (gradientStep == 1) {
			gradientStep++;
			gradientIncreasing = true;
		}
		else if (gradientStep == 100) {
			gradientStep--;
			gradientIncreasing = false;
		}
		else {
			if (gradientIncreasing) gradientStep++;
			else gradientStep--;
		}

		if (State.GradientMenuTheme) {
			float stepR = float((State.MenuGradientColor2.x - State.MenuGradientColor1.x) / 100);
			float stepG = float((State.MenuGradientColor2.y - State.MenuGradientColor1.y) / 100);
			float stepB = float((State.MenuGradientColor2.z - State.MenuGradientColor1.z) / 100);
			State.MenuGradientColor = ImVec4(State.MenuGradientColor1.x + stepR * gradientStep, 
				State.MenuGradientColor1.y + stepG * gradientStep,
				State.MenuGradientColor1.z + stepB * gradientStep,
				State.MenuThemeColor.w);
		}

		try {
			if (!init)
				Menu::Init();
			std::string modText = "SickoMenu " + State.SickoVersion;
			ImGui::Begin(const_cast<char*>(modText.c_str()), &State.ShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
			static ImVec4 titleCol = State.MenuThemeColor;
			if (State.RgbMenuTheme)
				titleCol = State.RgbColor;
			else
				titleCol = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
			titleCol.w = 1.f;
			ImGui::TextColored(titleCol, const_cast<char*>(modText.c_str()));
			ImGui::SameLine(ImGui::GetWindowWidth() - 20 * State.dpiScale);
			if (ImGui::Button("-")) State.ShowMenu = false; //minimize button
			//ImGui::BeginTabBar("AmongUs#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);
			ImGui::BeginChild("###SickoMenu", ImVec2(90 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
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
			if (IsInGame() && ImGui::Selectable("Sabotage", openSabotage)) {
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