#pragma once
#include <vector>
#include "imgui/imgui.h"

namespace Toasts {
	struct ToastInfo
	{
		ImVec4 headerColor;
		std::string header;
		std::string body;
		float maxTime;
		float remainingTime;
	};

	enum class AlignmentY : uint8_t
	{
		Left = 0,
		Center = 1,
		Right = 2
	};

	void AddToast(const std::string& header, const std::string& body, ImVec4 headerColor = ImVec4(1.f, 1.f, 1.f, 1.f));
	void Render();
};