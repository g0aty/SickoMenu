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
    // Begone, garbage code.

    using SabotageEntry = std::pair<const char*, SystemTypes__Enum>;
    using SabotageList = std::vector<SabotageEntry>;

    static const SabotageList skeldSabotages = {
        {"Reactor", SystemTypes__Enum::Reactor}, {"Lights", SystemTypes__Enum::Electrical},
        {"Oxygen", SystemTypes__Enum::LifeSupp}, {"Comms", SystemTypes__Enum::Comms},
        {"Doors", SystemTypes__Enum::Doors},
    };
    static const SabotageList miraHqSabotages = skeldSabotages;
    static const SabotageList polusSabotages = {
        {"Seismic Stabilizers", SystemTypes__Enum::Laboratory}, {"Lights", SystemTypes__Enum::Electrical},
        {"Oxygen", SystemTypes__Enum::LifeSupp}, {"Comms", SystemTypes__Enum::Comms},
        {"Doors", SystemTypes__Enum::Doors},
    };
    static const SabotageList airshipSabotages = {
        {"Lights", SystemTypes__Enum::Electrical}, {"Comms", SystemTypes__Enum::Comms},
        {"Doors", SystemTypes__Enum::Doors}, {"Crash Course", SystemTypes__Enum::HeliSabotage},
    };
    static const SabotageList fungleSabotages = {
        {"Reactor", SystemTypes__Enum::Reactor}, {"Doors", SystemTypes__Enum::Doors},
        {"Mushroom Mixup", SystemTypes__Enum::MushroomMixupSabotage}, {"Comms", SystemTypes__Enum::Comms},
    };

    static const SabotageList* GetCurrentSabotageList(const char** outMapName) {
        const SabotageList* list = &skeldSabotages;
        const char* mapName = "The Skeld";
        switch (State.mapType) {
        case Settings::MapType::Hq: list = &miraHqSabotages; mapName = "Mira HQ"; break;
        case Settings::MapType::Pb: list = &polusSabotages; mapName = "Polus"; break;
        case Settings::MapType::Airship: list = &airshipSabotages; mapName = "The Airship"; break;
        case Settings::MapType::Fungle: list = &fungleSabotages; mapName = "The Fungle"; break;
        default: break; 
        }
        *outMapName = mapName;
        return list;
    }

    void RenderDisableSabotages() {
        if (!IsHost()) return;
        const char* mapName = "The Skeld";
        const SabotageList* list;
        if (IsInLobby()) {
            Settings::MapType currentMapType = Settings::MapType::Ship;
            switch (GameOptions().GetByte(app::ByteOptionNames__Enum::MapId)) {
            case 1: currentMapType = Settings::MapType::Hq; break;
            case 2: currentMapType = Settings::MapType::Pb; break;
            case 4: currentMapType = Settings::MapType::Airship; break;
            case 5: currentMapType = Settings::MapType::Fungle; break;
            default: currentMapType = Settings::MapType::Ship; break;
            }
            static const SabotageList* lobbyList;
            switch (currentMapType) {
            case Settings::MapType::Hq: lobbyList = &miraHqSabotages; mapName = "Mira HQ"; break;
            case Settings::MapType::Pb: lobbyList = &polusSabotages; mapName = "Polus"; break;
            case Settings::MapType::Airship: lobbyList = &airshipSabotages; mapName = "The Airship"; break;
            case Settings::MapType::Fungle: lobbyList = &fungleSabotages; mapName = "The Fungle"; break;
            default: lobbyList = &skeldSabotages; mapName = "The Skeld"; break;
            }
            list = lobbyList;
        }
        else {
            list = GetCurrentSabotageList(&mapName);
        }
        ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
        bool first = true;
        for (auto& [name, sysType] : *list) {
            if (!first) ImGui::SameLine();
            first = false;
            bool disabled = State.DisabledSabotageTypes.count((int)sysType) > 0;
            if (ToggleButton(name, &disabled)) {
                if (disabled) State.DisabledSabotageTypes.insert((int)sysType);
                else State.DisabledSabotageTypes.erase((int)sysType);
                State.Save();
            }
        }
    }

    void Render() {
        if (!IsInGame()) return;

        ImGui::SameLine(100 * State.dpiScale);
        ImGui::BeginChild("###Sabotage", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
        ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

        if (IsHost() && ToggleButton("Disable Sabotages", &State.DisableSabotages)) {
            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
            ImGui::Separator();
            ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
        }
        if (AnimatedButton("Repair Sabotage")) {
            RepairSabotage(*Game::pLocalPlayer);
        }

        if (ToggleButton("Auto Repair Sabotages", &State.AutoRepairSabotage)) {
            State.Save();
        }

        ImGui::NewLine();
        if (State.DisableSabotages)
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Sabotages have been disabled. Nothing can be sabotaged.");
        //i skidded some code from https://github.com/scp222thj/MalumMenu/

        if (AnimatedButton("Sabotage All")) {
            if (State.mapType != Settings::MapType::Fungle) {
                for (uint32_t i = 0; i < 5; i++)
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

        if (AnimatedButton("Random Sabotage")) {
            switch (State.mapType) {
            case Settings::MapType::Pb:
            {
                int randIndex = randi(1, 3);
                switch (randIndex) {
                case 1:
                {
                    for (uint32_t i = 0; i < 5; i++)
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
                    for (uint32_t i = 0; i < 5; i++)
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
                    for (uint32_t i = 0; i < 5; i++)
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

        if (State.mapType != Settings::MapType::Fungle && AnimatedButton("Sabotage Lights")) {
            for (uint32_t i = 0; i < 5; i++)
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Electrical, i));
        }
        if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle) {
            if (AnimatedButton("Sabotage Reactor")) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Reactor, 128));
            }
        }
        else if (State.mapType == Settings::MapType::Pb) {
            if (AnimatedButton("Sabotage Seismic Stabilizers")) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Laboratory, 128));
            }
        }
        else if (State.mapType == Settings::MapType::Airship) {
            if (AnimatedButton("Sabotage Crash Course")) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::HeliSabotage, 128));
            }
        }
        if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq) {
            if (AnimatedButton("Sabotage Oxygen")) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::LifeSupp, 128));
            }
        }
        if (State.mapType == Settings::MapType::Fungle) {
            if (AnimatedButton("Activate Mushroom Mixup")) {
                State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::MushroomMixupSabotage, 1));
            }
        }
        if (AnimatedButton("Sabotage Comms")) {
            State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Comms, 128));
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
        if (ToggleButton("Disable Lights [Auto Moving Switches]", &State.DisableLightSwitches)) State.Save();

        if (ToggleButton("Disable Fix Comms", &State.DisableComms)) State.Save();

        if (ToggleButton("Spam Sabotage Reactor", &State.DisableReactor)) State.Save();

        if ((State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Hq) && ToggleButton("Spam Sabotage Oxygen", &State.DisableOxygen))
            State.Save();

        if (State.mapType == Settings::MapType::Fungle && ToggleButton("Infinite Mushroom Mixup", &State.InfiniteMushroomMixup))
            State.Save();

        ImGui::EndChild();
    }
}