#include "pch-il2cpp.h"
#include "doors_tab.h"
#include "game.h"
#include "gui-helpers.hpp"
#include "imgui/imgui.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"

using namespace std::string_view_literals;

namespace DoorsTab {
	void Render() {
		if (IsInGame() && !State.mapDoors.empty()) {
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::BeginChild("doors#list", ImVec2(200, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
			bool shouldEndListBox = ImGui::ListBoxHeader("###doors#list", ImVec2(200, 150) * State.dpiScale);
			for (auto systemType : State.mapDoors) {
				/*/if (systemType == SystemTypes__Enum::Decontamination
					|| systemType == SystemTypes__Enum::Decontamination2
					|| systemType == SystemTypes__Enum::Decontamination3) {
					continue;
				}*/
				bool isOpen;
				auto openableDoor = GetOpenableDoorByRoom(systemType);
				if ("PlainDoor"sv == openableDoor->klass->parent->name
					|| "PlainDoor"sv == openableDoor->klass->name) {
					isOpen = reinterpret_cast<PlainDoor*>(openableDoor)->fields.Open;
				}
				else if ("MushroomWallDoor"sv == openableDoor->klass->name) {
					isOpen = reinterpret_cast<MushroomWallDoor*>(openableDoor)->fields.open;
				}
				else {
					continue;
				}
				bool isPinned = std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), systemType) != State.pinnedDoors.end();

				bool isSelected = std::find(State.selectedDoors.begin(), State.selectedDoors.end(), systemType) == State.selectedDoors.end();

				ImVec4 selectableColor = isPinned ? ImVec4(1.f, 0.f, 0.f, 1.f) :
					(State.RgbMenuTheme ? State.RgbColor : State.MenuThemeColor);

				if (isPinned || !isOpen) ImGui::PushStyleColor(ImGuiCol_Text, selectableColor);

				if (ImGui::Selectable(TranslateSystemTypes(systemType), !isSelected)) {
					bool isCtrl = ImGui::IsKeyDown(0x11) || ImGui::IsKeyDown(0xA2) || ImGui::IsKeyDown(0xA3);
					bool isShifted = ImGui::IsKeyDown(0x10);

					if (isCtrl) {
						if (isShifted) {
							State.selectedDoors.clear();
						}
						else {
							auto it_sel = std::find(State.selectedDoors.begin(), State.selectedDoors.end(), systemType);
							if (it_sel != State.selectedDoors.end()) {
								State.selectedDoors.erase(it_sel);
							}
							else {
								State.selectedDoors.push_back(systemType);
							}
						}
					}
					else {
						State.selectedDoors = { systemType };
					}
				}

				if (isPinned || !isOpen) ImGui::PopStyleColor(1);
			}
			if (shouldEndListBox)
				ImGui::ListBoxFooter();
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::BeginChild("doors#options", ImVec2(300, 0) * State.dpiScale, false, ImGuiWindowFlags_NoBackground);

			if (IsHost() && State.DisableSabotages) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sabotages have been disabled.");
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Nothing can be sabotaged.");
			}

			if (AnimatedButton("Close All Doors"))
			{
				for (auto door : State.mapDoors)
				{
					State.rpcQueue.push(new RpcCloseDoorsOfType(door, false));
				}
			}

			if (AnimatedButton("Close Room Door"))
			{
				State.rpcQueue.push(new RpcCloseDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer)), false));
			}

			if (State.mapType == Settings::MapType::Pb || State.mapType == Settings::MapType::Airship || State.mapType == Settings::MapType::Fungle) {
				if (AnimatedButton("Open All Doors"))
				{
					for (auto door : State.mapDoors)
					{
						State.rpcQueue.push(new RpcOpenDoorsOfType(door));
					}
				}

				if (AnimatedButton("Open Room Door"))
				{
					State.rpcQueue.push(new RpcOpenDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer))));
				}
			}

			if (AnimatedButton("Pin All Doors"))
			{
				for (auto door : State.mapDoors)
				{
					if (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), door) == State.pinnedDoors.end())
					{
						/*if (door != SystemTypes__Enum::Decontamination && door != SystemTypes__Enum::Decontamination2 && door != SystemTypes__Enum::Decontamination3)
							*/State.rpcQueue.push(new RpcCloseDoorsOfType(door, true));
					}
				}
			}
			if (AnimatedButton("Unpin All Doors"))
			{
				State.pinnedDoors.clear();
			}

			ImGui::NewLine();
			if (!State.selectedDoors.empty()) {
				if (AnimatedButton(State.selectedDoors.size() == 1 ? "Close Door" : "Close Doors")) {
					for (auto door : State.selectedDoors)
						State.rpcQueue.push(new RpcCloseDoorsOfType(door, false));
				}

				if (AnimatedButton(State.selectedDoors.size() == 1 ? "Pin Door" : "Pin Doors")) {
					for (auto door : State.selectedDoors) {
						bool isPinned = std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), door) != State.pinnedDoors.end();
						if (!isPinned) State.rpcQueue.push(new RpcCloseDoorsOfType(door, true));
					}
				}
				if (AnimatedButton(State.selectedDoors.size() == 1 ? "Unpin Door" : "Unpin Doors")) {
					for (auto door : State.selectedDoors) {
						bool isPinned = std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), door) != State.pinnedDoors.end();
						if (isPinned) State.pinnedDoors.erase(std::remove(State.pinnedDoors.begin(), State.pinnedDoors.end(), door), State.pinnedDoors.end());
					}
				}

				if ((State.mapType == Settings::MapType::Pb || State.mapType == Settings::MapType::Airship || State.mapType == Settings::MapType::Fungle) &&
					AnimatedButton(State.selectedDoors.size() == 1 ? "Open Door" : "Open Doors"))
				{
					for (auto door : State.selectedDoors)
						State.rpcQueue.push(new RpcOpenDoorsOfType(door));
				}
			}
			if (State.mapType == Settings::MapType::Pb || State.mapType == Settings::MapType::Airship || State.mapType == Settings::MapType::Fungle)
			{
				ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
				if (ToggleButton("Auto Open Doors on Use", &State.AutoOpenDoors)) State.Save();

				/*if (ToggleButton("Spam Open/Close Doors", &State.SpamDoors)) State.Save();*/
			}
			ImGui::EndChild();
		}
	}
}