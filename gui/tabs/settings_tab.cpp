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
		Spoofing
	};

	static bool openGeneral = true; //default to general tab group
	static bool openSpoofing = false;

	void CloseOtherGroups(Groups group) {
		openGeneral = group == Groups::General;
		openSpoofing = group == Groups::Spoofing;
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
		if (openGeneral) {
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
			if (ToggleButton("Show Keybinds", &State.ShowKeybinds)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Allow Activating Keybinds while Chatting", &State.KeybindsWhileChatting)) {
				State.Save();
			}
			if (State.ShowKeybinds) {
				ImGui::Text("Show/Hide Menu Keybind:");
				ImGui::SameLine();
				if (HotKey(State.KeyBinds.Toggle_Menu)) {
					State.Save();
				}

				ImGui::Text("Show/Hide Console Keybind:");
				ImGui::SameLine();
				if (HotKey(State.KeyBinds.Toggle_Console)) {
					State.Save();
				}
			}
			if (ToggleButton("Always Show Menu on Startup", &State.ShowMenuOnStartup)) {
				State.Save();
			}
			if (ToggleButton("Panic (Disable SickoMenu)", &State.PanicMode)) {
				State.Save();
			}
			if (State.ShowKeybinds) {
				ImGui::SameLine();
				if (HotKey(State.KeyBinds.Toggle_Sicko)) {
					State.Save();
				}
			}
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			if (ToggleButton("Adjust by DPI", &State.AdjustByDPI)) {
				if (!State.AdjustByDPI) {
					State.dpiScale = 1.0f;
				}
				else {
					State.dpiScale = ImGui_ImplWin32_GetDpiScaleForHwnd(DirectX::window);
				}
				State.dpiChanged = true;
				State.Save();
			}

			static const std::vector<const char*> DPI_SCALING_LEVEL = { "50%", "55%", "60%", "65%", "70%", "75%", "80%", "85%", "90%", "95%", "100%", "105%", "110%", "115%", "120%", "125%", "130%", "135%", "140%", "145%", "150%", "155%", "160%", "165%", "170%", "175%", "180%", "185%", "190%", "195%", "200%", "205%", "210%", "215%", "220%", "225%", "230%", "235%", "240%", "245%", "250%", "255%", "260%", "265%", "270%", "275%", "280%", "285%", "290%", "295%", "300%" };
			ImGui::SameLine();
			int scaleIndex = (int(std::clamp(State.dpiScale, 0.5f, 3.0f) * 100.0f) - 50) / 5;
			if (CustomListBoxInt("Menu Scale", &scaleIndex, DPI_SCALING_LEVEL, 100 * State.dpiScale)) {
				State.dpiScale = (scaleIndex * 5 + 50) / 100.0f;
				State.dpiChanged = true;
			}

			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

			if (ImGui::ColorEdit3("Menu Theme Color", (float*)&State.MenuThemeColor, ImGuiColorEditFlags__OptionsDefault | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview)) {
				State.Save();
			}

			if (ToggleButton("RGB Menu Theme", &State.RgbMenuTheme)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset Menu Theme Color"))
			{
				State.MenuThemeColor = ImVec4(1.f, 0.f, 0.424f, 1.f);
			}

			SteppedSliderFloat("Opacity", (float*)&State.MenuThemeColor.w, 0.1f, 1.f, 0.01f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);

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

			if (Achievements::IsSupported() && ImGui::Button("Unlock all achievements"))
			{
				Achievements::UnlockAll();
			}

			ImGui::SameLine();
			if (ToggleButton("Unlock Cosmetics", &State.UnlockCosmetics)) {
				State.Save();
			}

			if (ToggleButton("Safe Mode", &State.SafeMode)) {
				State.Save();
			}
			/*ImGui::SameLine();
			if (ToggleButton("Spoof Modded Host", &State.SpoofModdedHost)) {
				State.Save(); //haven't figured this out yet
			}*/
			if (ToggleButton("Allow other SickoMenu users to see you're using SickoMenu", &State.SickoDetection)) {
				State.Save();
			}

			ImGui::Text("Keep safe mode on in official servers (NA, Europe, Asia) to prevent anticheat detection!");
		}
		if (openSpoofing) {
			if (ToggleButton("Spoof Guest Account", &State.SpoofGuestAccount)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ToggleButton("Use Custom Guest Friend Code", &State.UseGuestFriendCode)) {
				State.Save();
			}
			
			if (InputString("Guest Friend Code", &State.GuestFriendCode)) {
				State.Save();
			}
			ImGui::Text("Guest friend code should be <= 10 characters long and cannot have a hashtag.");
			ImGui::Text("It will only apply after restarting the game. For best results, use version.dll!");
			if (ToggleButton("Spoof Level", &State.SpoofLevel)) {
				State.Save();
			}
			ImGui::SameLine();
			if (ImGui::InputInt("Level", &State.FakeLevel, 0, 1)) {
				State.Save();
			}
			ImGui::Text("Spoofed friend code only applies as host!");
			if (ToggleButton("Spoof Friend Code", &State.SpoofFriendCode)) {
				State.Save();
			}
			if (InputString("Fake Friend Code", &State.FakeFriendCode)) {
				State.Save();
			}
			if (ToggleButton("Spoof Platform", &State.SpoofPlatform)) {
				State.Save();
			}
			ImGui::SameLine();
			if (CustomListBoxInt("Platform", &State.FakePlatform, PLATFORMS))
				State.Save();
		}
		ImGui::EndChild();
	}
}
