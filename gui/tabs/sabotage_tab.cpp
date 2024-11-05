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

namespace SabotageTab {

    std::atomic<bool> infiniteMushroomMixupActive(false);
    std::atomic<int> mushroomMixupInterval(100);
    std::atomic<bool> unfixCommsActive(false);
    std::atomic<bool> unfixFungleCommsActive(false);
    std::atomic<bool> unfixReactorActive(false);
    std::atomic<bool> unfixOxygenActive(false);
    std::atomic<bool> unfixCrashCourseActive(false);
    std::atomic<bool> unfixPolusReactorActive(false);
    std::atomic<bool> automaticToggle(false);
    bool brokeSystemSwitchesToggled = false;
    std::thread toggleThread;

    static void MushroomMixupSabotageLoop() {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        static std::uniform_int_distribution<int> dist(10000, 99999);

        while (infiniteMushroomMixupActive) {
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1));

            std::this_thread::sleep_for(std::chrono::milliseconds(mushroomMixupInterval));
        }
    }

    static int generateRandomCode() {
        static std::random_device rd;
        static std::mt19937 mt(rd());
        static std::uniform_int_distribution<int> dist(10000, 99999);
        return dist(mt);
    }

    static void UnfixFungleCommsLoop() {
        static std::mt19937 gen(std::random_device{}());
        static std::uniform_int_distribution<> dis(5, 20);
        while (unfixFungleCommsActive) {
            if (State.mapType == Settings::MapType::Fungle) {
                int newCode = generateRandomCode();
                if (std::rand() % 10 == 0) {
                    newCode = 0;
                }

                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, newCode));
            }
            int delay = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        RepairSabotage(*Game::pLocalPlayer);
    }

    class RpcLoopHelper {
    public:
        static int generateRandomCode() {
            static std::random_device rd;
            static std::mt19937 mt(rd());
            static std::uniform_int_distribution<int> dist(10000, 99999);
            return dist(mt);
        }

        static int getRandomDelay(int min = 10, int max = 20) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<> dis(min, max);
            return dis(gen);
        }

        static bool shouldResetCode() {
            return std::rand() % 10 == 0;
        }

        static void pushCommsUpdate(int code) {
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, code));
        }

        static void pushReactorUpdate(int code) {
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, code));
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128));
        }

        static void pushOxygenUpdate(int code) {
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, code));
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128));
        }

        static void pushPolusReactorUpdate(int code) {
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, code));
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128));
        }

        static void performRepair() {
            RepairSabotage(*Game::pLocalPlayer);
        }

        static bool isValidMapTypeForComms() {
            return State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Pb ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Airship;
        }

        static bool isValidMapTypeForReactor() {
            return State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Airship ||
                State.mapType == Settings::MapType::Fungle;
        }

        static bool isValidMapTypeForPolusReactor() {
            return State.mapType == Settings::MapType::Pb;
        }

        static bool isValidMapTypeForOxygen() {
            return State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq;
        }
    };

    static void UnfixCommsLoop() {
        while (unfixCommsActive) {
            if (RpcLoopHelper::isValidMapTypeForComms()) {
                RpcLoopHelper::pushCommsUpdate(128);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        RpcLoopHelper::performRepair();
    }

    static void UnfixReactorLoop() {
        auto lastRepairTime = std::chrono::steady_clock::now();
        const auto repairInterval = std::chrono::milliseconds(250);

        while (unfixReactorActive) {
            if (RpcLoopHelper::isValidMapTypeForReactor()) {
                int newCode = RpcLoopHelper::generateRandomCode();
                if (RpcLoopHelper::shouldResetCode()) {
                    newCode = 0;
                }
                RpcLoopHelper::pushReactorUpdate(newCode);
            }

            auto now = std::chrono::steady_clock::now();
            if (now - lastRepairTime >= repairInterval) {
                RpcLoopHelper::performRepair();
                lastRepairTime = now;
            }

            int delay = RpcLoopHelper::getRandomDelay();
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        RpcLoopHelper::performRepair();
    }

    static void UnfixPolusReactorLoop() {
        auto lastRepairTime = std::chrono::steady_clock::now();
        const auto repairInterval = std::chrono::milliseconds(250);

        while (unfixPolusReactorActive) {
            if (RpcLoopHelper::isValidMapTypeForPolusReactor()) {
                int newCode = RpcLoopHelper::generateRandomCode();
                if (RpcLoopHelper::shouldResetCode()) {
                    newCode = 0;
                }
                RpcLoopHelper::pushPolusReactorUpdate(newCode);
            }

            auto now = std::chrono::steady_clock::now();
            if (now - lastRepairTime >= repairInterval) {
                RpcLoopHelper::performRepair();
                lastRepairTime = now;
            }

            int delay = RpcLoopHelper::getRandomDelay();
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        RpcLoopHelper::performRepair();
    }

    static void UnfixOxygenLoop() {
        auto lastRepairTime = std::chrono::steady_clock::now();
        const auto repairInterval = std::chrono::milliseconds(250);

        while (unfixOxygenActive) {
            if (RpcLoopHelper::isValidMapTypeForOxygen()) {
                int newCode = RpcLoopHelper::generateRandomCode();
                if (RpcLoopHelper::shouldResetCode()) {
                    newCode = 0;
                }
                RpcLoopHelper::pushOxygenUpdate(newCode);
            }

            auto now = std::chrono::steady_clock::now();
            if (now - lastRepairTime >= repairInterval) {
                RpcLoopHelper::performRepair();
                lastRepairTime = now;
            }

            int delay = RpcLoopHelper::getRandomDelay();
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        RpcLoopHelper::performRepair();
    }

    static void UnfixCrashCourseLoop() {
        auto lastRepairTime = std::chrono::steady_clock::now();
        const auto repairInterval = std::chrono::milliseconds(250);

        while (unfixCrashCourseActive) {
            if (State.mapType == Settings::MapType::Airship) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128));
            }
            auto now = std::chrono::steady_clock::now();
            if (now - lastRepairTime >= repairInterval) {
                RepairSabotage(*Game::pLocalPlayer);
                lastRepairTime = now;
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(10, 20);
            int delay = dis(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        RepairSabotage(*Game::pLocalPlayer);
    }

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
            if (ImGui::Button("Repair Sabotage")) {
                RepairSabotage(*Game::pLocalPlayer);
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
                ImGui::SliderInt("Mushroom Mixup Interval (ms)", reinterpret_cast<int*>(&mushroomMixupInterval), 100, 1000);
                bool infiniteMushroomMixupActiveLocal = infiniteMushroomMixupActive;
                if (ImGui::Checkbox("Infinite Mushroom Mixup", &infiniteMushroomMixupActiveLocal)) {
                    infiniteMushroomMixupActive = infiniteMushroomMixupActiveLocal;
                    if (infiniteMushroomMixupActive) {
                        std::thread(MushroomMixupSabotageLoop).detach();
                    }
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
            if (ToggleButton("Disable Lights [Auto Movement Switches]", &brokeSystemSwitchesToggled)) {
                if (brokeSystemSwitchesToggled) {
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
                    switchSystem->fields.ActualSwitches = switchSystem->fields.ExpectedSwitches;
                    State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, switchSystem->fields.ActualSwitches));
                }
            }
            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Pb ||
                State.mapType == Settings::MapType::Airship ||
                State.mapType == Settings::MapType::Fungle) {
                bool unfixCommsActiveLocal = unfixCommsActive.load();
                if (ToggleButton("Disable Fix Comms", &unfixCommsActiveLocal)) {
                    unfixCommsActive.store(unfixCommsActiveLocal);
                    if (unfixCommsActive) {
                        std::thread(UnfixCommsLoop).detach();
                    }
                }
            }
            ImGui::Dummy(ImVec2(15, 15) * State.dpiScale);
            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq ||
                State.mapType == Settings::MapType::Fungle) {
                bool unfixReactorActiveLocal = unfixReactorActive.load();
                if (ToggleButton("Spam Sabotage Reactor", &unfixReactorActiveLocal)) {
                    unfixReactorActive.store(unfixReactorActiveLocal);
                    if (unfixReactorActive) {
                        std::thread(UnfixReactorLoop).detach();
                    }
                }
            }

            if (State.mapType == Settings::MapType::Pb) {
                bool unfixPolusReactorActiveLocal = unfixPolusReactorActive.load();
                if (ToggleButton("Spam Sabotage Reactor", &unfixPolusReactorActiveLocal)) {
                    unfixPolusReactorActive.store(unfixPolusReactorActiveLocal);
                    if (unfixPolusReactorActive) {
                        std::thread(UnfixPolusReactorLoop).detach();
                    }
                }
            }

            if (State.mapType == Settings::MapType::Ship ||
                State.mapType == Settings::MapType::Hq) {
                bool unfixOxygenActiveLocal = unfixOxygenActive.load();
                if (ToggleButton("Spam Sabotage Oxygen", &unfixOxygenActiveLocal)) {
                    unfixOxygenActive.store(unfixOxygenActiveLocal);
                    if (unfixOxygenActive) {
                        std::thread(UnfixOxygenLoop).detach();
                    }
                }
            }

            if (State.mapType == Settings::MapType::Airship) {
                bool unfixCrashCourseActiveLocal = unfixCrashCourseActive.load();
                if (ToggleButton("Disable Fix Crash Course", &unfixCrashCourseActiveLocal)) {
                    unfixCrashCourseActive.store(unfixCrashCourseActiveLocal);
                    if (unfixCrashCourseActive) {
                        std::thread(UnfixCrashCourseLoop).detach();
                    }
                }
            }
            if (State.mapType == Settings::MapType::Fungle) {
                bool unfixFungleCommsActiveLocal = unfixFungleCommsActive.load();
                if (ToggleButton("Spam Fungle Fix Comms", &unfixFungleCommsActiveLocal)) {
                    unfixFungleCommsActive.store(unfixFungleCommsActiveLocal);
                    if (unfixFungleCommsActive) {
                        std::thread(UnfixFungleCommsLoop).detach();
                    }
                }
            }
            ImGui::EndChild();
        }
    }
}
