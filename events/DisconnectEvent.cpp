#include "pch-il2cpp.h"
#include "_events.h"
#include "utility.h"
#include "translations.hpp"

DisconnectEvent::DisconnectEvent(const EVENT_PLAYER& source) : EventInterface(source, EVENT_TYPES::EVENT_DISCONNECT) { }

void DisconnectEvent::Output() {
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(source.colorId)), source.playerName.c_str());
	ImGui::SameLine();
	ImGui::Text(T_("has left the game"));
	ImGui::SameLine();
	ImGui::Text(T_("[%s ago]"), std::format("{:%OM:%OS}", (std::chrono::system_clock::now() - this->timestamp)).c_str());
}

void DisconnectEvent::ColoredEventOutput() {
	ImGui::Text("[");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), T_("DISCONNECT"));
	ImGui::SameLine();
	ImGui::Text("]");
}