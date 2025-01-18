#include "pch-il2cpp.h"
#include "settings_tab.h"
#include "utility.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "game.h"
#include "achievements.hpp"
#include "DirectX.h"
#include "imgui/imgui_impl_win32.h"
#include "theme.hpp"
#include <cstdlib>

namespace AboutTab {
    enum Groups { Welcome, Credits };
    static bool openWelcome = true, openCredits = false;

    const ImVec4 Sicko = ImVec4(0.13f, 0.8f, 0.31f, 1.0f);
    const ImVec4 Menu = ImVec4(0.8f, 0.13f, 0.2f, 1.0f);
    const ImVec4 Orange = ImVec4(1.0f, 0.647f, 0.0f, 1.0f); // Оранжевый цвет
    const ImVec4 AUM_Color = ImVec4(180.0f / 255.0f, 50.0f / 255.0f, 93.0f / 255.0f, 1.0f);
    const ImVec4 lightSeaGreen = ImVec4(26.0f / 255.0f, 188.0f / 255.0f, 156.0f / 255.0f, 1.0f);

    void CloseOtherGroups(Groups group) {
        openWelcome = group == Welcome;
        openCredits = group == Credits;
    }

    void Render() {
        ImGui::SameLine(100 * State.dpiScale);
        ImGui::BeginChild("###About", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        if (TabGroup("Welcome", openWelcome)) CloseOtherGroups(Welcome);
        ImGui::SameLine();
        if (TabGroup("Credits", openCredits)) CloseOtherGroups(Credits);

        if (openWelcome) {
            ImGui::Text("Welcome %sto ", State.HasOpenedMenuBefore ? "back " : "");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Sicko, "Sicko");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Menu, "Menu");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("!");

            ImGui::TextColored(Sicko, "Sicko");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Menu, "Menu");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(" is a powerful utility for Among Us.");
            ImGui::Text("It aims to improve the game experience for all players!");
            ImGui::Text("Use the \"");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Orange, "Check for Updates");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("\" button to download the latest release!");

            if (ImGui::Button("GitHub")) OpenLink("https://github.com/g0aty/SickoMenu");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, Orange);
            if (ImGui::Button("Check for Updates")) OpenLink("https://github.com/g0aty/SickoMenu/releases/latest");
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.447f, 0.537f, 0.855f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.368f, 0.435f, 0.698f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.298f, 0.361f, 0.584f, 1.0f));
            ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
            if (ImGui::Button("Join our Discord!")) OpenLink("https://dsc.gg/sickos");
            ImGui::PopStyleColor(3);

            ImGui::TextColored(Sicko, "Sicko");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Menu, "Menu");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(" is a free and open-source software.");

            ImGui::Dummy(ImVec2(10, 10) * State.dpiScale);

            if (State.SickoVersion.find("pr") != std::string::npos) {
                ImGui::TextColored(Sicko, "You have access to pre-releases, enjoy!");
            }
            else {
                ImGui::TextColored(Menu, "If you've paid for this menu, demand a refund immediately.");
                ImGui::TextColored(Sicko, "Make sure you have downloaded the latest version of SickoMenu from GitHub or our");
                ImGui::TextColored(Sicko, "official Discord!");
            }
        }

        if (openCredits) {
            ImGui::TextColored(Sicko, "Sicko");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Menu, "Menu");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(" is a fork of ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(AUM_Color, "AmongUsMenu");

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(140.0f / 255.0f, 39.0f / 255.0f, 73.0f / 255.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(180.0f / 255.0f, 50.0f / 255.0f, 93.0f / 255.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(140.0f / 255.0f, 39.0f / 255.0f, 73.0f / 255.0f, 1.0f));

            if (ImGui::Button("AmongUsMenu")) OpenLink("https://github.com/BitCrackers/AmongUsMenu");

            ImGui::PopStyleColor(3);
            ImGui::Dummy(ImVec2(6, 6) * State.dpiScale);

            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(ImVec2(pos.x + 1, pos.y));
            ImGui::Text("Contributors:");
            ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + 1));
            ImGui::Text("Contributors:");
            ImGui::SetCursorScreenPos(pos);
            ImGui::Text("Contributors:");
            ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
            ImGui::PushStyleColor(ImGuiCol_Text, lightSeaGreen);
            if (ImGui::Button("GDjkhp")) OpenLink("https://github.com/GDjkhp");
            if (ImGui::Button("Reycko")) OpenLink("https://github.com/Reycko");
            if (ImGui::Button("astra1dev")) OpenLink("https://github.com/astra1dev");
            if (ImGui::Button("Luckyheat")) OpenLink("https://github.com/Luckyheat");
            ImGui::PopStyleColor();

            ImGui::Dummy(ImVec2(6, 6) * State.dpiScale);
            ImVec2 pos1 = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(ImVec2(pos1.x + 1, pos1.y));
            ImGui::Text("Some  people  who  contributed  to");
            ImGui::SetCursorScreenPos(ImVec2(pos1.x, pos1.y + 1));
            ImGui::Text("Some  people  who  contributed  to");
            ImGui::SetCursorScreenPos(pos1);
            ImGui::Text("Some  people  who  contributed  to");
            ImGui::SameLine();
            ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x + 1, ImGui::GetCursorScreenPos().y));

            ImVec2 pos2 = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(ImVec2(pos2.x + 1, pos2.y));
            ImGui::TextColored(ImVec4(180.0f / 255.0f, 50.0f / 255.0f, 93.0f / 255.0f, 1.0f), "AUM");
            ImGui::SetCursorScreenPos(ImVec2(pos2.x, pos2.y + 1));
            ImGui::TextColored(ImVec4(180.0f / 255.0f, 50.0f / 255.0f, 93.0f / 255.0f, 1.0f), "AUM");
            ImGui::SetCursorScreenPos(pos2);
            ImGui::TextColored(ImVec4(180.0f / 255.0f, 50.0f / 255.0f, 93.0f / 255.0f, 1.0f), "AUM");
            ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

            ImGui::PushStyleColor(ImGuiCol_Text, AUM_Color);
            if (ImGui::Button("KulaGGin")) {
                OpenLink("https://github.com/KulaGGin");
            }

            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::Text("(Helped with some ImGui code for replay system)");

            ImGui::PushStyleColor(ImGuiCol_Text, AUM_Color);
            if (ImGui::Button("tomsa000")) {
                OpenLink("https://github.com/tomsa000");
            }

            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::Text("(Helped with fixing memory leaks and smart pointers)");

            ImGui::PushStyleColor(ImGuiCol_Text, AUM_Color);
            if (ImGui::Button("cddjr")) {
                OpenLink("https://github.com/cddjr");
            }

            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::Text("(Helped in updating to the Fungle release)");

            ImGui::Dummy(ImVec2(6, 6) * State.dpiScale);

            ImGui::Text("Thanks to ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(AUM_Color, "v0idp");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(" for originally creating ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(AUM_Color, "AmongUsMenu");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("!");

            ImGui::PushStyleColor(ImGuiCol_Text, AUM_Color);
            if (ImGui::Button("v0idp")) OpenLink("https://github.com/v0idp");
            ImGui::PopStyleColor();

            ImGui::Dummy(ImVec2(6, 6) * State.dpiScale);

            ImGui::Text("Everyone else who contributed to ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(AUM_Color, "AUM");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(" and I couldn't list here.");
            ImGui::Text("Thank you for making ");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Sicko, "Sicko");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored(Menu, "Menu");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text(" possible!");
        }

        ImGui::EndChild();
    }
}
