#include<MyImGui.h>

bool ImGui::IsAtShiftTrueToFalse(bool judge) {
	static bool previous = judge;
	bool result = previous && !judge;
	previous = judge;
	return result;
}

void ImGui::HelpMarker(const char* desc) {

	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {

		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
