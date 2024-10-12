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
				if (systemType == SystemTypes__Enum::Decontamination
					|| systemType == SystemTypes__Enum::Decontamination2
					|| systemType == SystemTypes__Enum::Decontamination3) {
					continue;
				}
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
				if (!(std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), systemType) == State.pinnedDoors.end()))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, { 1.f, 0.f, 0.f, 1.f });
					if (ImGui::Selectable(TranslateSystemTypes(systemType), State.selectedDoor == systemType))
						State.selectedDoor = systemType;
					ImGui::PopStyleColor(1);
				}
				else if (!isOpen)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, State.RgbMenuTheme ? State.RgbColor : State.MenuThemeColor);
					if (ImGui::Selectable(TranslateSystemTypes(systemType), State.selectedDoor == systemType))
						State.selectedDoor = systemType;
					ImGui::PopStyleColor(1);
				}
				else
				{
					if (ImGui::Selectable(TranslateSystemTypes(systemType), State.selectedDoor == systemType))
						State.selectedDoor = systemType;
				}
			}
			if (shouldEndListBox)
				ImGui::ListBoxFooter();
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::BeginChild("doors#options", ImVec2(300, 0) * State.dpiScale, false, ImGuiWindowFlags_NoBackground);

			if (State.DisableSabotages) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sabotages have been disabled.");
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Nothing can be sabotaged.");
			}

			if (ImGui::Button("Close All Doors"))
			{
				for (auto door : State.mapDoors)
				{
					State.rpcQueue.push(new RpcCloseDoorsOfType(door, false));
				}
			}
			if (State.ShowKeybinds)
				ImGui::SameLine();

			if (ImGui::Button("Close Room Door"))
			{
				State.rpcQueue.push(new RpcCloseDoorsOfType(GetSystemTypes(GetTrueAdjustedPosition(*Game::pLocalPlayer)), false));
			}
			if (State.ShowKeybinds)
				ImGui::SameLine();

			if (ImGui::Button("Pin All Doors"))
			{
				for (auto door : State.mapDoors)
				{
					if (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), door) == State.pinnedDoors.end())
					{
						if (door != SystemTypes__Enum::Decontamination && door != SystemTypes__Enum::Decontamination2 && door != SystemTypes__Enum::Decontamination3)
							State.rpcQueue.push(new RpcCloseDoorsOfType(door, true));
					}
				}
			}
			if (ImGui::Button("Unpin All Doors"))
			{
				State.pinnedDoors.clear();
			}
			ImGui::NewLine();
			if (State.selectedDoor != SystemTypes__Enum::Hallway) {
				auto plainDoor = GetPlainDoorByRoom(State.selectedDoor);

				if (ImGui::Button("Close Door")) {
					State.rpcQueue.push(new RpcCloseDoorsOfType(State.selectedDoor, false));
				}

				if (std::find(State.pinnedDoors.begin(), State.pinnedDoors.end(), State.selectedDoor) == State.pinnedDoors.end()) {
					if (ImGui::Button("Pin Door")) {
						State.rpcQueue.push(new RpcCloseDoorsOfType(State.selectedDoor, true));
					}
				}
				else {
					if (ImGui::Button("Unpin Door")) {
						State.pinnedDoors.erase(std::remove(State.pinnedDoors.begin(), State.pinnedDoors.end(), State.selectedDoor), State.pinnedDoors.end());
					}
				}
			}
			if (State.mapType == Settings::MapType::Pb || State.mapType == Settings::MapType::Airship || State.mapType == Settings::MapType::Fungle)
			{
				ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
				if (ToggleButton("Auto Open Doors", &State.AutoOpenDoors)) {
					State.Save();
				}
			}
			ImGui::EndChild();
		}
	}
}