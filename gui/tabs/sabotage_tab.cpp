#include "pch-il2cpp.h"
#include "sabotage_tab.h"
#include "utility.h"
#include "gui-helpers.hpp"
#include "_rpc.h"
#include "game.h"
#include "state.hpp"
#include <random>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <atomic>

namespace SabotageTab {

    std::atomic<bool> automaticToggle(false);
    std::thread toggleThread;

    static void ToggleSwitches(SwitchSystem* switchSystem) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);

        while (automaticToggle) {
            auto actualSwitches = switchSystem->fields.ActualSwitches;

            for (int i = 0; i < 5; i++) {
                if (dis(gen)) {
                    actualSwitches ^= (1 << i);
                }
            }

            switchSystem->fields.ActualSwitches = actualSwitches;

            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, actualSwitches));

            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }

        switchSystem->fields.ActualSwitches = 31;

        State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, switchSystem->fields.ActualSwitches));

        std::cout << "Sabotage repaired.\n";
    }

    void Render() {
        if (IsInGame()) {
            ImGui::SameLine(100 * State.dpiScale);
            ImGui::BeginChild("###Sabotage", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
            ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
            if (IsHost() && ToggleButton("Disable Sabotages", &State.DisableSabotages)) {
                ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
                ImGui::Separator();
                ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
            }
            if (ImGui::Button("Repair Sabotages")) {
                RepairSabotage(*Game::pLocalPlayer);
            }

            if (ToggleButton("Auto Repair Sabotages", &State.AutoRepairSabotage)) {
                State.Save();
            }

            ImGui::NewLine();
            if (State.DisableSabotages)
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sabotages have been disabled. Nothing can be sabotaged.");
            //i skidded some code from https://github.com/scp222thj/MalumMenu/

            if (ImGui::Button("Sabotage All")) {
                if (State.mapType != Settings::MapType::Fungle) {
                    for (size_t i = 0; i < 5; i++)
                        State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                }

                if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle)
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128));
                else if (State.mapType == Settings::MapType::Pb)
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128));
                else if (State.mapType == Settings::MapType::Airship)
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128));

                if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq)
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128));

                if (State.mapType == Settings::MapType::Fungle)
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1));

                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128));
            }

            if (ImGui::Button("Random Sabotage")) {
                switch (State.mapType) {
                case Settings::MapType::Pb:
                {
                    int randIndex = randi(1, 3);
                    switch (randIndex) {
                    case 1:
                    {
                        for (size_t i = 0; i < 5; i++)
                            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                    } break;
                    case 2: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128)); break;
                    case 3: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                case Settings::MapType::Airship:
                {
                    int randIndex = randi(1, 3);
                    switch (randIndex) {
                    case 1:
                    {
                        for (size_t i = 0; i < 5; i++)
                            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                    } break;
                    case 2: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128)); break;
                    case 3: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                case Settings::MapType::Fungle:
                {
                    int randIndex = randi(1, 3);
                    switch (randIndex) {
                    case 1: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1)); break;
                    case 2: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128)); break;
                    case 3: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                default:
                {
                    int randIndex = randi(1, 4);
                    switch (randIndex) {
                    case 1:
                    {
                        for (size_t i = 0; i < 5; i++)
                            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                    } break;
                    case 2: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128)); break;
                    case 3: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128)); break;
                    case 4: State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128)); break;
                    }
                }
                break;
                }
            }

            if (State.mapType != Settings::MapType::Fungle && ImGui::Button("Sabotage Lights")) {
                for (size_t i = 0; i < 5; i++)
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
            }
            if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle) {
                if (ImGui::Button("Sabotage Reactor")) {
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128));
                }
            }
            else if (State.mapType == Settings::MapType::Pb) {
                if (ImGui::Button("Sabotage Seismic Stabilizers")) {
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128));
                }
            }
            else if (State.mapType == Settings::MapType::Airship) {
                if (ImGui::Button("Sabotage Crash Course")) {
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128));
                }
            }
            if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq) {
                if (ImGui::Button("Sabotage Oxygen")) {
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128));
                }
            }
            if (State.mapType == Settings::MapType::Fungle) {
                if (ImGui::Button("Activate Mushroom Mixup")) {
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1));
                }
            }
            if (ImGui::Button("Sabotage Comms")) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128));
            }

            if (State.mapType == Settings::MapType::Fungle) {
                if (&State.MushroomMixupInterval) {
                    ImGui::SliderInt("Mushroom Mixup Interval (ms)", reinterpret_cast<int*>(&State.MushroomMixupInterval), 100, 1000);
                }
                if (ImGui::Checkbox("Mushroom Mixup", &State.MushroomMixup)) {
                    State.Save();
                }
            }

            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);

            if (State.mapType != Settings::MapType::Fungle) {
                if (ToggleButton("Disable Lights", &State.DisableLights)) {
                    if (auto switchSystem = (SwitchSystem*)il2cpp::Dictionary((*Game::pShipStatus)->fields.Systems)[SystemTypes__Enum::Electrical]) {
                        auto actualSwitches = switchSystem->fields.ActualSwitches;
                        auto expectedSwitches = switchSystem->fields.ExpectedSwitches;

                        auto switchMask = 1 << ((State.DisableLights ? 0 : 5) & 0x1F);

                        if ((actualSwitches & switchMask) != ((State.DisableLights ? ~expectedSwitches : expectedSwitches) & switchMask)) {
                            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, 5));
                        }
                    }
                }
            }
            ImGui::SameLine();
            if (ToggleButton("Disable Lights [Auto Movement Switches]", &State.DisableLightsAlt)) {
                if (State.DisableLightsAlt) {
                    if (State.mapType != Settings::MapType::Fungle) {
                        for (size_t i = 0; i < 5; i++) {
                            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
                        }

                        if (!automaticToggle) {
                            automaticToggle = true;
                            toggleThread = std::thread(ToggleSwitches, (SwitchSystem*)il2cpp::Dictionary((*Game::pShipStatus)->fields.Systems)[SystemTypes__Enum::Electrical]);
                            toggleThread.detach();
                        }
                    }
                }
                else {
                    automaticToggle = false;
                    auto switchSystem = (SwitchSystem*)il2cpp::Dictionary((*Game::pShipStatus)->fields.Systems)[SystemTypes__Enum::Electrical];
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, switchSystem->fields.ExpectedSwitches));
                    State.rpcQueue = std::queue<RPCInterface*>();
                }
            }
            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Pb ||
                State.mapType == Settings::MapType::Airship ||
                State.mapType == Settings::MapType::Fungle) {
                if (ToggleButton("Disable Fix Comms", &State.UnfixableCommsPrev)) {
                    State.Save();
                }
            }
            ImGui::SameLine();
            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Pb ||
                State.mapType == Settings::MapType::Airship ||
                State.mapType == Settings::MapType::Fungle) {
                if (ToggleButton("Spam Sabotage Comms", &State.UnfixableComms)) {
                    State.Save();
                }
            }
            ImGui::Dummy(ImVec2(15, 15) * State.dpiScale);
            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Fungle) {
                if (ToggleButton("Spam Sabotage Reactor", &State.UnfixableReactor)) {
                    State.Save();
                }
            }

            if (State.mapType == Settings::MapType::Pb) {
                if (ToggleButton("Spam Sabotage Seismic Stabilizers", &State.UnfixableLaboratory)) {
                    State.Save();
                }
            }



            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq) {
                if (ToggleButton("Spam Sabotage Oxygen", &State.UnfixableO2)) {
                    State.Save();
                }
            }

            if (State.mapType == Settings::MapType::Airship) {
                if (ToggleButton("Spam Sabotage Crash Course", &State.UnfixableCrashCourse)) {
                    State.Save();
                }
            }
            ImGui::EndChild();
        }
    }
}
