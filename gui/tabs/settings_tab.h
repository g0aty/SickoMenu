#pragma once

namespace SettingsTab {
	const std::vector<const char*> PLATFORMS = { "Epic Games", "Steam", "Mac", "MS Store", "itch.io", "iOS", "Android", "Switch", "Xbox", "Playstation", "Unknown" };
	const std::vector<const char*> MODS = { "SickoMenu", "AmongUsMenu", "KillNetwork" };
	void Render();
}