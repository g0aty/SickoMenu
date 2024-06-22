#pragma once

namespace SelfTab {
	const std::vector<const char*> TPOPTIONS = { "None", "Radar", "Anywhere" };
	const std::vector<const char*> FAKEROLES = { "Crewmate", "Impostor", "Scientist", "Engineer", "Guardian Angel", "Shapeshifter", "Crewmate Ghost", "Impostor Ghost", "Noisemaker", "Phantom", "Tracker" };
	const std::vector<const char*> NAMEGENERATION = { "Word Combo", "Random String", "Cycler Names" };
	void Render();
}
