#include "pch-il2cpp.h"
#include "toasts.hpp"
#include "imgui/imgui.h"
#include "gui-helpers.hpp"
#include "state.hpp"
#include "DirectX.h"
#include "logger.h"

namespace Toasts
{
	std::vector<ToastInfo> pendingToasts = {};

	void AddToast(const std::string& header, const std::string& body, ImVec4 headerColor) {
		ToastInfo newToast;

		headerColor.w *= State.MenuThemeColor.w;
		newToast.headerColor = headerColor;
		newToast.header = header;
		newToast.body = body;
		newToast.maxTime = State.ToastMaxDuration;
		newToast.remainingTime = State.ToastMaxDuration;

		pendingToasts.push_back(newToast);
	}

	void Render() {
		float edgePadding = 15.f * State.dpiScale;
		bool showToast = true; // just for p_open

		float toastWidth = 300.f * State.dpiScale;
		float toastHeight = 100.f * State.dpiScale;

		auto start = pendingToasts.size() > static_cast<std::size_t>(State.MaxToasts) ?
			pendingToasts.end() - State.MaxToasts : pendingToasts.begin();
		int offsetIndex = 0;
		float toastOffset = 0.f;

		float windowXOffset = (DirectX::GetWindowSize(true).x - DirectX::GetWindowSize(false).x) / 2.f;
		float windowYOffset = (DirectX::GetWindowSize(true).y - DirectX::GetWindowSize(false).y) / 2.f;

		for (auto it = start; it != pendingToasts.end();) {
			ToastInfo& toast = *it;
			bool clearToast = false;

			toastHeight = 50.f * State.dpiScale +
				ImGui::CalcTextSize(toast.body.c_str(), 0, false, toastWidth - 15.f * State.dpiScale).y;

			std::string windowName = std::format("###Toast_{}", offsetIndex);
			float toastXPos, toastYPos;

			switch ((AlignmentY)(std::clamp(State.ToastPositionX, 0, 2))) {
			case AlignmentY::Left:
				toastXPos = windowXOffset + edgePadding;
				break;
			case AlignmentY::Center:
				toastXPos = windowXOffset + (DirectX::GetWindowSize(false).x - toastWidth) / 2.f;
				break;
			case AlignmentY::Right:
				toastXPos = windowXOffset + DirectX::GetWindowSize(false).x - edgePadding - toastWidth;
				break;
			}

			toastYPos = State.ToastsOnTop ?
				windowYOffset + edgePadding + toastOffset :
				windowYOffset + DirectX::GetWindowSize(false).y - edgePadding - toastOffset - toastHeight;

			ImGui::SetNextWindowSize(ImVec2(toastWidth, toastHeight), ImGuiCond_Always);
			ImGui::SetNextWindowPos(ImVec2(toastXPos, toastYPos), ImGuiCond_Always);

			ImGui::PushStyleColor(ImGuiCol_Border, toast.headerColor);
			ImVec4 sliderColor = toast.headerColor;
			sliderColor.w *= State.MenuThemeColor.w * 0.8f; // apply transparency once again
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, sliderColor);
			ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.f, 0.f, 0.f, 0.f));

			ImGui::Begin(windowName.c_str(), &showToast, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
			
			ImGui::TextColored(toast.headerColor, toast.header.c_str());
			ImGui::SameLine(toastWidth - 21.f * State.dpiScale);
			if (ImGui::Button(("\u00D7" + windowName).c_str())) clearToast = true;
			// AnimatedButton isn't used since it glows the close button when there's a new toast

			float remainingTimeVisual = toast.remainingTime; // so that the actual remainingTime cannot be modified
			ImGui::SetNextItemWidth(toastWidth - 15.f * State.dpiScale); // the slider starts from around 15 pixels

			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			SteppedSliderFloat("", &remainingTimeVisual, 0.f, toast.maxTime, 0.1f, "", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoInput);
			ImGui::PopItemFlag();

			ImGui::BeginChild(("child" + windowName).c_str(), ImVec2(toastWidth - 15.f * State.dpiScale, 0.f), false, ImGuiWindowFlags_NoBackground);

			ImGui::TextWrapped(toast.body.c_str());

			ImGui::EndChild();

			ImGui::End();
			ImGui::PopStyleColor(4);

			toast.remainingTime -= Time_get_deltaTime(NULL);
			if (toast.remainingTime <= 0.f || clearToast) {
				it = pendingToasts.erase(it);
			}
			else {
				++it;
				++offsetIndex;
				toastOffset += toastHeight + edgePadding;
			}
		}
	}
}