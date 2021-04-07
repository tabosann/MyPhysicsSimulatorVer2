#pragma once

#include"MyImGui.h"

#define MY_RANGE_CHECKER(i, begin, end) (begin <= i && i < end)

template<class T>
void ImGui::SafeDelete(std::vector<T*>& items) {
	T* temp = items.back();
	items.pop_back();
	delete temp;
}

template<class T>
void ImGui::SafeDelete(std::vector<T*>& items, int targetIdx) {
	T* temp = items[targetIdx];
	items.erase(items.begin() + targetIdx);
	delete temp;
}

template<class T>
void ImGui::SafeDeleteButton(std::vector<T*>& items) {
	bool isEmpty = items.empty();
	ImGui::SameLine();
	if (ImGui::Button("Delete") && !isEmpty) ImGui::SafeDelete(items);
	ImGui::SameLine();
	if (isEmpty) ImGui::Text("ALL DELETED!");
}

//選択されたアイテム番号'selected_idx'経由でアイテムを削除するか尋ねる
template<class T>
bool ImGui::AskToDelete(std::vector<T*>& items, int targetIdx) {
	ImVec2 dragDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

	//ポップアップ表示の条件 = 選択したアイテムの上でカーソルを動かさないで右クリック
	bool terms = true;
	terms &= ImGui::IsMouseReleased(ImGuiMouseButton_Right);
	terms &= dragDelta.x == 0.0f && dragDelta.y == 0.0f;

	//ポップアップ表示
	if (terms) ImGui::OpenPopupOnItemClick("context");
	if (ImGui::BeginPopup("context")) {
		if (terms = ImGui::MenuItem("Delete")) ImGui::SafeDelete(items, targetIdx);
		ImGui::EndPopup();
	}
	return terms;
}

template<class T>
bool ImGui::SearchForSelectedItem(std::vector<T*>& items, const Char32& targetName) {
	for (T* item : items)
		if (item->_name == targetName) return true;
	return false;
}

template<class T>
const char* ImGui::CreateUniqueItemName(std::vector<T*>& items, const char* label) {
	static Char32 name;
	name = label;

	//アイテム名に被りがなくなるまで、末尾に付加する番号をカウントアップしていく
	int cnt = 0;
	while (ImGui::SearchForSelectedItem(items, name))
		Char32::Copy(name, "%s(%d)", label, ++cnt);

	return name;
}
