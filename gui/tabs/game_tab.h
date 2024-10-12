#pragma once
#include <vector>
#include "state.hpp"

namespace GameTab {
	const std::vector<const char*> KILL_DISTANCE = { "Short", "Medium", "Long", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20" };
	const std::vector<const char*> TASKBARUPDATES = { "Always", "Meetings", "Never", "3", "4", "5", "6" };
	const std::vector<const char*> COLORS = { "Red", "Blue", "Green", "Pink", "Orange", "Yellow", "Black", "White", "Purple", "Brown", "Cyan", "Lime", "Maroon", "Rose", "Banana", "Gray", "Tan", "Coral" };
	const std::vector<const char*> HOSTCOLORS = { "Red", "Blue", "Green", "Pink", "Orange", "Yellow", "Black", "White", "Purple", "Brown", "Cyan", "Lime", "Maroon", "Rose", "Banana", "Gray", "Tan", "Coral", "Fortegreen"};
	const std::vector<const char*> SMAC_PUNISHMENTS = { "Do Nothing", "Warn Self"/*, "Warn All (Chat)", State.SafeMode ? "Attempt to Ban" : "Attempt to Kick"*/};
	const std::vector<const char*> SMAC_HOST_PUNISHMENTS = { "Do Nothing", "Warn Self"/*, "Warn All (Chat)"*/, "Kick", "Ban"};
	void Render();
}