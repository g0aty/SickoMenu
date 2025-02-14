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
#include <cstdlib>

namespace AboutTab {
    enum Groups {
        Welcome,
        Credits
    };

    static bool openWelcome = true; // Default to welcome tab group
    static bool openCredits = false;

    void CloseOtherGroups(Groups group) {
        openWelcome = group == Groups::Welcome;
        openCredits = group == Groups::Credits;
    }

    void Render() {
        ImGui::SameLine(100 * State.dpiScale);
        ImGui::BeginChild("###About", ImVec2(500 * State.dpiScale, 300), true, ImGuiWindowFlags_NoBackground);
        
        if (TabGroup("Welcome", openWelcome)) {
            CloseOtherGroups(Groups::Welcome);
        }
        ImGui::SameLine();
        if (TabGroup("Credits", openCredits)) {
            CloseOtherGroups(Groups::Credits);
        }

        if (openWelcome) {
            std::string welcomeText = std::format("Welcome {}to SickoMenu {}!", State.HasOpenedMenuBefore ? "back " : "", State.SickoVersion);
            ImGui::Text(welcomeText.c_str());
            ImGui::Text("SickoMenu is a powerful utility for Among Us.");
            ImGui::Text("It aims to improve the game experience for all players!");
            ImGui::Text("Use the \"Check for Updates\" button to download the latest release!");
            
            if (ImGui::Button("GitHub")) {
                OpenLink("https://github.com/g0aty/SickoMenu");
            }
            ImGui::SameLine();
            if (ImGui::Button("Check for Updates")) {
                OpenLink("https://github.com/g0aty/SickoMenu/releases/latest");
            }

            // Add note about manually updating SickoMenu
            ImGui::Text("Note: After downloading the latest release, you must manually update SickoMenu.");
            ImGui::Text("Follow the same installation steps as before, but replace the old DLL with the new one.");
            ImGui::Text("Make sure to close the game before updating!");

            ImGui::Text("Join the Discord server for support, bug reports, and sneak peeks!");
            if (ImGui::Button("Join our Discord!")) {
                OpenLink("https://dsc.gg/sickos"); // SickoMenu Discord invite
            }

            ImGui::Text("SickoMenu is a free and open-source software.");

            if (State.SickoVersion.find("pr") != std::string::npos) {
                ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "You have access to pre-releases, enjoy!");
            }
            else {
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "If you've paid for this menu, demand a refund immediately.");
                ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Make sure you have downloaded the latest version of SickoMenu from GitHub or our");
                ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "official Discord!");
            }
        }

        if (openCredits) {
            ImGui::Text("SickoMenu is a fork of AmongUsMenu (archived), go check it out!");
            if (ImGui::Button("AmongUsMenu")) {
                OpenLink("https://github.com/BitCrackers/AmongUsMenu");
            }

            ImGui::Text("Contributors:");
            if (ImGui::Button("GDjkhp")) {
                OpenLink("https://github.com/GDjkhp");
            }

            if (ImGui::Button("Reycko")) {
                OpenLink("https://github.com/Reycko");
            }

            if (ImGui::Button("astra1dev")) {
                OpenLink("https://github.com/astra1dev");
            }

            if (ImGui::Button("Luckyheat")) {
                OpenLink("https://github.com/Luckyheat");
            }
            ImGui::SameLine();
            ImGui::Text("Developed the Destruct tab features for SickoMenu.");

            ImGui::Text("Some people who contributed to AUM:");
            if (ImGui::Button("KulaGGin")) {
                OpenLink("https://github.com/KulaGGin");
            }
            ImGui::SameLine();
            ImGui::Text("(Helped with some ImGui code for replay system)");

            if (ImGui::Button("tomsa000")) {
                OpenLink("https://github.com/tomsa000");
            }
            ImGui::SameLine();
            ImGui::Text("(Helped with fixing memory leaks and smart pointers)");

            if (ImGui::Button("cddjr")) {
                OpenLink("https://github.com/cddjr");
            }
            ImGui::SameLine();
            ImGui::Text("(Helped in updating to the Fungle release)");

            ImGui::Text("Thanks to v0idp for originally creating AmongUsMenu!");
            if (ImGui::Button("v0idp")) {
                OpenLink("https://github.com/v0idp");
            }                        

            ImGui::Text("Thank you to everyone else who contributed to AUM,");
            ImGui::Text("and whose contributions couldn't all be listed here.");
            ImGui::Text("Your support has made SickoMenu possible!");

            ImGui::Text("Thank you to all contributors, SickoMenu Discord staff, and developers");
            ImGui::Text("whose efforts have made SickoMenu possible!");

            // Add thanks for donators and server boosters
            ImGui::Text("A special thanks to our donators and those who boosted the SickoMenu Discord server.");
            ImGui::Text("Your contributions are greatly appreciated!");
        }
        ImGui::EndChild();
    }
}
