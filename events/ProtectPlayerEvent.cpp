#include "pch-il2cpp.h"
#include "_events.h"
#include "utility.h"
#include "translations.hpp"

ProtectPlayerEvent::ProtectPlayerEvent(const EVENT_PLAYER& source, const EVENT_PLAYER& target) : EventInterface(source, EVENT_TYPES::EVENT_PROTECTPLAYER) {
	this->target = target;
}

void ProtectPlayerEvent::Output() {
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(source.colorId)), source.playerName.c_str());
	ImGui::SameLine();
	ImGui::Text(">");
	ImGui::SameLine();
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(target.colorId)), target.playerName.c_str());
	ImGui::SameLine();
	ImGui::Text(T_("[%s ago]"), std::format("{:%OM:%OS}", (std::chrono::system_clock::now() - this->timestamp)).c_str());
}

void ProtectPlayerEvent::ColoredEventOutput() {
	ImGui::Text("[");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.1f, 0.75f, 0.75f, 1.f), T_("PROTECT"));
	ImGui::SameLine();
	ImGui::Text("]");
}