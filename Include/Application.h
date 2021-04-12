#pragma once

#include<MyWindowBase.h>

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

//�V���O���g���݌v
//DX12���C�u�����̏��������s���A�E�B���h�E�̍쐬���s���B
//'_dx12'�����L���邱�ƂŁA�쐬�����E�B���h�E��ւ̑����������B
class Application : public WndBase
{
	template<class T>
	using vector = std::vector<T>;
	template<class T>
	using unique_ptr = std::unique_ptr<T>;

public:

	vector<MyRectangle*> _rectangles; //�����̂̃I�u�W�F�N�g
	vector<MySphere*>	 _spheres;    //���̂̃I�u�W�F�N�g

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

	unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	unique_ptr<ObjectRenderer> _renderer = nullptr;

	void BeginEdit();
	void EndEdit();

	//�Ǝ��̃E�B���h�E�v���V�[�W��
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//�V���O���g���݌v�̂��߁A�A�N�Z�X�֎~�ɂ��Ă���
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
