#pragma once

namespace SelfTab {
	const std::vector<const char*> TPOPTIONS = { "None", "Radar", "Anywhere" };
	const std::vector<const char*> FAKEROLES = { "Crewmate", "Impostor", "Scientist", "Engineer", "Guardian Angel", "Shapeshifter", "Crewmate Ghost", "Impostor Ghost", "Noisemaker", "Phantom", "Tracker" };
	const std::vector<const char*> NAMEGENERATION = { "Word Combo", "Random String", "Cycler Names" };
	const std::vector<const char*> BODYTYPES = { "Normal", "Horse", "Long" };
	const std::vector<const char*> FONTS = { "Barlow-Italic", "Barlow-Medium", "Barlow-Bold", "Barlow-SemiBold", "Barlow-SemiBold (Masked)", "Barlow-ExtraBold", "Barlow-BoldItalic", "Barlow-BoldItalic (Masked)", "Barlow-Black", "Barlow-Light", "Barlow-Regular", "Barlow-Regular (Masked)", "Barlow-Regular (Outline)", "Brook", "LiberationSans", "NotoSans", "VCR", "CONSOLA", "digital-7", "OCRAEXT", "DIN_Pro_Bold_700"};
	//const std::vector<const char*> MATERIALS = { };
	void Render();
}
