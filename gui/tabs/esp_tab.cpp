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
		changed |= ToggleButton("Enable", &State.ShowEsp);

		changed |= ToggleButton("Show Ghosts", &State.ShowEsp_Ghosts);
		//dead bodies for v3.1
		changed |= ToggleButton("Hide During Meetings", &State.HideEsp_During_Meetings);

		changed |= ToggleButton("Show Boxes", &State.ShowEsp_Box);
		changed |= ToggleButton("Show Tracers", &State.ShowEsp_Tracers);
		changed |= ToggleButton("Show Distances", &State.ShowEsp_Distance);
		//better esp (from noobuild) coming v3.1
		changed |= ToggleButton("Role-based", &State.ShowEsp_RoleBased);

		if (State.ShowEsp_RoleBased) {
			ImGui::SameLine();
			changed |= ToggleButton("Crewmates", &State.ShowEsp_Crew);
			ImGui::SameLine();
			changed |= ToggleButton("Impostors", &State.ShowEsp_Imp);
		}

		ImGui::EndChild();
		if (changed) {
			State.Save();
		}
	}
}