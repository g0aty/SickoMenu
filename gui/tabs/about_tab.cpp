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

    static bool openWelcome = true; //default to welcome tab group
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
            ImGui::Text("SickoMenu is a highly sophisticated cheat menu for Among Us based on AmongUsMenu.");
            ImGui::Text("It promises more features and regular updates.");
            ImGui::Text("Use the \"Check for Updates\" button to download the latest release!");
            if (ImGui::Button("GitHub")) {
                OpenLink("https://github.com/g0aty/SickoMenu");
            }
            ImGui::SameLine();
            if (ImGui::Button("Check for Updates")) {
                OpenLink("https://github.com/g0aty/SickoMenu/releases/latest");
            }
            ImGui::Text("Join the Discord server for support, bug reports, and sneak peeks!");
            if (ImGui::Button("Join our Discord!")) {
                OpenLink("https://dsc.gg/sickos"); //sickomenu discord invite
            }
            ImGui::Text("SickoMenu is a free and open-source software.");
            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "If you've paid for this menu, demand a refund immediately.");
            ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Make sure you have downloaded the latest version of SickoMenu from GitHub or our");
            ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "official Discord!");
            //hopefully stop people from reselling a foss menu for actual money
        }

        if (openCredits) {
            ImGui::Text("SickoMenu is a fork of AmongUsMenu, go check it out!");
            if (ImGui::Button("AmongUsMenu")) {
                OpenLink("https://github.com/BitCrackers/AmongUsMenu");
            }
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

            ImGui::Text("Everyone else who contributed to AUM and I couldn't list here.");
            ImGui::Text("Thank you for making SickoMenu possible!");
        }
        ImGui::EndChild();
    }
}