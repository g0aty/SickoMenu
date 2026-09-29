#include "pch-il2cpp.h"
#include "_events.h"
#include "utility.h"

ModerationEvent::ModerationEvent(const EVENT_PLAYER& source, const std::string& message) : EventInterface(source, EVENT_TYPES::EVENT_MODERATION) {
	this->message = message;
}

void ModerationEvent::Output() {
	ImGui::Text("%s", this->message.c_str());
	ImGui::SameLine();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - this->timestamp);
	long long totalSeconds = elapsed.count();
	long long minutes = totalSeconds / 60;
	long long seconds = totalSeconds % 60;
	ImGui::Text("[%02lld:%02lld ago]", minutes, seconds);
}

void ModerationEvent::ColoredEventOutput() {
	ImGui::Text("[");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 0.65f, 0.f, 1.f), "MOD");
	ImGui::SameLine();
	ImGui::Text("]");
}