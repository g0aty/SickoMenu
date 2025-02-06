#include "pch-il2cpp.h"
#include "gui-helpers.hpp"
#include "keybinds.h"
#include "state.hpp"
#include "game.h"
#include "logger.h"
#include "DirectX.h"
#include <DirectX.h>
#include <sstream>
#include <string>
#include <unordered_set>
#include "imgui.h"


extern bool enableSpellCheck;


std::unordered_set<std::string> dictionary = {
    "the", "be", "to", "of", "and", "a", "in", "that", "have", "I", "it", "for", "not", "on", "with", "as", "you", "do", "at", 
    "this", "but", "his", "by", "from", "they", "we", "say", "her", "she", "or", "an", "will", "my", "one", "all", "would", 
    "there", "their", "what", "so", "up", "out", "if", "about", "who", "get", "which", "go", "me", "when", "make", "can", 
    "like", "time", "no", "just", "him", "know", "take", "person", "into", "year", "your", "good", "some", "could", "them", 
    "see", "other", "than", "then", "now", "look", "only", "come", "its", "over", "think", "also", "back", "after", "use", 
    "two", "how", "our", "work", "first", "well", "way", "even", "new", "want", "because", "any", "these", "give", "day", 
    "most", "us", "is", "are", "was", "were", "been", "being", "have", "has", "had", "having", "do", "does", "did", "doing", 
    "a", "an", "the", "more", "over", "just", "be", "no", "see", "world", "run", "place", "good", "find", "way", "work", "school",
    "game", "night", "right", "left", "white", "color", "someone", "man", "woman", "child", "children", "people", "language", "hand",
    "eye", "face", "head", "mouth", "life", "death", "question", "answer", "time", "space", "light", "dark", "story", "line", "know",
    "known", "table", "shirt", "pants", "shoes", "computer", "movie", "television", "board", "screen", "car", "bicycle", "train", "city",
    "town", "village", "country", "nation", "world", "music", "song", "guitar", "piano", "harmonica", "drums", "band", "sing", "dance", 
    "eat", "drink", "sleep", "run", "jump", "smile", "laugh", "cry", "think", "remember", "forget", "understand", "love", "hate", "like", 
    "need", "want", "wish", "desire", "hope", "try", "hard", "work", "play", "study", "learn", "teach", "student", "teacher", "school", 
    "university", "class", "book", "pen", "paper", "word", "sentence", "paragraph", "book", "novel", "poem", "author", "writer", "reader"
};


bool isCorrect(const std::string& word) {
    return dictionary.find(word) != dictionary.end();
}

void HighlightMisspelledWords(const std::string& text) {
    std::istringstream iss(text);
    std::string word;

    while (iss >> word) {
      
        word.erase(remove_if(word.begin(), word.end(), [](unsigned char c) { return !isalpha(c); }), word.end());
        
        bool correct = isCorrect(word);

        if (!correct) {
          
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", word.c_str());
        } else {
            
            ImGui::Text("%s ", word.c_str());
        }
    }
}


bool ToggleButton(const char* str_id, bool* v) {
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


using namespace ImGui;

bool CustomListBoxInt(const char* label, int* value, const std::vector<const char*> list, float width, ImVec4 col, ImGuiComboFlags flags) {
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
	if (col.x == 0 && col.y == 0 && col.z == 0 && col.w == 0) Text(label);
	else TextColored(col, label);

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

bool SteppedSliderFloat(const char* label, float* v, float v_min, float v_max, float v_step, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
	char text_buf[64] = {};
	ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), format, *v);

	const int stepCount = int((v_max - v_min) / v_step);
	int v_i = int((*v - v_min) / v_step);
	const bool valueChanged = SliderInt(label, &v_i, 0, stepCount, text_buf);

	*v = v_min + float(v_i) * v_step;
	return valueChanged;
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
	const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	RenderNavHighlight(frame_bb, id);
	RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

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
	if (grab_bb.Max.x > grab_bb.Min.x)
		window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

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

	if (!IsItemHovered())
		return false;

	SetTooltip("Press any key to change the keybind, ESC to reset");
	for (uint8_t vKey : KeyBinds::GetValidKeys()) {
		if (KeyBinds::IsKeyDown(vKey)) {
			key = (vKey != VK_ESCAPE ? vKey : 0x00);
			return true;
		}
	}

	return false;
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

	drawList->AddCircleFilled(center, 4.5F * State.dpiScale, color);
	drawList->AddCircle(center, (4.5F + 0.5F) * State.dpiScale, statusColor, 0, 2.0F);
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

	ImGui::PushID(label);
	ImGui::PushStyleColor(ImGuiCol_Button, highlight ? activeCol : defaultCol);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredCol);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeCol);
	bool selected = ImGui::Button(label);
	ImGui::PopStyleColor(3);
	ImGui::PopID();
	return selected;
}
