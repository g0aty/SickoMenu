#pragma once

namespace SettingsTab {
	const std::vector<const char*> PLATFORMS = { "Epic Games", "Steam", "Mac", "MS Store", "itch.io", "iOS", "Android", "Switch", "Xbox", "Playstation", "Unknown" };
	const std::vector<const char*> MODS = { "SickoMenu", "AmongUsMenu", "KillNetwork" };
	const std::vector<const char*> AUVERSIONS = { "v16.0.0 / v16.0.2", "v16.0.5 / v16.1.0" };
	const std::vector<const char*> FONTS = { "Barlow-Italic", "Barlow-Medium", "Barlow-Bold", "Barlow-SemiBold", "Barlow-SemiBold (Masked)", "Barlow-ExtraBold", "Barlow-BoldItalic", "Barlow-BoldItalic (Masked)", "Barlow-Black", "Barlow-Light", "Barlow-Regular", "Barlow-Regular (Masked)", "Barlow-Regular (Outline)", "Brook", "LiberationSans", "NotoSans", "VCR", "CONSOLA", "digital-7", "OCRAEXT", "DIN_Pro_Bold_700" };
	const std::vector<const char*> TIME_OFFSETS = { "+", "-" };
	const ColorMapping PLATFORM_NAMES_COLOR[] = {
		{"Epic Games",		ImColor::ImColor(0xFF19E1FF)}, // 0xAABBGGRR
		{"Steam",			ImColor::ImColor(0xFF909946)},
		{"Mac",				ImColor::ImColor(0xFFFFBEDC)},
		{"MS Store",		ImColor::ImColor(0xFF3182F5)},
		{"itch.io",			ImColor::ImColor(0xFF000080)},
		{"iOS",				ImColor::ImColor(0xFFD86343)},
		{"Android",			ImColor::ImColor(0xFF4BB43C)},
		{"Switch",			ImColor::ImColor(0xFF4B19E6)},
		{"Xbox",			ImColor::ImColor(0xFFC3FFAA)},
		{"Playstation",		ImColor::ImColor(0xFFF4D442)},
		{"Unknown",			ImColor::ImColor(0xFFA9A9A9)},
	};
	void Render();
	void OpenSubGroup(const std::string& name);
}