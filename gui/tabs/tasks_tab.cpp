#include "pch-il2cpp.h"
#include "tasks_tab.h"
#include "game.h"
#include "state.hpp"
#include "utility.h"
#include "gui-helpers.hpp"

namespace TasksTab {
	using TaskList = std::vector<TaskTypes__Enum>;

	static const TaskList skeldTasks = {
		TaskTypes__Enum::SubmitScan, TaskTypes__Enum::PrimeShields, TaskTypes__Enum::FuelEngines,
		TaskTypes__Enum::ChartCourse, TaskTypes__Enum::StartReactor, TaskTypes__Enum::SwipeCard,
		TaskTypes__Enum::ClearAsteroids, TaskTypes__Enum::UploadData, TaskTypes__Enum::EmptyChute,
		TaskTypes__Enum::EmptyGarbage, TaskTypes__Enum::AlignEngineOutput, TaskTypes__Enum::FixWiring,
		TaskTypes__Enum::CalibrateDistributor, TaskTypes__Enum::DivertPower, TaskTypes__Enum::UnlockManifolds,
		TaskTypes__Enum::CleanO2Filter, TaskTypes__Enum::VentCleaning, TaskTypes__Enum::StabilizeSteering,
	};
	static const TaskList miraHqTasks = {
		TaskTypes__Enum::SubmitScan, TaskTypes__Enum::PrimeShields, TaskTypes__Enum::UploadData,
		TaskTypes__Enum::StartReactor, TaskTypes__Enum::ClearAsteroids, TaskTypes__Enum::EmptyGarbage,
		TaskTypes__Enum::FixWiring, TaskTypes__Enum::DivertPower, TaskTypes__Enum::UnlockManifolds,
		TaskTypes__Enum::VentCleaning, TaskTypes__Enum::AssembleArtifact, TaskTypes__Enum::SortSamples,
		TaskTypes__Enum::MeasureWeather, TaskTypes__Enum::EnterIdCode, TaskTypes__Enum::BuyBeverage,
		TaskTypes__Enum::ProcessData, TaskTypes__Enum::RunDiagnostics, TaskTypes__Enum::WaterPlants,
		TaskTypes__Enum::MonitorOxygen,
	};
	static const TaskList polusTasks = {
		TaskTypes__Enum::SubmitScan, TaskTypes__Enum::FuelEngines, TaskTypes__Enum::ChartCourse,
		TaskTypes__Enum::StartReactor, TaskTypes__Enum::SwipeCard, TaskTypes__Enum::ClearAsteroids,
		TaskTypes__Enum::UploadData, TaskTypes__Enum::InspectSample, TaskTypes__Enum::EmptyGarbage,
		TaskTypes__Enum::AlignEngineOutput, TaskTypes__Enum::FixWiring, TaskTypes__Enum::UnlockManifolds,
		TaskTypes__Enum::StoreArtifacts, TaskTypes__Enum::FillCanisters, TaskTypes__Enum::FixWeatherNode,
		TaskTypes__Enum::InsertKeys, TaskTypes__Enum::ScanBoardingPass, TaskTypes__Enum::OpenWaterways,
		TaskTypes__Enum::ReplaceWaterJug, TaskTypes__Enum::RepairDrill, TaskTypes__Enum::AlignTelescope,
		TaskTypes__Enum::RecordTemperature,
	};
	static const TaskList airshipTasks = {
		TaskTypes__Enum::FuelEngines, TaskTypes__Enum::UploadData, TaskTypes__Enum::EmptyChute,
		TaskTypes__Enum::EmptyGarbage, TaskTypes__Enum::FixWiring, TaskTypes__Enum::CalibrateDistributor,
		TaskTypes__Enum::DivertPower, TaskTypes__Enum::StabilizeSteering, TaskTypes__Enum::PolishRuby,
		TaskTypes__Enum::ResetBreakers, TaskTypes__Enum::Decontaminate, TaskTypes__Enum::MakeBurger,
		TaskTypes__Enum::UnlockSafe, TaskTypes__Enum::SortRecords, TaskTypes__Enum::PutAwayPistols,
		TaskTypes__Enum::FixShower, TaskTypes__Enum::CleanToilet, TaskTypes__Enum::DressMannequin,
		TaskTypes__Enum::PickUpTowels, TaskTypes__Enum::RewindTapes, TaskTypes__Enum::StartFans,
		TaskTypes__Enum::DevelopPhotos, TaskTypes__Enum::GetBiggolSword, TaskTypes__Enum::PutAwayRifles,
		TaskTypes__Enum::StopCharles, TaskTypes__Enum::VentCleaning,
	};
	static const TaskList fungleTasks = {
		TaskTypes__Enum::UploadData, TaskTypes__Enum::FixWiring, TaskTypes__Enum::VentCleaning,
		TaskTypes__Enum::RecordTemperature, TaskTypes__Enum::BuildSandcastle, TaskTypes__Enum::CatchFish,
		TaskTypes__Enum::CollectShells, TaskTypes__Enum::LiftWeights, TaskTypes__Enum::RoastMarshmallow,
		TaskTypes__Enum::TestFrisbee, TaskTypes__Enum::CollectSamples, TaskTypes__Enum::CollectVegetables,
		TaskTypes__Enum::HoistSupplies, TaskTypes__Enum::MineOres, TaskTypes__Enum::PolishGem,
		TaskTypes__Enum::ReplaceParts, TaskTypes__Enum::CrankGenerator, TaskTypes__Enum::TuneRadio,
		TaskTypes__Enum::ExtractFuel, TaskTypes__Enum::MonitorMushroom, TaskTypes__Enum::PlayVideogame,
	};

	static void RenderTaskEnforcer() {
		if (ImGui::CollapsingHeader("Task Enforcer", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
			if (ToggleButton("Auto Kick Slackers", &State.AutoKickSlackers))
				State.Save();
			ImGui::SameLine();
			if (ToggleButton("Ignore Whitelisted Players", &State.AutoKickSlackersIgnoreWhitelist))
				State.Save();
			ImGui::Text("Kicks players below task threshold after grace period.");
			if (SliderIntV2("Task Threshold %", &State.AutoKickSlackersThreshold, 1, 100, "%d%%", ImGuiSliderFlags_NoInput))
				State.Save();
			if (SliderIntV2("Grace Period (sec)", &State.AutoKickSlackersGrace, 50, 500, "%ds", ImGuiSliderFlags_NoInput))
				State.Save();
		}
	}

	static void RenderDisableTasks() {
		if (ImGui::CollapsingHeader("Disable Tasks", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::TextDisabled("Disabled tasks won't be assigned next game.");
			ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
			if (AnimatedButton("Clear All##disabletasks"))
				State.DisabledTaskTypes.clear();
			ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);

			int mapId = GameOptions().GetByte(app::ByteOptionNames__Enum::MapId);
			const TaskList* currentTasks = &skeldTasks;
			const char* mapName = "The Skeld";
			switch (mapId) {
			case 1: currentTasks = &miraHqTasks; mapName = "Mira HQ"; break;
			case 2: currentTasks = &polusTasks; mapName = "Polus"; break;
			case 4: currentTasks = &airshipTasks; mapName = "Airship"; break;
			case 5: currentTasks = &fungleTasks; mapName = "Fungle"; break;
			default: break;
			}

			ImGui::TextDisabled("Map: %s", mapName);
			ImGui::Dummy(ImVec2(3, 3) * State.dpiScale);
			ImVec4 themeCol = State.RgbMenuTheme ? State.RgbColor : (State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor);
			ImVec4 themeColDark = ImVec4(themeCol.x * 0.7f, themeCol.y * 0.7f, themeCol.z * 0.7f, themeCol.w);
			ImVec4 themeColDarker = ImVec4(themeCol.x * 0.5f, themeCol.y * 0.5f, themeCol.z * 0.5f, themeCol.w);

			ImGui::Columns(2, "disabledTasksCols", false);
			for (auto taskType : *currentTasks) {
				int id = (int)taskType;
				const char* name = TranslateTaskTypes(taskType);
				bool disabled = State.DisabledTaskTypes.count(id) > 0;
				ImGui::PushStyleColor(ImGuiCol_Button, disabled ? themeCol : ImVec4(0.f, 0.f, 0.f, 0.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, disabled ? themeColDarker : themeColDark);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, themeCol);
				if (AnimatedButton((std::string(name) + "##" + std::to_string(id)).c_str())) {
					if (disabled) State.DisabledTaskTypes.erase(id);
					else State.DisabledTaskTypes.insert(id);
				}
				ImGui::PopStyleColor(3);
				ImGui::NextColumn();
			}
			ImGui::Columns(1);
		}
	}

	void Render() {
		bool inGameWithTasks = IsInGame() && GetPlayerData(*Game::pLocalPlayer)->fields.Tasks != NULL;
		bool lobbyHost = IsInLobby() && IsHost();

		if (!inGameWithTasks && !lobbyHost) return;

		ImGui::SameLine(100 * State.dpiScale);
		ImGui::BeginChild("###Tasks", ImVec2(500 * State.dpiScale, 0), true, ImGuiWindowFlags_NoBackground);
		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

		if (inGameWithTasks) {
			auto tasks = GetNormalPlayerTasks(*Game::pLocalPlayer);

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
			if (!State.SafeMode) ImGui::SameLine();
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
				bool clientSide = (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek);
				if (AnimatedButton(clientSide ? "Play Shields Animation (Client-sided)" : "Play Shields Animation")) {
					State.rpcQueue.push(new RpcPlayAnimation(1));
				}
			}

			if (State.mapType == Settings::MapType::Ship) {
				bool clientSide = (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek);
				if (AnimatedButton(clientSide ? "Play Trash Animation (Client-sided)" : "Play Trash Animation")) {
					State.rpcQueue.push(new RpcPlayAnimation(10));
				}
			}

			if (State.mapType == Settings::MapType::Ship || State.mapType == Settings::MapType::Pb) {
				bool clientSide = (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek);
				if (ToggleButton(clientSide ? "Play Weapons Animation (Client-sided)" : "Play Weapons Animation", &State.PlayWeaponsAnimation)) {
					State.Save();
				}
			}

			bool clientSide = (!State.BypassVisualTasks && (options.GetGameMode() == GameModes__Enum::Normal && !options.GetBool(app::BoolOptionNames__Enum::VisualTasks)) || options.GetGameMode() == GameModes__Enum::HideNSeek);
			if (ToggleButton(clientSide ? "Play Medbay Scan Animation (Client-sided)" : "Play Medbay Scan Animation", &State.PlayMedbayScan)) {
				if (State.PlayMedbayScan) State.rpcQueue.push(new RpcSetScanner(true));
				else State.rpcQueue.push(new RpcSetScanner(false));
			}

			if (!(State.mapType == Settings::MapType::Hq || State.mapType == Settings::MapType::Fungle) && ToggleButton("Fake Cameras In Use", &State.FakeCameraUsage)) {
				State.rpcQueue.push(new RpcUpdateSystem(SystemTypes__Enum::Security, (State.FakeCameraUsage ? 1 : 0)));
			}

			if (IsInMultiplayerGame()) {
				float taskPercentage = (float)(*Game::pGameData)->fields.CompletedTasks / (float)(*Game::pGameData)->fields.TotalTasks;
				ImGui::TextColored(ImVec4(1.0f - taskPercentage, 1.0f, 1.0f - taskPercentage, 1.0f), "%.2f%% Total Tasks Completed", taskPercentage * 100);
			}
		}

		if (IsHost()) {
			if (inGameWithTasks) {
				ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
				ImGui::Separator();
				ImGui::Dummy(ImVec2(7, 7) * State.dpiScale);
			}
			RenderTaskEnforcer();

			if (lobbyHost) {
				ImGui::Dummy(ImVec2(5, 5) * State.dpiScale);
				RenderDisableTasks();
			}
		}

		ImGui::EndChild();
	}
}