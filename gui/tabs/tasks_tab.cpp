#include "pch-il2cpp.h"
#include "tasks_tab.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"

namespace TasksTab {
	void Render() {
		if (IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.Tasks != NULL) {
			ImGui::SameLine(100 * State.dpiScale);
			ImGui::BeginChild("###Tasks", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
			ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
			//if (!PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))) {
			auto tasks = GetNormalPlayerTasks(*Game::pLocalPlayer);

			bool allTasksComplete = false;
			uint16_t tasksCompleted = 0;
			for (auto task : tasks) {
				if (task->fields.taskStep == task->fields.MaxStep)
					++tasksCompleted;
			}

			if (tasks.size() != tasksCompleted) {
				if (AnimatedButton("Complete All Tasks")) {
					CompleteAllTasks();
				}
			}
			if (!State.SafeMode) {
				ImGui::SameLine();
			}
			if (!State.SafeMode && AnimatedButton("Complete Everyone's Tasks")) {
				for (auto player : GetAllPlayerControl()) {
					CompleteAllTasks(player);
				}
			}

			ImGui::NewLine();

			for (size_t i = 0; i < tasks.size(); ++i) {
				auto task = tasks[i];
				if (!NormalPlayerTask_get_IsComplete(task, NULL) && AnimatedButton(("Complete##" + std::to_string(task->fields._._Id_k__BackingField)).c_str())) {
					State.taskRpcQueue.push(new RpcCompleteTask(task->fields._._Id_k__BackingField));
				}
				else if (NormalPlayerTask_get_IsComplete(task, NULL)) {
					ColoredButton(ImVec4(0.f, 1.f, 0.f, 1.f), ("Completed!##" + std::to_string(task->fields._._Id_k__BackingField)).c_str());
				}

				ImGui::SameLine();

				auto taskIncompleteCol = State.LightMode ? AmongUsColorToImVec4(app::Palette__TypeInfo->static_fields->Black) : AmongUsColorToImVec4(app::Palette__TypeInfo->static_fields->White);

				ImGui::TextColored(NormalPlayerTask_get_IsComplete(task, NULL)
					? ImVec4(0.0F, 1.0F, 0.0F, 1.0F)
					: taskIncompleteCol
					, TranslateTaskTypes(task->fields._.TaskType));
			}

			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			//}

			GameOptions options;
			if (!options.GetBool(app::BoolOptionNames__Enum::VisualTasks) && ToggleButton("Bypass Visual Tasks Being Off", &State.BypassVisualTasks))
				State.Save();

			if (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) {
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Visual tasks are turned OFF in this lobby.");
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Any animations (other than cameras) are client-sided only!");
			}
			else if (options.GetGameMode() == GameModes__Enum::HideNSeek)
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Animations other than cameras are client-sided only in Hide n Seek!");

			if (State.mapType == Settings::MapType::Ship) {
				if (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek) {
					if (AnimatedButton("Play Shields Animation (Client-sided)"))
					{
						State.rpcQueue.push(new RpcPlayAnimation(1));
					}
				}
				else {
					if (AnimatedButton("Play Shields Animation"))
					{
						State.rpcQueue.push(new RpcPlayAnimation(1));
					}
				}
			}

			if (State.mapType == Settings::MapType::Ship) {
				if (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek) {
					if (AnimatedButton("Play Trash Animation (Client-sided)"))
					{
						State.rpcQueue.push(new RpcPlayAnimation(10));
					}
				}
				else {
					if (AnimatedButton("Play Trash Animation"))
					{
						State.rpcQueue.push(new RpcPlayAnimation(10));
					}
				}
			}

			if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Pb) {

				if (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek) {
					if (ToggleButton("Play Weapons Animation (Client-sided)", &State.PlayWeaponsAnimation))
					{
						State.Save();
					}
				}
				else {
					if (ToggleButton("Play Weapons Animation", &State.PlayWeaponsAnimation))
					{
						State.Save();
					}
				}
			}

			if (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek) {
				if (ToggleButton("Play Medbay Scan Animation (Client-sided)", &State.PlayMedbayScan))
				{
					if (State.PlayMedbayScan)
					{
						State.rpcQueue.push(new RpcSetScanner(true));
					}
					else
					{
						State.rpcQueue.push(new RpcSetScanner(false));
					}
				}
			}
			else {
				if (ToggleButton("Play Medbay Scan Animation", &State.PlayMedbayScan))
				{
					if (State.PlayMedbayScan)
					{
						State.rpcQueue.push(new RpcSetScanner(true));
					}
					else
					{
						State.rpcQueue.push(new RpcSetScanner(false));
					}
				}
			}

			if (!(State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle) && ToggleButton("Fake Cameras In Use", &State.FakeCameraUsage))
			{
				State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Security, (State.FakeCameraUsage ? 1 : 0)));
			}

			if (IsInMultiplayerGame() && IsInGame()) {
				float taskPercentage = 0.0f;
				if ((float)(*Game::pGameData)->fields.TotalTasks > 0.0f) {
					taskPercentage = (float)(*Game::pGameData)->fields.CompletedTasks / (float)(*Game::pGameData)->fields.TotalTasks;
				}
				ImGui::TextColored(ImVec4(1.0f - taskPercentage, 1.0f, 1.0f - taskPercentage, 1.0f), "%.2f%% Total Tasks Completed", taskPercentage * 100);
			}

			ImGui::EndChild();
		}
	}
}
