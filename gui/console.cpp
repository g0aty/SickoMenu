#include "pch-il2cpp.h"
#include "console.hpp"
#include "imgui/imgui.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "logger.h"

namespace ConsoleGui
{
	std::vector<std::pair<const char*, bool>> event_filter =
	{
#define ADD_EVENT(name, desc) {desc, false}
		ALL_EVENTS
#undef ADD_EVENT
	};

	std::array<std::pair<PlayerSelection, bool>, Game::MAX_PLAYERS> player_filter;

	bool init = false;
	void Init() {
		ImGui::SetNextWindowSize(ImVec2(520, 320) * State.dpiScale, ImGuiCond_None);
		ImGui::SetNextWindowBgAlpha(State.MenuThemeColor.w);

		if (!init)
		{
			for (auto it = event_filter.begin(); it != event_filter.end(); it++) {
				// Exclude the following events
				switch (static_cast<EVENT_TYPES>(it - event_filter.begin())) {
				case EVENT_TYPES::EVENT_WALK:
					it->first = "";
					break;
				}
			}
			init = true;
		}
	}


	void Render() {
		ConsoleGui::Init();
		ImGui::Begin("###Console", &State.ShowConsole, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
		static ImVec4 titleCol = State.MenuThemeColor;
		if (State.RgbMenuTheme)
			titleCol = State.RgbColor;
		else
			titleCol = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
		titleCol.w = 1.f;
		ImGui::TextColored(titleCol, "Console");
		ImGui::SameLine(ImGui::GetWindowWidth() - 20 * State.dpiScale);
		if (AnimatedButton("-")) State.ShowConsole = false; //minimize button
		ImGui::BeginChild("console#filter", ImVec2(520, 40) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
		ImGui::Text("Event Filter: ");
		ImGui::SameLine();
		CustomListBoxIntMultiple("Event Types", &ConsoleGui::event_filter, 100.f * State.dpiScale);
		if (IsInGame()) {
			ImGui::SameLine(0.f * State.dpiScale, 5.f * State.dpiScale);
			ImGui::Text("Player Filter: ");
			ImGui::SameLine();
			CustomListBoxPlayerSelectionMultiple("Players", &ConsoleGui::player_filter, 150.f * State.dpiScale);
		}
		if (AnimatedButton("Clear Console")) {
			synchronized(Replay::replayEventMutex) {
				State.liveConsoleEvents.clear();
			}
		}
		ImGui::EndChild();
		ImGui::Separator();
		ImGui::BeginChild("console#scroll", ImVec2(511, 270) * State.dpiScale, true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoBackground);

		// pre-processing of filters
		bool isUsingEventFilter = false, isUsingPlayerFilter = false;
		for (const auto& pair : ConsoleGui::event_filter) {
			if (pair.second) {
				isUsingEventFilter = true;
				break;
			}
		}
		for (auto& pair : ConsoleGui::player_filter) {
			if (pair.second && pair.first.has_value()) {
				isUsingPlayerFilter = true;
				break;
			}
		}

		synchronized(Replay::replayEventMutex) {
			size_t i = State.liveConsoleEvents.size() - 1;
			for (auto rit = State.liveConsoleEvents.rbegin(); rit != State.liveConsoleEvents.rend(); ++rit, --i) {
				EventInterface* evt = (*rit).get();
				if (evt == NULL)
				{
					STREAM_ERROR("State.liveConsoleEvents[" << i << "] was NULL (liveConsoleEvents.size(): " << State.liveConsoleEvents.size() << ")");
					continue;
				}
				if (evt->getType() == EVENT_TYPES::EVENT_WALK)
					continue;

				if (isUsingEventFilter && ConsoleGui::event_filter.at((size_t)evt->getType()).second == false)
					continue;
				if (isUsingPlayerFilter) {
					if (evt->getSource().playerId < 0 || evt->getSource().playerId >= ConsoleGui::player_filter.size())
						continue;
					auto& p = ConsoleGui::player_filter.at(evt->getSource().playerId);
					if (p.second == false)
						continue;
					if (!p.first.has_value())
						continue;
				}

				evt->ColoredEventOutput();
				ImGui::SameLine();
				evt->Output();
			}
		}
		ImGui::EndChild();
		ImGui::End();
	}
}