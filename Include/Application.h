#pragma once

#include<MyWindowBase.h>

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

class Application : public WndBase
{
public:

	std::vector<MyRectangle*> _rectangles; //�����̂̃I�u�W�F�N�g
	std::vector<MySphere*>	  _spheres;    //���̂̃I�u�W�F�N�g

	//�A�v���P�[�V�����֐��Q
	static Application& Instance();
	bool Init();
	void Run();
	void ShutDown();

	//ImGui�֐��Q
	void ImGui_Render();
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

	virtual ~Application();

private:

	std::unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	std::unique_ptr<ObjectRenderer> _renderer = nullptr;

	void BeginEdit();
	void EndEdit();

	//�Ǝ��̃E�B���h�E�v���V�[�W��
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//�V���O���g���݌v�̂��߁A�A�N�Z�X�֎~�ɂ��Ă���
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
