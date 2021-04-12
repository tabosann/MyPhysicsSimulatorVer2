#pragma once

#include"../ImGui/imgui.h"
#include<MyChar32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ImGui
{
	bool IsAtShiftTrueToFalse(bool judge);
	void HelpMarker(const char* desc);
}

//テンプレート関数群
namespace ImGui
{
	template<class T>
	void SafeDelete(std::vector<T*>& items);
	template<class T>
	void SafeDelete(std::vector<T*>& items, int targetIdx);
	template<class T>
	void SafeDeleteButton(std::vector<T*>& items);
	template<class T>
	bool AskToDelete(std::vector<T*>& items, int targetIdx);

	template<class T>
	bool SearchForSelectedItem(std::vector<T*>& items, const Char32& targetIdx);
	template<class T>
	const char* CreateUniqueItemName(std::vector<T*>& items, const char* label);
}

#include"MyImGui_Impl.h"