
#include"MyImGui.h"
#include<DX12Wrapper.h>
#include<MyRectangle.h>
#include<MySphere.h>
#include<Application.h>

bool ImGui::IsAtShiftTrueToFalse(bool judge)
{
	static bool previous = judge;
	bool result = previous && !judge;
	previous = judge;
	return result;
}

void ImGui::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void ImGui::PlayButton(bool& play)
{
	Char32 text = play ? "Stop" : "Play";
	if (ImGui::Button(text, ImVec2(100, 0))) play = !play;
}

void ImGui::ResetButton(Application* app)
{
	Char32 text = "All Reset";
	if (ImGui::Button(text, ImVec2(100, 0)))
	{
		for (auto r : app->_rectangles) r->Reset();
		for (auto s : app->_spheres)    s->Reset();
		app->GetDX12()->Reset();
	}
}
