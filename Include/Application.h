#pragma once

#include"../MyHelpers/MyWindowBase.h"

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

//�V���O���g���݌v
//DX12���C�u�����̏��������s���A�E�B���h�E�̍쐬���s���B
//'_dx12'�����L���邱�ƂŁA�쐬�����E�B���h�E��ւ̑����������B
class Application : public WndBase
{
public:

	//�`�悷�镨�̂���
	std::vector<MyRectangle*> _rectangles;
	std::vector<MySphere*>	  _spheres;

	virtual ~Application();

	static Application& Instance();
	bool Init();
	void BeginEdit();
	void EndEdit();
	void ImGui_Render();
	void Run();
	void ShutDown();

private:

	std::unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	std::unique_ptr<ObjectRenderer> _renderer = nullptr;
	bool                            _play = false;

	//ImGui�֐��Q
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

	//�Ǝ��̃E�B���h�E�v���V�[�W��
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//�V���O���g���݌v�̂��߁A�A�N�Z�X�֎~�ɂ��Ă���
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
