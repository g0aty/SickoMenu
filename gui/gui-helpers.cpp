#include "pch-il2cpp.h"
#include "gui-helpers.hpp"
#include "keybinds.h"
#include "state.hpp"
#include "game.h"
#include "logger.h"
#include "DirectX.h"

using namespace ImGui;

bool CustomListBoxInt(const char* label, int* value, const std::vector<const char*> list, float width, ImVec4 col, ImGuiComboFlags flags, const char* visualLabel) {
	auto comboLabel = "##" + std::string(label);
	auto leftArrow = "##" + std::string(label) + "Left";
	auto rightArrow = "##" + std::string(label) + "Right";

	ImGuiStyle& style = GetStyle();
	float spacing = style.ItemInnerSpacing.x;
	PushItemWidth(width);
	bool response = BeginCombo(comboLabel.c_str(), (*value >= 0 ? list.at(*value) : nullptr), ImGuiComboFlags_NoArrowButton | flags);
	if (response) {
		response = false;
		for (size_t i = 0; i < list.size(); i++) {
			bool is_selected = (*value == i);
			if (Selectable(list.at(i), is_selected)) {
				*value = (int)i;
				response = true;
			}
			if (is_selected)
				SetItemDefaultFocus();
		}
		EndCombo();
	}

	PopItemWidth();
	SameLine(0, spacing);

	const bool LeftResponse = ArrowButton(leftArrow.c_str(), ImGuiDir_Left);
	if (LeftResponse) {
		*value -= 1;
		if (*value < 0) *value = int(list.size() - 1);
		return LeftResponse;
	}
	SameLine(0, spacing);
	const bool RightResponse = ArrowButton(rightArrow.c_str(), ImGuiDir_Right);
	if (RightResponse) {
		*value += 1;
		if (*value > (int)(list.size() - 1)) *value = 0;
		return RightResponse;
	}
	SameLine(0, spacing);
	//noobuild by gdjkhp
	if (col.x == 0 && col.y == 0 && col.z == 0 && col.w == 0) Text(visualLabel == "" ? label : visualLabel);
	else TextColored(col, visualLabel == "" ? label : visualLabel);

	return response;
}

bool CustomListBoxIntMultiple(const char* label, std::vector<std::pair<const char*, bool>>* list, float width, bool resetButton, ImGuiComboFlags flags) {
	auto comboLabel = "##" + std::string(label);
	auto buttonLabel = "Reset##" + std::string(label);
	ImGuiStyle& style = GetStyle();
	float spacing = style.ItemInnerSpacing.x;
	PushItemWidth(width);
	size_t countSelected = 0;
	for (auto& pair : *list) {
		if (pair.second) {
			countSelected++;
		}
	}
	std::string preview;
	if (countSelected > 0) {
		char buf[32] = { 0 };
		sprintf_s(buf, "%zu item(s) selected", countSelected);
		preview = buf;
	}
	else
		preview = label;
	bool response = BeginCombo(comboLabel.c_str(), preview.c_str(), flags);
	if (response) {
		response = false;
		for (auto& pair : *list) {
			if (strcmp(pair.first, "") == 0) // ignore all entries with empty labels so we can create padding
				continue;
			if (Selectable(pair.first, pair.second)) {
				pair.second ^= 1;
				response = true;
			}
			if (pair.second)
				SetItemDefaultFocus();
		}
		EndCombo();
	}

	PopItemWidth();

	if (resetButton)
	{
		SameLine(0, spacing);
		const bool resetResponse = Button(buttonLabel.c_str());
		if (resetResponse) {
			for (auto& pair : *list)
				pair.second = false;
			return resetResponse;
		}
	}

	return response;
}

bool CustomListBoxPlayerSelectionMultiple(const char* label, std::array<std::pair<PlayerSelection, bool>, Game::MAX_PLAYERS>* list, float width, bool resetButton, ImGuiComboFlags flags) {
	if (!IsInGame()) return false; // works only ingame

	auto comboLabel = "##" + std::string(label);
	auto buttonLabel = "Reset##" + std::string(label);
	ImGuiStyle& style = GetStyle();
	float spacing = style.ItemInnerSpacing.x;
	PushItemWidth(width);
	size_t countSelected = 0;
	for (auto& pair : *list) {
		if (pair.second) {
			countSelected++;
		}
	}
	std::string preview;
	if (countSelected > 0) {
		char buf[32] = { 0 };
		sprintf_s(buf, "%zu player(s) selected", countSelected);
		preview = buf;
	}
	else
		preview = label;
	bool response = BeginCombo(comboLabel.c_str(), preview.c_str(), flags);
	if (response) {
		response = false;
		auto localData = GetPlayerData(*Game::pLocalPlayer);
		for (auto playerData : GetAllPlayerData()) {
			auto playerSelection = PlayerSelection(playerData);
			const auto& player = playerSelection.validate();
			if (!player.has_value())
				continue;
			if (player.is_Disconnected()) // maybe make that an option for replays ? (parameter based on "state.showDisconnected" related data)
				continue;

			auto outfit = GetPlayerOutfit(playerData);
			if (outfit == NULL) return false;
			auto& item = list->at(playerData->fields.PlayerId);
			std::string playerName = convert_from_string(outfit->fields.PlayerName);
			PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0) * State.dpiScale);
			PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0) * State.dpiScale);
			if (Selectable(std::string("##" + playerName + "_ConsoleName").c_str(), item.second))
			{
				item.second ^= 1;
				if (item.second)
				{
					if (const auto& result = item.first.validate();
						!result.has_value() || result.is_Disconnected())
						item.first = playerSelection;
				}
				response = true;
			}
			if (item.second)
				SetItemDefaultFocus();
			SameLine();
			ColorButton(std::string("##" + playerName + "_ConsoleColorButton").c_str(), AmongUsColorToImVec4(GetPlayerColor(outfit->fields.ColorId)), ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_NoTooltip);
			SameLine();
			PopStyleVar(2);
			Dummy(ImVec2(0, 0) * State.dpiScale);
			SameLine();

			ImVec4 nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->White);
			if (State.RevealRoles)
			{
				std::string roleName = GetRoleName(playerData->fields.Role, State.AbbreviatedRoleNames);
				playerName = playerName + " (" + roleName + ")";
				nameColor = AmongUsColorToImVec4(GetRoleColor(playerData->fields.Role));
			}
			else if (PlayerIsImpostor(localData) && PlayerIsImpostor(playerData))
				nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->ImpostorRed);
			//else if (PlayerSelection(playerData).is_LocalPlayer() || std::count(State.aumUsers.begin(), State.aumUsers.end(), playerData->fields.PlayerId))
				//nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->Orange);

			if (playerData->fields.IsDead)
				nameColor = AmongUsColorToImVec4(Palette__TypeInfo->static_fields->DisabledGrey);

			TextColored(nameColor, playerName.c_str());
		}
		EndCombo();
	}

	PopItemWidth();

	if (resetButton)
	{
		SameLine(0, spacing);
		const bool resetResponse = Button(buttonLabel.c_str());
		if (resetResponse) {
			for (size_t i = 0; i < list->size(); i++)
				list->at(i).second = false;
			return resetResponse;
		}
	}

	return response;
}

bool SliderChrono(const char* label, void* p_data, const void* p_min, const void* p_max, std::string_view format, ImGuiSliderFlags flags)
{
	flags |= ImGuiSliderFlags_NoInput; // disable manual inputs by default

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	if (!State.Replay_IsLive && !State.Replay_IsPlaying) {
		const auto& playIcon = icons.at(ICON_TYPES::PLAY);
		const auto& iconSize = ImVec2((float)playIcon.iconImage.imageWidth, (float)playIcon.iconImage.imageHeight) * playIcon.scale;
		if (ImageButton((ImTextureID)playIcon.iconImage.shaderResourceView, iconSize))
		{
			State.Replay_IsPlaying = true;
		}
	}
	else {
		// Live or Playing
		const auto& pauseIcon = icons.at(ICON_TYPES::PAUSE);
		const auto& iconSize = ImVec2((float)pauseIcon.iconImage.imageWidth, (float)pauseIcon.iconImage.imageHeight) * pauseIcon.scale;
		if (ImageButton((ImTextureID)pauseIcon.iconImage.shaderResourceView, iconSize))
		{
			State.Replay_IsPlaying = State.Replay_IsLive = false;
		}
	}

	SameLine(0.0f * State.dpiScale, 1.0f * State.dpiScale);

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb))
		return false;

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool hovered = ItemHoverable(frame_bb, id);
	bool clicked = false;;
	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
	if (!temp_input_is_active)
	{
		const bool focus_requested = temp_input_allowed && FocusableItemRegister(window, id);
		clicked = (hovered && g.IO.MouseClicked[0]);
		if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
		{
			SetActiveID(id, window);
			SetFocusID(id, window);
			FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
			{
				temp_input_is_active = true;
				FocusableItemUnregister(window);
			}
		}
	}

	// Draw frame
	auto frameMin = ImVec2(frame_bb.Min.x + 7.f * State.dpiScale, frame_bb.Min.y + 4.f * State.dpiScale);

	const ImU32 frame_col = GetColorU32(g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	RenderNavHighlight(frame_bb, id);
	RenderFrame(frameMin, frame_bb.Max - 4.f * State.dpiScale, frame_col, true, g.Style.FrameRounding);

	// Slider behavior
	ImRect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, ImGuiDataType_S64, p_data, p_min, p_max, nullptr, flags | ImGuiSliderFlags_NoRoundToFormat, &grab_bb);
	if (value_changed) {
		MarkItemEdited(id);

		// check if new current timestamp is matching the live timestamp
		// this logic makes sure that we can switch between live and replay mode
		auto newMatchCurrent = std::chrono::time_point_cast<std::chrono::seconds>(State.MatchCurrent);
		auto matchLiveMs = std::chrono::time_point_cast<std::chrono::seconds>(State.MatchLive);
		if (newMatchCurrent == matchLiveMs)
		{
			State.Replay_IsLive = true;
			State.Replay_IsPlaying = false;
		}
		else
		{
			State.Replay_IsLive = false;
		}
	}

	// Render grab
	if (grab_bb.Max.x > grab_bb.Min.x) {
		window->DrawList->AddCircleFilled((grab_bb.Min + grab_bb.Max) / 2, 8 * State.dpiScale, GetColorU32(ImGuiCol_SliderGrabActive));
		if (&p_data != &p_min) {
			ImVec2 maxActive = ImVec2(grab_bb.Max.x - 14 * State.dpiScale, grab_bb.Max.y - 2.f * State.dpiScale);
			RenderFrame(frameMin, maxActive, GetColorU32(ImGuiCol_ButtonActive), true, g.Style.FrameRounding);
		}
	}
		//window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

	// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	RenderTextClipped(frame_bb.Min, frame_bb.Max, format.data(), format.data() + format.length(), NULL, ImVec2(0.5f, 0.5f));

	if (label_size.x > 0.0f)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	SameLine(0.0f * State.dpiScale, 10.0f * State.dpiScale);

	ImU32 liveColor = (State.Replay_IsLive) ? ColorConvertFloat4ToU32(ImVec4(255.0f, 0.f, 0.f, 255.0f)) : ColorConvertFloat4ToU32(ImVec4(128.f, 128.f, 128.f, 255.0f));
	const ImVec2 circlePos(window->DC.CursorPos.x, window->DC.CursorPos.y + 9.5f * State.dpiScale);
	window->DrawList->AddCircleFilled(circlePos, 5.0f * State.dpiScale, liveColor);
	SameLine(0.0f * State.dpiScale, 18.f * State.dpiScale);
	Text("Live");


	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);

	return value_changed;
}

bool HotKey(uint8_t& key)
{
	Text("[ %s ]", KeyBinds::ToString(key));

	if (IsItemHovered()) SetTooltip("Press any key while clicking on the keybind to change it, ESC to reset");

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) || !IsItemHovered())
		return false;

	for (uint8_t vKey : KeyBinds::GetValidKeys()) {
		if (KeyBinds::IsKeyDown(vKey)) {
			key = (vKey != VK_ESCAPE ? vKey : 0x00);
			State.Save();
			return true;
		}
	}

	return true;
}

void drawPlayerDot(PlayerControl* player, const ImVec2& winPos, ImU32 color, ImU32 statusColor)
{
	ImDrawList* drawList = GetWindowDrawList();

	Vector2 playerPos = app::PlayerControl_GetTruePosition(player, NULL);

	const auto& map = maps[(size_t)State.mapType];
	float xOffset = getMapXOffsetSkeld(map.x_offset) + (float)State.RadarExtraWidth;
	float yOffset = map.y_offset + (float)State.RadarExtraHeight;

	float radX = xOffset + (playerPos.x * map.scale);
	float radY = yOffset - (playerPos.y * map.scale);
	const ImVec2& center = ImVec2(radX, radY) * State.dpiScale + winPos;

	if ((State.RevealRoles || PlayerIsImpostor(GetPlayerData(*Game::pLocalPlayer))) && PlayerIsImpostor(GetPlayerData(player))) {
		ImVec2 topLeft = ImVec2(center.x - 4.5F * State.dpiScale, center.y - 4.5F * State.dpiScale);
		ImVec2 bottomRight = ImVec2(center.x + 4.5F * State.dpiScale, center.y + 4.5F * State.dpiScale);

		// Draw a filled rectangle
		drawList->AddRectFilled(topLeft, bottomRight, color, 1.f);

		// Draw a rectangle outline
		drawList->AddRect(topLeft, bottomRight, statusColor, 1.f, 15, 2.f);
	}
	else {
		drawList->AddCircleFilled(center, 4.5F * State.dpiScale, color);
		drawList->AddCircle(center, (4.5F + 0.5F) * State.dpiScale, statusColor, 0, 2.0F);
	}
}

void drawPlayerIcon(PlayerControl* player, const ImVec2& winPos, ImU32 color)
{
	ImDrawList* drawList = GetWindowDrawList();

	Vector2 playerPos = app::PlayerControl_GetTruePosition(player, NULL);

	const auto& map = maps[(size_t)State.mapType];
	float xOffset = getMapXOffsetSkeld(map.x_offset) + (float)State.RadarExtraWidth;
	float yOffset = map.y_offset + (float)State.RadarExtraHeight;

	IconTexture icon = icons.at(ICON_TYPES::PLAYER);
	IconTexture visor = icons.at(ICON_TYPES::PLAYERVISOR);
	float halfImageWidth = icon.iconImage.imageWidth * icon.scale * 0.5f, halfImageHeight = icon.iconImage.imageHeight * icon.scale * 0.5f;
	float radX = xOffset + (playerPos.x - halfImageWidth) * map.scale;
	float radY = yOffset - (playerPos.y - halfImageHeight) * map.scale;
	float radXMax = xOffset + (playerPos.x + halfImageWidth) * map.scale;
	float radYMax = yOffset - (playerPos.y + halfImageHeight) * map.scale;

	const ImVec2& p_min = ImVec2(radX, radY) * State.dpiScale + winPos;
	const ImVec2& p_max = ImVec2(radXMax, radYMax) * State.dpiScale + winPos;

	drawList->AddImage((void*)icon.iconImage.shaderResourceView,
		p_min, p_max,
		ImVec2(0.0f, 1.0f),
		ImVec2(1.0f, 0.0f),
		color);

	drawList->AddImage((void*)visor.iconImage.shaderResourceView,
		p_min, p_max,
		ImVec2(0.0f, 1.0f),
		ImVec2(1.0f, 0.0f),
		(/*State.RadarVisorRoleColor && */State.RevealRoles) ?
		GetColorU32(AmongUsColorToImVec4(GetRoleColor(GetPlayerData(player)->fields.Role))) :
		GetColorU32(AmongUsColorToImVec4(Palette__TypeInfo->static_fields->VisorColor)));

	if (GetPlayerData(player)->fields.IsDead)
		drawList->AddImage((void*)icons.at(ICON_TYPES::CROSS).iconImage.shaderResourceView,
			p_min, p_max,
			ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
}

void drawDeadPlayerDot(DeadBody* deadBody, const ImVec2& winPos, ImU32 color)
{
	ImDrawList* drawList = GetWindowDrawList();

	Vector2 bodyPos = app::DeadBody_get_TruePosition(deadBody, NULL);

	const auto& map = maps[(size_t)State.mapType];
	float xOffset = getMapXOffsetSkeld(map.x_offset) + (float)State.RadarExtraWidth;
	float yOffset = map.y_offset + (float)State.RadarExtraHeight;

	float radX = xOffset + (bodyPos.x * map.scale);
	float radY = yOffset - (bodyPos.y * map.scale);

	drawList->AddText(GetFont(), 16 * State.dpiScale,
		ImVec2(radX - 5.F, radY - 6.75F) * State.dpiScale + winPos, color, "X");
}

void drawDeadPlayerIcon(DeadBody* deadBody, const ImVec2& winPos, ImU32 color)
{
	ImDrawList* drawList = GetWindowDrawList();

	Vector2 bodyPos = app::DeadBody_get_TruePosition(deadBody, NULL);

	const auto& map = maps[(size_t)State.mapType];
	float xOffset = getMapXOffsetSkeld(map.x_offset) + (float)State.RadarExtraWidth;
	float yOffset = map.y_offset + (float)State.RadarExtraHeight;

	IconTexture icon = icons.at(ICON_TYPES::DEAD);
	float radX = xOffset + (bodyPos.x - (icon.iconImage.imageWidth * icon.scale * 0.5f)) * map.scale;
	float radY = yOffset - (bodyPos.y - (icon.iconImage.imageHeight * icon.scale * 0.5f)) * map.scale;
	float radXMax = xOffset + (bodyPos.x + (icon.iconImage.imageWidth * icon.scale * 0.5f)) * map.scale;
	float radYMax = yOffset - (bodyPos.y + (icon.iconImage.imageHeight * icon.scale * 0.5f)) * map.scale;

	drawList->AddImage((void*)icon.iconImage.shaderResourceView,
		ImVec2(radX, radY) * State.dpiScale + winPos,
		ImVec2(radXMax, radYMax) * State.dpiScale + winPos,
		ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), color);
}

struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool InputStringMultiline(const char* label, std::string* str, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool InputStringWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputTextWithHint(label, hint, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ToggleButton(const char* str_id, bool* v)
{
	ImVec4* colors = GetStyle().Colors;
	ImVec2 p = GetCursorScreenPos();
	ImDrawList* draw_list = GetWindowDrawList();

	float height = GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;
	InvisibleButton(str_id, ImVec2(width, height));
	bool result = false;
	if (IsItemClicked()) {
		*v = !*v;
		result = true;
	}

	if (IsItemHovered())
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), GetColorU32(*v ? colors[ImGuiCol_FrameBg] : colors[ImGuiCol_FrameBgActive]), height * 0.5f);
	else
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), GetColorU32(*v ? colors[ImGuiCol_FrameBgActive] : colors[ImGuiCol_FrameBg]), height * 0.50f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, GetColorU32(colors[ImGuiCol_CheckMark]));
	SameLine();
	Text(str_id);
	return result;
}

bool TabGroup(const char* label, bool highlight)
{
	static ImVec4 vec = State.MenuThemeColor;
	if (State.RgbMenuTheme)
		vec = State.RgbColor;
	else
		vec = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
	auto defaultCol = ImVec4((float)(vec.x / 1.25), (float)(vec.y / 1.25), (float)(vec.z / 1.25), 0.76f * State.MenuThemeColor.w);
	auto hoveredCol = ImVec4((float)(vec.x / 1.25), (float)(vec.y / 1.25), (float)(vec.z / 1.25), 0.86f * State.MenuThemeColor.w);
	auto activeCol = ImVec4((float)(vec.x / 1.25), (float)(vec.y / 1.25), (float)(vec.z / 1.25), 1.0f * State.MenuThemeColor.w);

	PushID(label);
	PushStyleColor(ImGuiCol_Button, highlight ? activeCol : defaultCol);
	PushStyleColor(ImGuiCol_ButtonHovered, hoveredCol);
	PushStyleColor(ImGuiCol_ButtonActive, activeCol);
	bool selected = Button(label);
	PopStyleColor(3);
	PopID();
	return selected;
}

bool ColoredButton(ImVec4 col, const char* label) {
	auto hoveredCol = ImVec4((float)(col.x / 1.25), (float)(col.y / 1.25), (float)(col.z / 1.25), (State.LightMode ? 0.3f : 0.86f) * State.MenuThemeColor.w);
	auto activeCol = ImVec4((float)(col.x / 1.25), (float)(col.y / 1.25), (float)(col.z / 1.25), (State.LightMode ? 0.4f : 1.f) * State.MenuThemeColor.w);
	PushStyleColor(ImGuiCol_Text, col);
	PushStyleColor(ImGuiCol_ButtonHovered, hoveredCol);
	PushStyleColor(ImGuiCol_ButtonActive, activeCol);
	bool ret = Button(label);
	PopStyleColor(3);
	return ret;
}

void BoldText(const char* text, ImVec4 col) {
	ImVec2 pos = GetCursorScreenPos();
	std::vector<std::pair<uint8_t, uint8_t>> vec = { {1, 1}, {1, 0}, {0, 1}, {0, 0} };
	for (std::pair<uint8_t, uint8_t> i : vec) {
		SetCursorScreenPos(ImVec2(pos.x + i.first, pos.y + i.second));
		if (col.x == 0.f && col.y == 0.f && col.z == 0.f && col.w == 0.f) Text(text);
		else TextColored(col, text);
	}
}

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
		return "%d";
	const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
		return g.TempBuffer;
#else
		IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
	}
	return fmt;
}

bool SliderScalarV2(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb))
		return false;

	// Default format string when passing NULL
	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;
	else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
		format = PatchFormatStringFloatToInt(format);

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool hovered = ItemHoverable(frame_bb, id);
	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
	if (!temp_input_is_active)
	{
		const bool focus_requested = temp_input_allowed && FocusableItemRegister(window, id);
		const bool clicked = (hovered && g.IO.MouseClicked[0]);
		if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
		{
			SetActiveID(id, window);
			SetFocusID(id, window);
			FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || g.NavInputId == id))
			{
				temp_input_is_active = true;
				FocusableItemUnregister(window);
			}
		}
	}

	if (temp_input_is_active)
	{
		// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
		const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
		return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
	}

	// Draw frame
	auto frameMin = ImVec2(frame_bb.Min.x + 7.f * State.dpiScale, frame_bb.Min.y + 4.f * State.dpiScale);

	const ImU32 frame_col = GetColorU32(g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	RenderNavHighlight(frame_bb, id);
	RenderFrame(frameMin, frame_bb.Max - 4.f * State.dpiScale, frame_col, true, g.Style.FrameRounding);

	// Slider behavior
	ImRect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
	if (value_changed)
		MarkItemEdited(id);

	// Render grab
	if (grab_bb.Max.x > grab_bb.Min.x) {
		window->DrawList->AddCircleFilled((grab_bb.Min + grab_bb.Max) / 2, 8 * State.dpiScale, GetColorU32(ImGuiCol_SliderGrabActive));
		if (&p_data != &p_min) {
			ImVec2 maxActive = ImVec2(grab_bb.Max.x - 14 * State.dpiScale, grab_bb.Max.y - 2.f * State.dpiScale);
			RenderFrame(frameMin, maxActive, GetColorU32(ImGuiCol_ButtonActive), true, g.Style.FrameRounding);
		}
	}

	// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
	RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

	if (label_size.x > 0.0f)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
	return value_changed;
}

bool SliderIntV2(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarV2(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool SteppedSliderFloat(const char* label, float* v, float v_min, float v_max, float v_step, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
	char text_buf[64] = {};
	ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), format, *v);

	const int stepCount = int((v_max - v_min) / v_step);
	int v_i = int((*v - v_min) / v_step);
	const bool valueChanged = SliderIntV2(label, &v_i, 0, stepCount, text_buf, 0);

	*v = v_min + float(v_i) * v_step;
	return valueChanged;
}