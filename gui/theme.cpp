#include "pch-il2cpp.h"
#include "theme.hpp"
#include "state.hpp"
#include <imgui/imgui.h>

ImVec4 HI(float v) {
	static ImVec4 vec = State.MenuThemeColor;
	if (State.RgbMenuTheme)
		vec = State.RgbColor;
	else
		vec = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
	return ImVec4(vec.x, vec.y, vec.z, v * State.MenuThemeColor.w);
}

ImVec4 MED(float v) {
	static ImVec4 vec = State.MenuThemeColor;
	if (State.RgbMenuTheme)
		vec = State.RgbColor;
	else
		vec = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
	return ImVec4((float)(vec.x / 1.25), (float)(vec.y / 1.25), (float)(vec.z / 1.25), v * State.MenuThemeColor.w);
}

ImVec4 LOW(float v) {
	static ImVec4 vec = State.MenuThemeColor;
	if (State.RgbMenuTheme)
		vec = State.RgbColor;
	else
		vec = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
	return ImVec4((float)(vec.x / 1.5625), (float)(vec.y / 1.5625), (float)(vec.z / 1.5625), v * State.MenuThemeColor.w);
}

ImVec4 BG(float bg, float v = 1) {
	if (!State.MatchBackgroundWithTheme) return ImVec4(bg, bg, bg, v * State.MenuThemeColor.w);
	else {
		static ImVec4 vec = State.MenuThemeColor;
		if (State.RgbMenuTheme)
			vec = State.RgbColor;
		else
			vec = State.GradientMenuTheme ? State.MenuGradientColor : State.MenuThemeColor;
		return ImVec4((float)(vec.x / 2), (float)(vec.y / 2), (float)(vec.z / 2), v * State.MenuThemeColor.w);
	}
}

#define IMGUI_TEXT(v) State.LightMode ? ImVec4(0.2f, 0.2f, 0.2f, v * State.MenuThemeColor.w) : ImVec4(1.f, 1.f, 1.f, v * State.MenuThemeColor.w)

void ApplyTheme()
{
	static const ImGuiStyle defaultStyle;

	auto& style = ImGui::GetStyle();
	style = defaultStyle;
	style.Colors[ImGuiCol_Text] = IMGUI_TEXT(0.78f);
	style.Colors[ImGuiCol_TextDisabled] = IMGUI_TEXT(0.28f);
	style.Colors[ImGuiCol_WindowBg] = BG(0.15f);
	style.Colors[ImGuiCol_ChildBg] = BG(0.15f);
	style.Colors[ImGuiCol_PopupBg] = BG(0.230f, 0.9f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = BG(0.230f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
	style.Colors[ImGuiCol_TitleBg] = BG(0.15f);
	style.Colors[ImGuiCol_TitleBgActive] = BG(0.15f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.15f);
	style.Colors[ImGuiCol_Tab] = MED(0.76f);
	style.Colors[ImGuiCol_TabHovered] = MED(0.86f);
	style.Colors[ImGuiCol_TabActive] = HI(1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = BG(0.230f, 0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = BG(0.230f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = BG(0.13f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
	style.Colors[ImGuiCol_CheckMark] = HI(1.00f);
	style.Colors[ImGuiCol_SliderGrab] = MED(0.78f);
	style.Colors[ImGuiCol_SliderGrabActive] = MED(1.00f);
	style.Colors[ImGuiCol_Button] = BG(0.230f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
	style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
	style.Colors[ImGuiCol_Header] = MED(0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
	style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = BG(0.230f, 0.04f);
	style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
	style.Colors[ImGuiCol_PlotLines] = IMGUI_TEXT(0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = IMGUI_TEXT(0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.230f, 0.73f);

	if (State.LightMode) {
		style.Colors[ImGuiCol_WindowBg] = BG(0.95f);
		style.Colors[ImGuiCol_ChildBg] = BG(0.95f);
		style.Colors[ImGuiCol_PopupBg] = BG(0.9f, 0.9f);
		style.Colors[ImGuiCol_FrameBg] = BG(0.9f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = BG(0.95f);
		style.Colors[ImGuiCol_TitleBgActive] = BG(0.95f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.95f);
		style.Colors[ImGuiCol_MenuBarBg] = BG(0.9f, 0.5f);
		style.Colors[ImGuiCol_ScrollbarBg] = BG(0.9f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrab] = BG(0.6f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.8f);
		style.Colors[ImGuiCol_Button] = BG(0.85f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = MED(0.7f);
		style.Colors[ImGuiCol_ButtonActive] = MED(0.9f);
		style.Colors[ImGuiCol_ResizeGrip] = BG(0.9f, 0.1f);
		style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.8f);
		style.Colors[ImGuiCol_ResizeGripActive] = MED(0.9f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = BG(0.9f, 0.7f);
	}

	style.WindowPadding = ImVec2(6, 4);
	style.WindowRounding = 4.0f;
	style.FramePadding = ImVec2(5, 2);
	style.FrameRounding = 3.0f;
	style.ItemSpacing = ImVec2(7, 1);
	style.ItemInnerSpacing = ImVec2(1, 1);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 6.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 16.0f;
	style.GrabMinSize = 20.0f;
	style.GrabRounding = 2.0f;

	style.WindowTitleAlign.x = 0.50f;

	style.Colors[ImGuiCol_Border] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.FrameBorderSize = 0.0f;
	style.WindowBorderSize = 0.0f;

	style.ChildBorderSize = 0.0f;

	// scale by dpi
	style.ScaleAllSizes(State.dpiScale);

	static int rgbDelay = 0;
	if (rgbDelay <= 0) {
		State.RgbNameColor += 0.025f;
		constexpr auto tau = 2.f * 3.14159265358979323846f;
		while (State.RgbNameColor > tau) State.RgbNameColor -= tau;
		const auto calculate = [](float value) {return std::sin(value) * .5f + .5f; };
		auto color_r = calculate(State.RgbNameColor + 0.f);
		auto color_g = calculate(State.RgbNameColor + 4.f);
		auto color_b = calculate(State.RgbNameColor + 2.f);
		State.rgbCode = std::format("<#{:02x}{:02x}{:02x}>", int(color_r * 255), int(color_g * 255), int(color_b * 255));

		State.RgbColor.x = color_r;
		State.RgbColor.y = color_g;
		State.RgbColor.z = color_b;

		rgbDelay = int(0.01 * GetFps());
	}
	else rgbDelay--;

	static int gradientDelay = 0;
	static uint8_t gradientStep = 1;
	if (gradientDelay <= 0) {
		static bool gradientIncreasing = true;
		if (gradientStep == 1) {
			gradientStep++;
			gradientIncreasing = true;
		}
		else if (gradientStep == 100) {
			gradientStep--;
			gradientIncreasing = false;
		}
		else {
			if (gradientIncreasing) gradientStep++;
			else gradientStep--;
		}
		gradientDelay = int(0.02 * GetFps());
	}
	else gradientDelay--;

	if (State.GradientMenuTheme) {
		float stepR = float((State.MenuGradientColor2.x - State.MenuGradientColor1.x) / 100);
		float stepG = float((State.MenuGradientColor2.y - State.MenuGradientColor1.y) / 100);
		float stepB = float((State.MenuGradientColor2.z - State.MenuGradientColor1.z) / 100);
		State.MenuGradientColor = ImVec4(State.MenuGradientColor1.x + stepR * gradientStep,
			State.MenuGradientColor1.y + stepG * gradientStep,
			State.MenuGradientColor1.z + stepB * gradientStep,
			State.MenuThemeColor.w);
	}
}