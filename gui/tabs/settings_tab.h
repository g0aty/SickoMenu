#pragma once

namespace SettingsTab {
	const std::vector<const char*> PLATFORMS = { "Epic Games", "Steam", "Mac", "MS Store", "itch.io", "iOS", "Android", "Switch", "Xbox", "Playstation", "Unknown" };
	const std::vector<const char*> MODS = { "SickoMenu", "AmongUsMenu", "KillNetwork" };
	const std::vector<const char*> AUVERSIONS = { "v16.0.0 / v16.0.2", "v16.0.5 / v16.1.0" };
	const std::vector<const char*> FONTS = { "Barlow-Italic", "Barlow-Medium", "Barlow-Bold", "Barlow-SemiBold", "Barlow-SemiBold (Masked)", "Barlow-ExtraBold", "Barlow-BoldItalic", "Barlow-BoldItalic (Masked)", "Barlow-Black", "Barlow-Light", "Barlow-Regular", "Barlow-Regular (Masked)", "Barlow-Regular (Outline)", "Brook", "LiberationSans", "NotoSans", "VCR", "CONSOLA", "digital-7", "OCRAEXT", "DIN_Pro_Bold_700" };
	void Render();
}