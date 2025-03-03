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
		Keybinds
	};

	static bool openGeneral = true; //default to general tab group
	static bool openSpoofing = false;
	static bool openKeybinds = false;

	void CloseOtherGroups(Groups group) {
		openGeneral = group == Groups::General;
		openSpoofing = group == Groups::Spoofing;
		openKeybinds = group == Groups::Keybinds;
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
				if (isNewConfig && (ImGui::Button(CheckConfigExists(State.selectedConfig) ? "Overwrite" : "Save"))) {
					State.Save();
					CONFIGS = GetAllConfigs();

					selectedConfigInt = std::distance(CONFIGS.begin(), std::find(CONFIGS.begin(), CONFIGS.end(), State.selectedConfig));
				}

				if (isDelete && CheckConfigExists(State.selectedConfig)) {
					if (ImGui::Button("Delete")) {
						selectedConfigInt--;
						State.Delete();
						CONFIGS = GetAllConfigs();
						if (selectedConfigInt < 0) selectedConfigInt = 0;
					}
				}
			}*/
#pragma endregion

			InputString("Config Name", &State.selectedConfig);

			if (CheckConfigExists(State.selectedConfig) && ImGui::Button("Load Config"))
			{
				State.Save(); //save previous settings
				State.Load();
				State.Save(); //actually save the selected config
			}
			if (CheckConfigExists(State.selectedConfig)) ImGui::SameLine();
			if (ImGui::Button("Save Config"))
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
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50 * State.dpiScale);
			if (ImGui::InputFloat("Menu Scale", &State.dpiScale)) {
				State.dpiScale = std::clamp(State.dpiScale, 0.5f, 3.f);
				State.dpiChanged = true;
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
			if (ImGui::Button("Reset Menu Theme"))
			{
				State.MenuThemeColor = ImVec4(1.f, 0.f, 0.424f, 1.f);
			}

			SteppedSliderFloat("Opacity", (float*)&State.MenuThemeColor.w, 0.1f, 1.f, 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

			if (ImGui::InputInt("FPS", &State.GameFPS)) {
				State.GameFPS = std::clamp(State.GameFPS, 1, 2147483647);
				State.Save();
			}

#ifdef _DEBUG
			if (ToggleButton("Show Debug Tab", &State.showDebugTab)) {
				State.Save();
			}
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
#endif
			if (InputString("Username", &State.userName)) {
				State.Save();
			}

			if (!(IsHost() || !State.SafeMode)) {
				if (State.userName.length() >= (size_t)13)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username is too long, gets detected by anticheat. This name will be ignored.");
				else if (!IsNameValid(State.userName))
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Username contains characters blocked by anticheat. This name will be ignored.");
			}

			if ((IsInGame() || IsInLobby()) && (IsNameValid(State.userName) || (IsHost() || !State.SafeMode)) && ImGui::Button("Set Name")) {
				if (IsInGame())
					State.rpcQueue.push(new RpcSetName(State.userName));
				else if (IsInLobby())
					State.lobbyRpcQueue.push(new RpcSetName(State.userName));
				LOG_INFO("Successfully set in-game name to \"" + State.userName + "\"");
			}
			if ((IsInGame() || IsInLobby())) ImGui::SameLine();
			if (IsNameValid(State.userName) && ImGui::Button("Set as Account Name")) {
				SetPlayerName(State.userName);
				LOG_INFO("Successfully set account name to \"" + State.userName + "\"");
			}
			ImGui::SameLine();
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
			static bool showMessage = false;

			if (ToggleButton("Unlock Cosmetics", &State.UnlockCosmetics)) {
				State.Save();
				showMessage = true;
				timer = static_cast<float>(ImGui::GetTime());
			}

			if (showMessage) {
				float currentTime = static_cast<float>(ImGui::GetTime());
				if (currentTime - timer < 4.0f) {
					ImGui::SameLine();
					if (State.UnlockCosmetics)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Unlocked All Cosmetics!");
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Disabled Unlock Cosmetics!");
				}
				else {
					showMessage = false;
				}
			}

			if (Achievements::IsSupported())
			{
				ImGui::SameLine();
				if (ImGui::Button("Unlock All Achievements"))
					Achievements::UnlockAll();
			}

			if (ToggleButton("Safe Mode", &State.SafeMode)) {
				State.Save();
			}

			static float timer1 = 0.0f;
			static bool showMessage1 = false;

			if (showMessage1) {
				float currentTime = static_cast<float>(ImGui::GetTime());
				if (currentTime - timer1 < 4.0f) {
					ImGui::SameLine();
					if (State.UnlockCosmetics)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Unlocked All Cosmetics!");
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Disabled Unlock Cosmetics!");
				}
				else {
					showMessage1 = false;
				}
			}

			static int modToShow = 0;

			if (ToggleButton("Allow other mod users to see you're using", &State.ModDetection)) {
				State.Save();
			}
			ImGui::SameLine();
			if (CustomListBoxInt(" ", &modToShow, MODS, 100.f * State.dpiScale)) {
				State.SickoDetection = modToShow == 0;
				State.AmongUsMenuDetection = modToShow == 1;
				State.KillNetworkDetection = modToShow == 2;
				State.Save();
			}

			ImGui::Text("Keep safe mode on in official servers (NA, Europe, Asia) to prevent anticheat detection!");
		}
		if (openSpoofing) {
			if (ToggleButton("Spoof Guest Account", &State.SpoofGuestAccount)) {
				State.Save();
			}
			if (State.SpoofGuestAccount) {
				ImGui::SameLine();
				if (ToggleButton("Use Custom Guest Friend Code", &State.UseGuestFriendCode)) {
					State.Save();
				}
				if (State.UseGuestFriendCode) {
					if (InputString("Guest Friend Code", &State.GuestFriendCode)) {
						State.Save();
					}
					ImGui::Text("Guest friend code should be <= 10 characters long and cannot have a hashtag.");
				}
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Pro Tip: You can bypass the free chat restriction using a space after your custom friend");
				ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "code!");

				/*if (ToggleButton("Use Custom Guest PUID", &State.UseGuestPuid)) {
					State.Save();
				}
				if (State.UseGuestPuid) {
					ImGui::SameLine();
					if (InputString("Guest PUID", &State.GuestPuid)) {
						State.Save();
					}
				}*/
			}
			/*if (ImGui::Button("Force Login as Guest")) {
				State.ForceLoginAsGuest = true;
			}*/
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

			//if (ToggleButton("Disable Host Anticheat", &State.DisableHostAnticheat)) State.Save();
			if (ToggleButton("Disable Host Anticheat (+25 Mode)", &State.DisableHostAnticheat)) {
				if (!State.DisableHostAnticheat && State.BattleRoyale) {
					State.BattleRoyale = false;
					State.GameMode = 0;
				}
				State.Save();
			}
			/*if (ToggleButton("Spoof RPC", &State.RPCSpoof)) {
				State.Save();
			}
			if (CustomListBoxInt("RPC", &State.RPC, RPC))
				State.Save();*/
		}

		if (openKeybinds) {
			if (HotKey(State.KeyBinds.Toggle_Menu)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Menu");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Console)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Console");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Radar))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Radar");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Replay))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Replay");

			if (HotKey(State.KeyBinds.Toggle_ChatAlwaysActive)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Show/Hide Chat");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_ReadGhostMessages)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Read Ghost Messages");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Sicko))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Panic Mode");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Hud))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Enable/Disable HUD");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Freecam)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Freecam");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Zoom))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Zoom");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Noclip))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("NoClip");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Toggle_Autokill))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Autokill");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Reset_Appearance))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Reset Appearance");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Randomize_Appearance))
				State.Save();
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Confuse Now");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Repair_Sabotage)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Repair All Sabotages");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Close_All_Doors)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Close All Doors");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Close_Current_Room_Door)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Close Current Room Door");

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (HotKey(State.KeyBinds.Complete_Tasks)) {
				State.Save();
			}
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::Text("Complete All Tasks");
		}
		ImGui::EndChild();
	}
}