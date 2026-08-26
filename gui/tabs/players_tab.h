#pragma once
#include "utility.h"

namespace PlayersTab {
	const std::vector<const char*> FAKEROLES = { "Crewmate", "Impostor", "Scientist", "Engineer", "Guardian Angel", "Shapeshifter", "Crewmate Ghost", "Impostor Ghost", "Noisemaker", "Phantom", "Tracker", "Detective", "Viper", "Judge"};
	const std::vector<const char*> GHOSTROLES = { "Guardian Angel", "Crewmate Ghost", "Impostor Ghost" };
	const std::vector<const char*> SHIPVENTS = { "Admin", "Hallway", "Cafeteria", "Electrical", "Upper Engine", "Security", "Medbay", "Weapons", "Lower Reactor", "Lower Engine", "Shields", "Upper Reactor", "Upper Navigation", "Lower Navigation" };
	const std::vector<const char*> HQVENTS = { "Balcony", "Cafeteria", "Reactor", "Laboratory", "Office", "Admin", "Greenhouse", "Medbay", "Decontamination", "Locker Room", "Launchpad" };
	const std::vector<const char*> PBVENTS = { "Security", "Electrical", "O2", "Communications", "Office", "Admin", "Laboratory", "Lava Pool", "Storage", "Right Seismic", "Left Seismic", "Outside Admin" };
	const std::vector<const char*> AIRSHIPVENTS = { "Vault", "Cockpit", "Viewing Deck", "Engine", "Kitchen", "Lower Main Hall", "Upper Main Hall", "Right Gap Room", "Left Gap Room", "Showers", "Records", "Cargo Bay" };
	const std::vector<const char*> FUNGLEVENTS = { "Communications", "Kitchen", "Lookout", "Outside Dorm", "Laboratory", "Jungle (Laboratory)", "Jungle (Greenhouse)", "Splash Zone", "Cafeteria" };
	const std::vector<const char*> COLORS = { "Red", "Blue", "Green", "Pink", "Orange", "Yellow", "Black", "White", "Purple", "Brown", "Cyan", "Lime", "Maroon", "Rose", "Banana", "Gray", "Tan", "Coral", "Fortegreen" };
	const ColorMapping COLOR_NAMES_COLOR[] = {
		{"Red",			    ImColor::ImColor(0xFF1111C6)}, // 0xAABBGGRR
		{"Blue",			ImColor::ImColor(0xFFD22E13)},
		{"Green",			ImColor::ImColor(0xFF2D8011)},
		{"Pink",			ImColor::ImColor(0xFFBB54EE)},
		{"Orange",			ImColor::ImColor(0xFF0D7DF0)},
		{"Yellow",			ImColor::ImColor(0xFF57F6F6)},
		{"Black",			ImColor::ImColor(0xFF4E473F)},
		{"White",			ImColor::ImColor(0xFFF1E1D7)},
		{"Purple",			ImColor::ImColor(0xFFBC2F6B)},
		{"Brown",			ImColor::ImColor(0xFF1E4971)},
		{"Cyan",			ImColor::ImColor(0xFFDDFF38)},
		{"Lime",			ImColor::ImColor(0xFF39F050)},
		{"Maroon",			ImColor::ImColor(0xFF2E1D5F)},
		{"Rose",			ImColor::ImColor(0xFFD3C0EC)},
		{"Banana",			ImColor::ImColor(0xFFA8E7F0)},
		{"Gray",			ImColor::ImColor(0xFF938575)},
		{"Tan",			    ImColor::ImColor(0xFF778891)},
		{"Coral",			ImColor::ImColor(0xFF6464D7)},
		{"Fortegreen",      ImColor::ImColor(0xFF62A626)},
	};
	void Render();
	void OpenSubGroup(const std::string& name);
}