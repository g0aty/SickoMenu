#include "pch-il2cpp.h"
#include "_events.h"
#include "utility.h"

SabotageEvent::SabotageEvent(const EVENT_PLAYER& source, SystemTypes__Enum systemType, SABOTAGE_ACTIONS action) : EventInterface(source, EVENT_TYPES::EVENT_SABOTAGE) {
	this->systemType = systemType;
	this->action = action;
}

void SabotageEvent::Output() {
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(source.colorId)), source.playerName.c_str());
	if (IsColorBlindMode()) {
		ImGui::SameLine();
		ImGui::Text(("(" + GetColorName(source.colorId) + ")").c_str());
	}
	ImGui::SameLine();
	ImGui::Text("(%s)", TranslateSystemTypes(systemType));
	ImGui::SameLine();
	ImGui::Text("[%s ago]", std::format("{:%OM:%OS}", (std::chrono::system_clock::now() - this->timestamp)).c_str());
}

void SabotageEvent::ColoredEventOutput() {
	ImGui::Text("[");
	ImGui::SameLine();

	ImVec4 color;
	((action == SABOTAGE_ACTIONS::SABOTAGE_FIX) ? color = ImVec4(0.f, 1.f, 0.f, 1.f) : color = ImVec4(1.f, 0.f, 0.f, 1.f));

	ImGui::TextColored(color, ((action == SABOTAGE_ACTIONS::SABOTAGE_CALL) ? "SABOTAGE" : "REPAIR"));
	ImGui::SameLine();
	ImGui::Text("]");
}