#include "pch-il2cpp.h"
#include "debug_tab.h"
#include "imgui/imgui.h"
#include "state.hpp"
#include "main.h"
#include "game.h"
#include "profiler.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include "gui-helpers.hpp"
#include "translations.hpp"

namespace DebugTab {

	void Render() {
		ImGui::SameLine(100 * State.dpiScale);
		ImGui::BeginChild("###Debug", ImVec2(500, 0) * State.dpiScale, true, ImGuiWindowFlags_NoBackground);
		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
#ifndef _VERSION
		if (AnimatedButton("Unload DLL"))
		{
			SetEvent(hUnloadEvent);
		}
		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);
#endif
		ToggleButton("Enable Occlusion Culling", &State.OcclusionCulling);
		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

		if (AnimatedButton("Force Load Settings"))
		{
			State.Load();
		}
		if (AnimatedButton("Force Save Settings"))
		{
			State.Save();
		}
		if (AnimatedButton("Clear RPC Queues"))
		{
			State.rpcQueue = std::queue<RPCInterface*>();
			State.lobbyRpcQueue = std::queue<RPCInterface*>();
		}

		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

		if (ToggleButton("Log Unity Debug Messages", &State.ShowUnityLogs)) State.Save();
		if (ToggleButton("Log Hook Debug Messages", &State.ShowHookLogs)) State.Save();

		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

		if (ImGui::CollapsingHeader(T_("Experiments##debug"))) {
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), T_("These features are in development and can break at any time."));
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), T_("Use these at your own risk."));
			if (ToggleButton("Point System (Only for Hosting)", &State.TournamentMode)) State.Save();
			if (ToggleButton("April Fools' Mode", &State.AprilFoolsMode)) State.Save();
			/*static float timer = 0.0f;
			static bool SafeModeNotification = false;*/
			static bool safeModeWarnState = false;

			if (!safeModeWarnState && ToggleButton("Safe Mode", &State.SafeMode)) {
				if (!State.SafeMode) {
					safeModeWarnState = true;
					State.SafeMode = true;
				}
				/*SafeModeNotification = true;
				timer = static_cast<float>(ImGui::GetTime());*/
			}

			if (safeModeWarnState) {
				BoldText("Warning", ImVec4(1.f, 0.f, 0.f, 1.f));
				ImGui::Text(T_("By turning off Safe Mode, you can unlock functions"));
				ImGui::Text(T_("that are usually detected by the anticheat."));
				ImGui::Text(T_(" "));
				ImGui::Text(T_("However, you NEED to ensure that the lobby host has a reduced"));
				ImGui::Text(T_("anticheat (host authority), so the other functions work."));
				ImGui::Text(T_(" "));
				ImGui::Text(T_("Otherwise, you will get banned from the lobby by the anticheat!"));
				ImGui::Text(T_("NOTE: The developers will NOT be held responsible for this."));
				ImGui::Text(T_(" "));
				ImGui::Text(T_("Are you sure that you want to turn it off?"));

				if (ColoredButton(ImVec4(0.f, 1.f, 0.f, 1.f), "Yes")) {
					safeModeWarnState = false;
					State.SafeMode = false;
				}
				ImGui::SameLine();
				if (ColoredButton(ImVec4(1.f, 0.f, 0.f, 1.f), "No")) {
					safeModeWarnState = false;
				}
			}

			/*if (SafeModeNotification) {
				float currentTime = static_cast<float>(ImGui::GetTime());

				if (currentTime - timer < 5.0f) {
					ImGui::SameLine();
					if (State.SafeMode)
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Safe Mode is Enabled!");
					else
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Safe Mode is Disabled! (The likelihood of getting banned increases)");
				}
				else {
					SafeModeNotification = false;
				}
			}*/

			// ImGui::Text("Keep safe mode on in official servers (NA, Europe, Asia) to prevent anticheat detection!");
		}

		ImGui::Dummy(ImVec2(4, 4) * State.dpiScale);

		if (ImGui::CollapsingHeader(T_("Replay##debug")))
		{
			synchronized(Replay::replayEventMutex) {
				size_t numWalkPoints = 0;
				for (const auto& pair : State.replayWalkPolylineByPlayer) {
					numWalkPoints += pair.second.pendingPoints.size() + pair.second.simplifiedPoints.size();
				}
				ImGui::Text(T_("Num Walk Points: %d"), numWalkPoints);
				ImGui::Text(T_("Num Live Replay Events: %d"), State.liveReplayEvents.size());
				ImGui::Text(T_("Num Live Console Events: %d"), State.liveConsoleEvents.size());
			}

			ImGui::Text(T_("ReplayMatchStart: %s"), std::format("{:%OH:%OM:%OS}", State.MatchStart).c_str());
			ImGui::Text(T_("ReplayMatchCurrent: %s"), std::format("{:%OH:%OM:%OS}", State.MatchCurrent).c_str());
			ImGui::Text(T_("ReplayMatchLive: %s"), std::format("{:%OH:%OM:%OS}", std::chrono::system_clock::now()).c_str());
			ImGui::Text(T_("ReplayIsLive: %s"), (State.Replay_IsLive) ? "True" : "False");
			ImGui::Text(T_("ReplayIsPlaying: %s"), (State.Replay_IsPlaying) ? "True" : "False");

			if (AnimatedButton("Re-simplify polylines (check console)"))
			{
				SYNCHRONIZED(Replay::replayEventMutex);
				for (auto& playerPolylinePair : State.replayWalkPolylineByPlayer)
				{
					std::vector<ImVec2> resimplifiedPoints;
					std::vector<std::chrono::system_clock::time_point> resimplifiedTimeStamps;
					Replay::WalkEvent_LineData& plrLineData = playerPolylinePair.second;
					size_t numOldSimpPoints = plrLineData.simplifiedPoints.size();
					DoPolylineSimplification(plrLineData.simplifiedPoints, plrLineData.simplifiedTimeStamps, resimplifiedPoints, resimplifiedTimeStamps, 50.f, false);
					STREAM_DEBUG("Player[" << playerPolylinePair.first << "]: Re-simplification could reduce " << numOldSimpPoints << " points to " << resimplifiedPoints.size());
				}
			}
		}

		if (ImGui::CollapsingHeader(T_("Colors##debug")))
		{
			il2cpp::Array colArr = app::Palette__TypeInfo->static_fields->PlayerColors;
			auto colArr_raw = colArr.begin();
			size_t length = colArr.size();
			for (size_t i = 0; i < length; i++)
			{
				const app::Color32& col = colArr_raw[i];
				const ImVec4& conv_col = AmongUsColorToImVec4(col);
				static constexpr std::array COLORS = { "Red", "Blue", "Green", "Pink", "Orange", "Yellow", "Black", "White", "Purple", "Brown", "Cyan", "Lime", "Maroon", "Rose", "Banana", "Gray", "Tan", "Coral", "Fortegreen" };
				ImGui::TextColored(conv_col, "%s [%d]: (%d, %d, %d, %d)", COLORS.at(i), i, col.r, col.g, col.b, col.a);
			}
		}

		if (ImGui::CollapsingHeader(T_("Profiler##debug")))
		{
			if (AnimatedButton("Clear Stats"))
			{
				Profiler::ClearStats();
			}

			std::stringstream statStream;
			Profiler::AppendStatStringStream("WalkEventCreation", statStream);
			Profiler::AppendStatStringStream("ReplayRender", statStream);
			Profiler::AppendStatStringStream("ReplayPolyline", statStream);
			Profiler::AppendStatStringStream("PolylineSimplification", statStream);
			Profiler::AppendStatStringStream("ReplayPlayerIcons", statStream);
			Profiler::AppendStatStringStream("ReplayEventIcons", statStream);
			// NOTE:
			// can also just do this to dump all stats, but i like doing them individually so i can control the order better:
			// Profiler::WriteStatsToStream(statStream);

			ImGui::TextUnformatted(statStream.str().c_str());
		}

		{ std::string activeScene = State.CurrentScene; ImGui::Text(std::vformat(T_("Active Scene: {}"), std::make_format_args(activeScene)).c_str()); }

		{ int fps = GetFps(); ImGui::Text(std::vformat(T_("Current FPS: {}"), std::make_format_args(fps)).c_str()); }

		ImGui::EndChild();
	}
}