#pragma once
#include <vector>
#include <imgui/imgui.h>
#include "utility.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui/imgui_internal.h"
#include "state.hpp"

static inline ImVec2 operator+(const ImVec2& lhs, const float scalar) { return ImVec2(lhs.x + scalar, lhs.y + scalar); }
static inline ImVec2 operator-(const ImVec2& lhs, const float scalar) { return ImVec2(lhs.x - scalar, lhs.y - scalar); }
static inline ImVec2& operator+=(ImVec2& lhs, const float scalar) { lhs.x += scalar; lhs.y += scalar; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const float scalar) { lhs.x -= scalar; lhs.y -= scalar; return lhs; }

bool CustomListBoxInt(const char* label, int* value, const std::vector<const char*> list, float width = 225.f, ImVec4 col = ImVec4(0, 0, 0, 0), ImGuiComboFlags flags = ImGuiComboFlags_None, const char* visualLabel = "");
bool CustomListBoxIntMultiple(const char* label, std::vector<std::pair<const char*, bool>>* list, float width, bool resetButton = true, ImGuiComboFlags flags = ImGuiComboFlags_None);
bool CustomListBoxPlayerSelectionMultiple(const char* label, std::array<std::pair<PlayerSelection, bool>, Game::MAX_PLAYERS>* list, float width, bool resetButton = true, ImGuiComboFlags flags = ImGuiComboFlags_None);
bool SteppedSliderFloat(const char* label, float* v, float v_min, float v_max, float v_step, const char* format, ImGuiSliderFlags flags);
bool SliderChrono(const char* label, void* p_data, const void* p_min, const void* p_max, std::string_view format, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
bool HotKey(uint8_t& key);
void drawPlayerDot(PlayerControl* player, const ImVec2& winPos, ImU32 color, ImU32 statusColor);
void drawPlayerIcon(PlayerControl* player, const ImVec2& winPos, ImU32 color);
void drawDeadPlayerDot(DeadBody* deadBody, const ImVec2& winPos, ImU32 color);
void drawDeadPlayerIcon(DeadBody* deadBody, const ImVec2& winPos, ImU32 color);
bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
bool InputStringMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
bool InputStringWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
bool ToggleButton(const char* str_id, bool* v);
bool TabGroup(const char* label, bool highlight = false);
bool ColoredButton(ImVec4 col, const char* label);
void BoldText(const char* text, ImVec4 col = ImVec4(0.f, 0.f, 0.f, 0.f));
bool SliderIntV2(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags);
bool AnimatedButton(const char* label, const ImVec2& size = ImVec2(0, 0));