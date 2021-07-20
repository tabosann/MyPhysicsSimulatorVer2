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
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

	DX12Wrapper* GetDX12() const;

	void SetRectangleObjects(MyRectangle** begin, MyRectangle** end);
	void SetSphereObjects(MySphere** begin, MySphere** end);
	void SetEditFunc(void(*EditFunc)());
	void SetRenderFunc(void(*RenderFunc)());

	virtual ~Application();

private:

	std::unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	std::unique_ptr<ObjectRenderer> _renderer = nullptr;

	//���[�U��`�֐��Q
	void (*_EditFunc)();   //�`��ȊO�̔C�ӂ̏���
	void (*_RenderFunc)(); //�`�揈��

	void BeginEdit();
	void EndEdit();
	void ImGui_Render();

	//�Ǝ��̃E�B���h�E�v���V�[�W��
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//�V���O���g���݌v�̂��߁A�A�N�Z�X�֎~�ɂ��Ă���
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
