#include "pch-il2cpp.h"
#include "settings_tab.h"
#include "utility.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "game.h"
#include "achievements.hpp"
#include "DirectX.h"
#include "imgui/imgui_impl_win32.h" // ImGui_ImplWin32_GetDpiScaleForHwnd
#include "theme.hpp" // ApplyTheme

namespace SettingsTab {
	enum Groups {
		General,
		Spoofing,
		Customization,
		Keybinds
	};

	static bool openGeneral = true; //default to general tab group
	static bool openSpoofing = false;
	static bool openCustomization = false;
	static bool openKeybinds = false;

	void CloseOtherGroups(Groups group) {
		openGeneral = group == Groups::General;
		openSpoofing = group == Groups::Spoofing;
		openCustomization = group == Groups::Customization;
		openKeybinds = group == Groups::Keybinds;
	}

	void CheckKeybindEdit(bool hotKey) {
		State.KeybindsBeingEdited = State.KeybindsBeingEdited || hotKey;
	}

	void Render() {
		ImGui::SameLine(100 * State.dpiScale);
		ImGui::BeginChild("###Settings", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		if (TabGroup("General", openGeneral)) {
			CloseOtherGroups(Groups::General);
		}
		ImGui::SameLine();
		if (TabGroup("Spoofing", openSpoofing)) {
			CloseOtherGroups(Groups::Spoofing);
		}
		ImGui::SameLine();
		if (TabGroup("Customization", openCustomization)) {
			CloseOtherGroups(Groups::Customization);
		}
		ImGui::SameLine();
		if (TabGroup("Keybinds", openKeybinds)) {
			CloseOtherGroups(Groups::Keybinds);
		}
		if (openGeneral) {
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
			if (ToggleButton("Show Keybinds", &State.ShowKeybinds)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Allow Activating Keybinds while Chatting", &State.KeybindsWhileChatting)) {
				State.Save();
			}
			if (ToggleButton("Always Show Menu on Startup", &State.ShowMenuOnStartup)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Panic Warning", &State.PanicWarning)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Extra Commands", &State.ExtraCommands)) {
				State.Save();
			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Type \"/help\" in chat to see all available commands.");
			}
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

			// sorry to anyone trying to read this code it is pretty messy
#pragma region New config menu, needs fixing
			/*
			std::vector<std::string> CONFIGS = GetAllConfigs();
			CONFIGS.push_back("[New]");
			CONFIGS.push_back("[Delete]");

			std::vector<const char*> CONFIGS_CHAR;

			for (const std::string& str : CONFIGS) {
				char* ch = new char[str.size() + 1];
				std::copy(str.begin(), str.end(), ch);
				ch[str.size()] = '\0';
				CONFIGS_CHAR.push_back(ch);
			}

			bool isNewConfig = CONFIGS.size() == 1;
			bool isDelete = false;

			int& selectedConfigInt = State.selectedConfigInt;
			std::string selectedConfig = CONFIGS[selectedConfigInt];

			if (CustomListBoxInt("Configs", &selectedConfigInt, CONFIGS_CHAR), 100 * State.dpiScale, ImVec4(0,0,0,0), ImGuiComboFlags_NoArrowButton) {
				isNewConfig = selectedConfigInt == CONFIGS.size() - 2;
				isDelete = selectedConfigInt == CONFIGS.size() - 1;
				if (!isNewConfig && !isDelete) State.selectedConfig = CONFIGS[selectedConfigInt];
				State.Save();
				State.Load();
			}

			if (isNewConfig || isDelete) {
				InputString("Name", &State.selectedConfig);
				if (isNewConfig && (AnimatedButton(CheckConfigExists(State.selectedConfig) ? "Overwrite" : "Save"))) {
					State.Save();
					CONFIGS = GetAllConfigs();

					selectedConfigInt = std::distance(CONFIGS.begin(), std::find(CONFIGS.begin(), CONFIGS.end(), State.selectedConfig));
				}

				if (isDelete && CheckConfigExists(State.selectedConfig)) {
					if (AnimatedButton("Delete")) {
						selectedConfigInt--;
						State.Delete();
						CONFIGS = GetAllConfigs();
						if (selectedConfigInt < 0) selectedConfigInt = 0;
					}
				}
			}*/
#pragma endregion

			InputString("Config Name", &State.selectedConfig);

			if (CheckConfigExists(State.selectedConfig) && AnimatedButton("Load Config"))
			{
				State.SaveConfig();
				State.Load();
				State.Save(); //actually save the selected config
			}
			if (CheckConfigExists(State.selectedConfig)) ImGui::SameLine();
			if (AnimatedButton("Save Config"))
			{
				State.Save();
			}
			if (!CheckConfigExists(State.selectedConfig)) {
				ImGui::Text("Config name not found!");
				ImGui::SameLine();
			}

			/*if (ToggleButton("Adjust by DPI", &State.AdjustByDPI)) {
				if (!State.AdjustByDPI) {
					State.dpiScale = 1.0f;
				}
				else {
					State.dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(DirectX::window);
				}
				State.dpiChanged = true;
				State.Save();
			}*/

			/*static const std::vector<const char*> DPI_SCALING_LEVEL = {"50%", "60%", "70%", "80%", "90%", "100%", "110%", "120%", "130%", "140%", "150%", "160%", "170%", "180%", "190%", "200%", "210%", "220%", "230%", "240%", "250%", "260%", "270%", "280%", "290%", "300%"};
			
			int scaleIndex = (int(std::clamp(State.dpiScale, 0.5f, 3.0f) * 100.0f) - 50) / 5;
			if (CustomListBoxInt("Menu Scale", &scaleIndex, DPI_SCALING_LEVEL, 100 * State.dpiScale)) {
				State.dpiScale = (scaleIndex * 10 + 50) / 100.0f;
				State.dpiChanged = true;
			}*/

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (ImGui::InputInt("FPS", &State.GameFPS)) {
				State.GameFPS = std::clamp(State.GameFPS, 1, 2147483647);
				State.Save();
			}

			ImGui::Dummy(ImVec2(1, 1) * State.dpiScale);

			if (ToggleButton("Auto-Exit Due To Low FPS", &State.LeaveDueLFPS)) {
				State.Save();
			}
			ImGui::SameLine();
			ImGui::PushItemWidth(80);
			ImGui::InputInt("Minimum FPS", &State.minFpsThreshold);
			if (State.minFpsThreshold < 0)
				State.minFpsThreshold = 0;
			ImGui::PopItemWidth();

			ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);

#ifdef _DEBUG
			if (ToggleButton("Show Debug Tab", &State.showDebugTab)) {
				State.Save();
			}
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
#endif
			if (!IsHost() && !State.SafeMode && !IsNameValid(State.userName)) {
				ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.f, 0.f, State.MenuThemeColor.w));
				if (InputString("Username", &State.userName)) State.Save();
				ImGui::PopStyleColor();
			}
			else if (InputString("Username", &State.userName)) State.Save();

			if (!IsNameValid(State.userName) && !IsHost() && State.SafeMode) {
				if (State.userName == "")
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Empty username gets detected by anticheat. This name will be ignored.");
				if (State.userName.length() >= (size_t)13)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username is too long, gets detected by anticheat. This name will be ignored.");
				else if (!IsNameValid(State.userName))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username contains characters blocked by anticheat. This name will be ignored.");
				else
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username gets detected by anticheat. This name will be ignored.");
			}

			if (IsNameValid(State.userName) || IsHost() || !State.SafeMode) {
				if ((IsInGame() || IsInLobby()) && AnimatedButton("Set Name")) {
					if (IsInGame())
						State.rpcQueue.push(new RpcSetName(State.userName));
					else if (IsInLobby())
						State.lobbyRpcQueue.push(new RpcSetName(State.userName));
					LOG_INFO("Successfully set in-game name to \"" + State.userName + "\"");
				}
				if (IsNameValid(State.userName)) {
					if ((IsInGame() || IsInLobby())) ImGui::SameLine();
					if (AnimatedButton("Set as Account Name")) {
						SetPlayerName(State.userName);
						LOG_INFO("Successfully set account name to \"" + State.userName + "\"");
					}
				}
				ImGui::SameLine();
			}
			if (ToggleButton("Automatically Set Name", &State.SetName)) {
				State.Save();
			}

			if (InputString("Custom Code", &State.customCode)) {
				State.Save();
			}
			if (ToggleButton("Replace Streamer Mode Lobby Code", &State.HideCode)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("RGB Lobby Code", &State.RgbLobbyCode)) {
				State.Save();
			}

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

			static float timer = 0.0f;
			static bool CosmeticsNotification = false;

			if (ToggleButton("Unlock Cosmetics", &State.UnlockCosmetics)) {
				State.Save();
				CosmeticsNotification = true;
				timer = static_cast<float>(ImGui::GetTime());
			}

			if (CosmeticsNotification) {
				float currentTime = static_cast<float>(ImGui::GetTime());
				if (currentTime - timer < 5.0f) {
					ImGui::SameLine();
					if (State.UnlockCosmetics)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Cosmetics Are Unlocked!");
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Cosmetics Are Locked!");
				}
				else {
					CosmeticsNotification = false;
				}
			}

			if (Achievements::IsSupported())
			{
				ImGui::SameLine();
				if (AnimatedButton("Unlock All Achievements"))
					Achievements::UnlockAll();
			}

			if (ToggleButton("Allow other mod users to see you're using", &State.ModDetection)) State.Save();
			ImGui::SameLine();
			if (CustomListBoxInt(" ", &State.BroadcastedMod, MODS, 100.f * State.dpiScale)) State.Save();
		}
		if (openSpoofing) {
			/*if (ToggleButton("Spoof Guest Account", &State.SpoofGuestAccount)) {
				State.Save();
			}
			if (State.SpoofGuestAccount) {
				ImGui::SameLine();
				if (ToggleButton("Use Custom Guest Friend Code", &State.UseNewFriendCode)) {
					State.Save();
				}
				if (State.UseNewFriendCode) {
					if (InputString("Guest Friend Code", &State.NewFriendCode)) {
						State.Save();
					}
					ImGui::Text("Guest friend code should be <= 10 characters long and cannot have a hashtag.");
				}
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Pro Tip: You can bypass the free chat restriction using a space after your custom friend");
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "code!");
			}*/
			/*if (AnimatedButton("Force Login as Guest")) {
				State.ForceLoginAsGuest = true;
			}*/
			if (ToggleButton("Spoof Guest Account (Quick Chat ONLY)", &State.SpoofGuestAccount)) {
				State.Save();
			}
			if (ToggleButton("Use Custom Friend Code (For New/Guest Account ONLY)", &State.UseNewFriendCode)) {
				State.Save();
			}
			if (State.UseNewFriendCode) {
				ImGui::SetNextItemWidth(150 * State.dpiScale); // Adjust the width of the input box
				if (InputString("Friend Code (For New/Guest Account ONLY)", &State.NewFriendCode)) {
					State.Save();
				}
				ImGui::Text("This new friend code should be <= 10 characters long and cannot have spaces.");
			}
			if (ToggleButton("Spoof Level", &State.SpoofLevel)) {
				State.Save();
			}
			if (State.SpoofLevel) {
				ImGui::SameLine();
				if (ImGui::InputInt("Level", &State.FakeLevel, 0, 1)) {
					State.Save();
				}

				if (State.SafeMode && (State.FakeLevel <= 0 || State.FakeLevel > 100001))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Level will be detected by anticheat, your level will be between 0 and 100001.");
			}

			if (ToggleButton("Spoof Platform", &State.SpoofPlatform)) {
				State.Save();
			}
			if (State.SpoofPlatform) {
				ImGui::SameLine();
				if (CustomListBoxInt("Platform", &State.FakePlatform, PLATFORMS))
					State.Save();
			}

			if (ToggleButton("Disable Anticheat While Hosting (+25 Mode)", &State.DisableHostAnticheat)) {
				if (!State.DisableHostAnticheat && State.BattleRoyale) {
					State.BattleRoyale = false;
					State.GameMode = 0;
				}
				State.Save();
			}
			/*if (State.DisableHostAnticheat) {
				BoldText("Warning (+25 Mode)", ImVec4(1.f, 0.f, 0.f, 1.f));
				BoldText("With this option enabled, you can only find public lobbies with +25 enabled.");
				BoldText("You may not find any public lobbies in the game listing due to this.");
				BoldText("This is intended behaviour, do NOT report it as a bug.");
			}*/
			if (ToggleButton("Spoof Among Us Version", &State.SpoofAUVersion))
				State.Save();
			if (State.SpoofAUVersion) {
				ImGui::SameLine();
				if (CustomListBoxInt("Version", &State.FakeAUVersion, AUVERSIONS))
					State.Save();
			}
		}

		if (openCustomization) {
			if (ToggleButton("Hide Watermark", &State.HideWatermark)) {
				State.Save();
			}

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (ToggleButton("Light Mode", &State.LightMode)) State.Save();
			ImGui::SameLine();
			if (!State.GradientMenuTheme) {
				if (ImGui::ColorEdit3("Menu Theme Color", (float*)&State.MenuThemeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
					State.Save();
				}
			}
			else {
				if (ImGui::ColorEdit3("Gradient Color 1", (float*)&State.MenuGradientColor1, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
					State.Save();
				}
				ImGui::SameLine();
				if (ImGui::ColorEdit3("Gradient Color 2", (float*)&State.MenuGradientColor2, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
					State.Save();
				}
			}
			ImGui::SameLine();
			if (ToggleButton("Gradient Theme", &State.GradientMenuTheme))
				State.Save();

			if (ToggleButton("Match Background with Theme", &State.MatchBackgroundWithTheme)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("RGB Menu Theme", &State.RgbMenuTheme)) {
				State.Save();
			}
			ImGui::SameLine();
			if (AnimatedButton("Reset Menu Theme"))
			{
				State.MenuThemeColor = ImVec4(1.f, 0.f, 0.424f, 1.f);
			}

			SteppedSliderFloat("Opacity", (float*)&State.MenuThemeColor.w, 0.1f, 1.f, 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (ToggleButton("Dark Game Theme", &State.DarkMode)) State.Save();
			ImGui::SameLine();
			if (ToggleButton("Custom Game Theme", &State.CustomGameTheme)) State.Save();

			if (State.CustomGameTheme) {
				if (ImGui::ColorEdit3("Background Color", (float*)&State.GameBgColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview))
					State.Save();
				ImGui::SameLine();
				if (ImGui::ColorEdit3("Text Color", (float*)&State.GameTextColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview))
					State.Save();
			}
			if (ToggleButton("Change Chat Font", &State.ChatFont)) {
				State.Save();
			}
			if (State.ChatFont) {
				ImGui::SameLine();
				if (CustomListBoxInt(" ", &State.ChatFontType, FONTS, 160.f * State.dpiScale)) {
					State.Save();
				}
			}

			ImGui::Dummy(ImVec2(4, 4)* State.dpiScale);

			if (ImGui::CollapsingHeader("GUI")) {
				ImGui::SetNextItemWidth(50 * State.dpiScale);
				if (ImGui::InputFloat("Menu Scale", &State.dpiScale)) {
					State.dpiScale = std::clamp(State.dpiScale, 0.5f, 3.f);
					State.dpiChanged = true;
					State.Save();
				}
				if (ToggleButton("Disable Animations", &State.DisableAnimations))
					State.Save();
				if (ImGui::InputFloat("Animation Speed", &State.AnimationSpeed)) {
					if (State.AnimationSpeed <= 0) State.AnimationSpeed = 1.f;
					State.Save();
				}
				SteppedSliderFloat("Rounding Radius Multiplier", &State.RoundingRadiusMultiplier, 0.f, 2.f, 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);
			}

			if (ImGui::CollapsingHeader("Role Colors")) {
				ImGui::ColorEdit4("Crewmate Ghost", (float*)&State.CrewmateGhostColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Crewmate", (float*)&State.CrewmateColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * State.dpiScale);
				ImGui::ColorEdit4("Engineer", (float*)&State.EngineerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				ImGui::ColorEdit4("Guardian Angel", (float*)&State.GuardianAngelColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Scientist", (float*)&State.ScientistColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * State.dpiScale);
				ImGui::ColorEdit4("Impostor", (float*)&State.ImpostorColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				
				ImGui::ColorEdit4("Shapeshifter", (float*)&State.ShapeshifterColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Impostor Ghost", (float*)&State.ImpostorGhostColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * State.dpiScale);
				ImGui::ColorEdit4("Noisemaker", (float*)&State.NoisemakerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				
				ImGui::ColorEdit4("Tracker", (float*)&State.TrackerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Phantom", (float*)&State.PhantomColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				if (AnimatedButton("Reset Role Colors")) {
					State.CrewmateGhostColor = ImVec4(1.f, 1.f, 1.f, 0.5f);
					State.CrewmateColor = ImVec4(1.f, 1.f, 1.f, 1.f);
					State.EngineerColor = ImVec4(0.f, 1.f, 1.f, 1.f);
					State.GuardianAngelColor = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);
					State.ScientistColor = ImVec4(0.2f, 0.2f, 1.f, 1.f);
					State.ImpostorColor = ImVec4(1.f, 0.1f, 0.1f, 1.f);
					State.ShapeshifterColor = ImVec4(1.f, 0.67f, 0.f, 1.f);
					State.ImpostorGhostColor = ImVec4(0.25f, 0.25f, 0.25f, 0.5f);
					State.NoisemakerColor = ImVec4(0.f, 1.f, 0.47f, 1.f);
					State.TrackerColor = ImVec4(0.65f, 0.36f, 1.f, 1.f);
					State.PhantomColor = ImVec4(0.53f, 0.f, 0.f, 1.f);
					State.Save();
				}
			}

			if (ImGui::CollapsingHeader("Other Colors")) {
				ImGui::ColorEdit4("Lobby Host", (float*)&State.HostColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Player ID", (float*)&State.PlayerIdColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * State.dpiScale);
				ImGui::ColorEdit4("Player Level", (float*)&State.LevelColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				ImGui::ColorEdit4("Platform", (float*)&State.PlatformColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Mod Usage", (float*)&State.ModUsageColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * State.dpiScale);
				ImGui::ColorEdit4("Name-Checker", (float*)&State.NameCheckerColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				ImGui::ColorEdit4("Friend Code", (float*)&State.FriendCodeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(150.f * State.dpiScale);
				ImGui::ColorEdit4("Dater Names", (float*)&State.DaterNamesColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				ImGui::SameLine(300.f * State.dpiScale);
				ImGui::ColorEdit4("Lobby Code", (float*)&State.LobbyCodeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
				
				ImGui::ColorEdit4("Lobby Age", (float*)&State.AgeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);

				if (AnimatedButton("Reset Other Colors")) {
					State.HostColor = ImVec4(1.f, 0.73f, 0.f, 1.f);
					State.PlayerIdColor = ImVec4(1.f, 0.f, 0.f, 1.f);
					State.LevelColor = ImVec4(0.f, 1.f, 0.f, 1.f);
					State.PlatformColor = ImVec4(0.73f, 0.f, 1.f, 1.f);
					State.ModUsageColor = ImVec4(1.f, 0.73f, 0.f, 1.f);
					State.NameCheckerColor = ImVec4(1.f, 0.67f, 0.f, 1.f);
					State.FriendCodeColor = ImVec4(0.2f, 0.6f, 1.f, 1.f);
					State.DaterNamesColor = ImVec4(1.f, 0.f, 0.f, 1.f);
					State.LobbyCodeColor = ImVec4(1.f, 0.73f, 0.f, 1.f);
					State.AgeColor = ImVec4(0.f, 1.f, 0.f, 1.f);
					State.Save();
				}
			}
		}

		if (openKeybinds) {
			State.KeybindsBeingEdited = false; // This should not stay on permanently

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Menu));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Menu");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Console));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Console");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Radar));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Radar");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Replay));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Replay");

			ImGui::Dummy(ImVec2(4, 4)* State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_ChatAlwaysActive));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Chat");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_ReadGhostMessages));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Read Ghost Messages");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Sicko));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Panic Mode");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Hud));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Enable/Disable HUD");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Freecam));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Freecam");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Zoom));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Zoom");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Noclip));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("NoClip");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Toggle_Autokill));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Autokill");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Reset_Appearance));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Reset Appearance");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Randomize_Appearance));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Confuse Now");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Repair_Sabotage));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Repair All Sabotages");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Close_All_Doors));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Close All Doors");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Close_Current_Room_Door));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Close Current Room Door");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Complete_Tasks));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Complete All Tasks");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			CheckKeybindEdit(HotKey(State.KeyBinds.Cancel_Start));
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Cancel Start Game");
		}
		ImGui::EndChild();
	}
}
