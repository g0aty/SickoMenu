#include "pch-il2cpp.h"
#include "esp.hpp"
#include "DirectX.h"
#include "utility.h"
#include "game.h"
#include "gui-helpers.hpp"

drawing_t* Esp::s_Instance = new drawing_t();
ImGuiWindow* CurrentWindow = nullptr;

static void RenderText(std::string_view text, const ImVec2& pos, const ImVec4& color, const bool outlined = true, const bool centered = true)
{
	if (text.empty() || State.PanicMode || color.w == 0.f) return;
	ImVec2 ImScreen = pos;
	auto size = ImGui::CalcTextSize(text.data(), text.data() + text.length()) * (State.ShowEsp_TextSize / State.dpiScale);
	if (centered)
	{
		ImScreen.x -= size.x * 0.5f * State.ShowEsp_TextSize;
		ImScreen.y -= size.y;
	}

	if (outlined)
	{
		CurrentWindow->DrawList->AddText(nullptr, size.y * State.ShowEsp_TextSize,
			ImScreen + 0.5f * State.ShowEsp_TextSize,
			ImGui::GetColorU32(IM_COL32_BLACK), text.data(), text.data() + text.length());
	}

	CurrentWindow->DrawList->AddText(nullptr, size.y * State.ShowEsp_TextSize, ImScreen, ImGui::GetColorU32(color), text.data(), text.data() + text.length());
}

static void RenderLine(const ImVec2& start, const ImVec2& end, const ImVec4& color, bool shadow = false) noexcept
{
	if (State.PanicMode) return;

	if (shadow)
	{
		CurrentWindow->DrawList->AddLine(
			start + 0.5f * State.ShowEsp_LineThickness,
			end + 0.5f * State.ShowEsp_LineThickness,
			ImGui::GetColorU32(color) & IM_COL32_A_MASK, State.ShowEsp_LineThickness);
	}

	CurrentWindow->DrawList->AddLine(start, end, ImGui::GetColorU32(color), State.ShowEsp_LineThickness);
}

static void RenderBox(const ImVec2& top, const ImVec2& bottom, const float height, const float width, const ImVec4& color, const bool wantsShadow = true)
{
	if (State.PanicMode) return;

	const ImVec2 points[] = {
		bottom, { bottom.x, ((float)(int)(bottom.y * 0.75f + top.y * 0.25f)) },
		{ bottom.x - 0.5f * State.ShowEsp_LineThickness, bottom.y }, { ((float)(int)(bottom.x * 0.75f + top.x * 0.25f)), bottom.y },

		{ top.x + 0.5f * State.ShowEsp_LineThickness, bottom.y }, { ((float)(int)(top.x * 0.75f + bottom.x * 0.25f)), bottom.y },
		{ top.x, bottom.y }, { top.x, ((float)(int)(bottom.y * 0.75f + top.y * 0.25f)) },

		{ bottom.x, top.y }, { bottom.x, ((float)(int)(top.y * 0.75f + bottom.y * 0.25f)) },
		{ bottom.x - 0.5f * State.ShowEsp_LineThickness, top.y }, { ((float)(int)(bottom.x * 0.75f + top.x * 0.25f)), top.y },

		top, { ((float)(int)(top.x * 0.75f + bottom.x * 0.25f)), top.y },
		{ top.x, top.y + 0.5f * State.ShowEsp_LineThickness }, { top.x, ((float)(int)(top.y * 0.75f + bottom.y * 0.25f)) }
	};

	if (wantsShadow) {
		const ImVec4& shadowColor = ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(color) & IM_COL32_A_MASK);
		for (size_t i = 0; i < std::size(points); i += 2) {
			RenderLine(points[i] + 0.5f * State.ShowEsp_LineThickness, points[i + 1] + 0.5f * State.ShowEsp_LineThickness, shadowColor, false);
		}
	}
	for (size_t i = 0; i < std::size(points); i += 2) {
		RenderLine(points[i], points[i + 1], color, false);
	}
}

static void DrawEsp(drawing_t& instance, EspPlayerData& pd, bool isDeadBody = false) {
	/////////////////////////////////
	//// Box ////////////////////////
	/////////////////////////////////
	if (State.ShowEsp_Box)
	{
		float width = isDeadBody ? GetScaleFromValue(50.f) : GetScaleFromValue(35.0f);
		float height = isDeadBody ? GetScaleFromValue(100.f) : GetScaleFromValue(120.0f);
		float yOffset = isDeadBody ? GetScaleFromValue(50.f) : GetScaleFromValue(65.f);

		ImVec2 top{ pd.Position.x + width, pd.Position.y + yOffset };
		ImVec2 bottom{ pd.Position.x - width, pd.Position.y - height + yOffset };

		RenderBox(top, bottom, height, width, pd.Color, State.ShowEsp_LineTextShadows);
	}

	/////////////////////////////////
	//// Tracers ////////////////////
	/////////////////////////////////
	if (State.ShowEsp_Tracers)
	{
		RenderLine(instance.LocalPosition, pd.Position, pd.Color, State.ShowEsp_LineTextShadows);
	}

	/////////////////////////////////
	//// Distance ///////////////////
	/////////////////////////////////
	if (State.ShowEsp_Distance)
	{
		// logic and calculation
		ImVec2 localPosition = instance.LocalPosition;

		// infamous trash codes
		float xOffset = (DirectX::GetWindowSize(true).x - DirectX::GetWindowSize().x) / 2.f;
		float yOffset = (DirectX::GetWindowSize(true).y - DirectX::GetWindowSize().y) / 2.f;

		float offsetFactor = State.ShowEsp_TextSize * (0.75f * State.ShowEsp_TextSize + 0.25f);
		// don't ask me why or how this offset works, but it does

		float minX = 40.0f * offsetFactor + xOffset, minY = yOffset,
			maxX = DirectX::GetWindowSize().x - 40.0f * offsetFactor + xOffset, // 1320
			maxY = DirectX::GetWindowSize().y - 40.0f * offsetFactor + yOffset; // 730

		float x = pd.Position.x, y = pd.Position.y - 60.f * (State.ShowEsp_TextSize - 1.f);
		float offset = 15.0f * State.ShowEsp_TextSize;

		float delX = x - localPosition.x, delY = y - localPosition.y;
		float dist = (std::sqrt)(delX * delX + delY * delY);
		if (dist < 1e-3f) dist = 1.f; // avoid zero division

		float dirX = delX / dist, dirY = delY / dist;

		float maxDist = dist;
		if (dirX > 0) maxDist = (std::min)(maxDist, (maxX - localPosition.x) / dirX);
		if (dirX < 0) maxDist = (std::min)(maxDist, (minX - localPosition.x) / dirX);
		if (dirY > 0) maxDist = (std::min)(maxDist, (maxY - localPosition.y) / dirY);
		if (dirY < 0) maxDist = (std::min)(maxDist, (minY - localPosition.y) / dirY);

		x = localPosition.x + dirX * maxDist;
		y = localPosition.y + dirY * maxDist;

		/*if (x < minX) {
			x = minX;
		}
		else if (x > maxX) {
			x = maxX;
		}

		if (y < minY) {
			y = minY;
		}
		else if (y > maxY) {
			y = maxY;
		}*/

		ImVec2 position = { x, y + offset };
		ImVec2 position2 = { x, y + 2 * (float)std::sqrt(State.ShowEsp_TextSize) * offset };
		// once again, don't ask me why it's multiplied by the square root, it offsets correctly


		char distance[32];
		sprintf_s(distance, "[%.1f m]", pd.Distance);

		if (!isDeadBody) {
			std::string lol = pd.Name;
			char* player = lol.data();
			RenderText(player, position, pd.Color, State.ShowEsp_LineTextShadows);

			// kill cd update
			GameOptions options;
			if (const auto& player = pd.playerData.validate();
				State.ShowKillCD
				&& !player.get_PlayerData()->fields.IsDead
				&& player.get_PlayerData()->fields.Role
				&& player.get_PlayerData()->fields.Role->fields.CanUseKillButton
				) {
				float killTimer = player.get_PlayerControl()->fields.killTimer;
				sprintf_s(distance, "[%.2f s]", killTimer);
			}
		}

		//std::string lol2 = std::to_string(it.Position.x) + ", " + std::to_string(it.Position.y);
		//char* pl = lol2.data();

		// render info
		RenderText(distance, isDeadBody ? position : position2, pd.Color, State.ShowEsp_LineTextShadows);
	}
}

void Esp::Render()
{
	if (State.PanicMode) return;

	CurrentWindow = ImGui::GetCurrentWindow();

	drawing_t& instance = Esp::GetDrawing();

	// Lock our mutex when we render (this will unlock when it goes out of scope)
	synchronized(instance.m_DrawingMutex) {
		// track player codes
		for (auto& it : instance.m_Players)
		{
			if (const auto& player = it.playerData.validate();
				player.has_value()						//Verify PlayerControl hasn't been destroyed (happens when disconnected)
				&& !player.is_Disconnected()		//Sanity check, shouldn't ever be true
				&& !player.is_LocalPlayer()			//Don't highlight yourself, you're ugly
				&& ( (player.get_PlayerData()->fields.IsDead && State.ShowEsp_Ghosts)
				|| (!player.get_PlayerData()->fields.IsDead && State.ShowEsp_Players) )
				&& it.OnScreen)
			{
				DrawEsp(instance, it);
			}
		}

		for (auto deadBody : GetAllDeadBodies()) {
			auto playerId = deadBody->fields.ParentId;
			auto playerData = GetPlayerDataById(playerId);

			if (std::find(State.validDeadBodyIds.begin(), State.validDeadBodyIds.end(), playerId) == State.validDeadBodyIds.end())
				continue; // the dead body has been fully dissolved or invalid in this case, don't render it

			EspPlayerData deadBodyData;

			Vector2 bodyPos = app::DeadBody_get_TruePosition(deadBody, NULL);
			bodyPos.x -= 0.15f;
			bodyPos.y -= 0.1f;
			deadBodyData.Position = WorldToScreen(bodyPos);

			deadBodyData.Color = ImVec4(0.f, 0.f, 0.f, 0.f);
			bool shouldShowRoleBased = (PlayerIsImpostor(playerData) && State.ShowEsp_Imp) ||
				(!PlayerIsImpostor(playerData) && State.ShowEsp_Crew);

			if (shouldShowRoleBased) {
				if (State.ShowEsp_RoleBased) {
					deadBodyData.Color = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
				}
				else if (auto outfit = GetPlayerOutfit(playerData); outfit != NULL) {
					deadBodyData.Color = AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId));
				}
			}

			deadBodyData.Distance = Vector2_Distance(GetTrueAdjustedPosition(*Game::pLocalPlayer), bodyPos, nullptr);
			deadBodyData.OnScreen = IsWithinScreenBounds(bodyPos);

			if (State.ShowEsp_DeadBodies) DrawEsp(instance, deadBodyData, true);
		}
	}
}