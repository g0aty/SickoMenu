#include "pch-il2cpp.h"
#include "radar_tab.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "utility.h"

namespace RadarTab {
	void Render() {
		ImGui::SameLine(100 * State.dpiScale);
		ImGui::BeginChild("###Radar", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
		if (ToggleButton("Show Radar", &State.ShowRadar)) {
			State.Save();
		}

		ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
		ImGui::Separator();
		ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

		if (ToggleButton("Show Dead Bodies", &State.ShowRadar_DeadBodies)) {
			State.Save();
		}
		if (ToggleButton("Show Ghosts", &State.ShowRadar_Ghosts)) {
			State.Save();
		}
		if (ToggleButton("Right Click to Teleport", &State.ShowRadar_RightClickTP)) {
			State.Save();
		}

		ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
		ImGui::Separator();
		ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

		if (ToggleButton("Hide Radar During Meetings", &State.HideRadar_During_Meetings)) {
			State.Save();
		}
		if (ToggleButton("Draw Player Icons", &State.RadarDrawIcons)) {
			State.Save();
		}
		/*if (State.RadarDrawIcons && State.RevealRoles) {
			ImGui::SameLine();
			if (ToggleButton("Show Role Color on Visor", &State.RadarVisorRoleColor)) {
				State.Save();
			}
		}*/

		if (ToggleButton("Lock Radar Position", &State.LockRadar)) {
			State.Save();
		}
		if (ToggleButton("Show Border", &State.RadarBorder)) {
			State.Save();
		}
		if (ImGui::ColorEdit4("Radar Color",
			(float*)&State.SelectedColor,
			ImGuiColorEditFlags__OptionsDefault
			| ImGuiColorEditFlags_NoInputs
			| ImGuiColorEditFlags_AlphaBar
			| ImGuiColorEditFlags_AlphaPreview)) {
			State.Save();
		}
		if (ImGui::InputInt("Extra Width", &State.RadarExtraWidth)) {
			State.RadarExtraWidth = abs(State.RadarExtraWidth); //prevent negatives
			State.Save();
		}
		if (ImGui::InputInt("Extra Height", &State.RadarExtraHeight)) {
			State.RadarExtraHeight = abs(State.RadarExtraHeight); //prevent negatives
			State.Save();
		}

		ImGui::EndChild();
	}
}