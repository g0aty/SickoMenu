#include "pch-il2cpp.h"
#include "_events.h"
#include "utility.h"

ShapeShiftEvent::ShapeShiftEvent(const EVENT_PLAYER& source, const EVENT_PLAYER& target) : EventInterface(source, EVENT_TYPES::EVENT_SHAPESHIFT) {
	this->target = target;
}

void ShapeShiftEvent::Output() {
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(source.colorId)), source.playerName.c_str());
	ImGui::SameLine();
	ImGui::Text(">");
	ImGui::SameLine();
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(target.colorId)), target.playerName.c_str());
	ImGui::SameLine();
	ImGui::Text("[%s ago]", std::format("{:%OM:%OS}", (std::chrono::system_clock::now() - this->timestamp)).c_str());
}

void ShapeShiftEvent::ColoredEventOutput() {
	ImGui::Text("[");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.5f, 0.f, 1.f), "SHAPESHIFT");
	ImGui::SameLine();
	ImGui::Text("]");
}

PhantomEvent::PhantomEvent(const EVENT_PLAYER& source, PHANTOM_ACTIONS action) : EventInterface(source, EVENT_TYPES::EVENT_PHANTOM)
{
	this->action = action;
}

void PhantomEvent::Output()
{
	ImGui::TextColored(AmongUsColorToImVec4(GetPlayerColor(source.colorId)), source.playerName.c_str());
	ImGui::SameLine();
	ImGui::Text("[%s ago]", std::format("{:%OM:%OS}", (std::chrono::system_clock::now() - this->timestamp)).c_str());
}

void PhantomEvent::ColoredEventOutput()
{
	ImGui::Text("[");
	ImGui::SameLine();

	ImVec4 color;
	((action == PHANTOM_ACTIONS::PHANTOM_APPEAR) ? color = ImVec4(0.f, 1.f, 0.f, 1.f) : color = ImVec4(1.f, 0.f, 0.f, 1.f));

	ImGui::TextColored(color, ((action == PHANTOM_ACTIONS::PHANTOM_APPEAR) ? "APPEAR" : "VANISH"));
	ImGui::SameLine();
	ImGui::Text("]");
}