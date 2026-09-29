#include "pch-il2cpp.h"
#include "esp_tab.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"

namespace EspTab {

	void Render() {
		bool changed = false;
		ImGui::SameLine(100 * State.dpiScale);
		ImGui::BeginChild("###ESP", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		changed |= ToggleButton("Show ESP", &State.ShowEsp);

		changed |= ToggleButton("Show Players", &State.ShowEsp_Players);
		changed |= ToggleButton("Show Ghosts", &State.ShowEsp_Ghosts);
		changed |= ToggleButton("Show Dead Bodies", &State.ShowEsp_DeadBodies);

		ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);

		changed |= ToggleButton("Show Tracer & Text Shadows", &State.ShowEsp_LineTextShadows);

		ImGui::SetNextItemWidth(100.f * State.dpiScale);
		if (ImGui::InputFloat("Tracer Thickness", &State.ShowEsp_LineThickness))
			State.ShowEsp_LineThickness = std::clamp(State.ShowEsp_LineThickness, 1.f, 5.f);

		ImGui::SetNextItemWidth(100.f * State.dpiScale);
		if (ImGui::InputFloat("Text Size", &State.ShowEsp_TextSize))
			State.ShowEsp_TextSize = std::clamp(State.ShowEsp_TextSize, 1.f, 1.5f);

		ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);

		changed |= ToggleButton("Hide During Meetings", &State.HideEsp_During_Meetings);

		changed |= ToggleButton("Show Boxes", &State.ShowEsp_Box);
		changed |= ToggleButton("Show Tracers", &State.ShowEsp_Tracers);
		changed |= ToggleButton("Show Distances", &State.ShowEsp_Distance);
		//better esp (from noobuild) coming v3.1
		changed |= ToggleButton("Use Role Colors Instead of Player Colors", &State.ShowEsp_RoleBased);

		changed |= ToggleButton("Show Crewmates", &State.ShowEsp_Crew);
		ImGui::SameLine();
		changed |= ToggleButton("Show Impostors", &State.ShowEsp_Imp);

		ImGui::EndChild();
		if (changed) {
			State.Save();
		}
	}
}