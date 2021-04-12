#pragma once

#include<MyWindowBase.h>

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

class Application : public WndBase
{
public:

	std::vector<MyRectangle*> _rectangles; //直方体のオブジェクト
	std::vector<MySphere*>	  _spheres;    //球体のオブジェクト

	//アプリケーション関数群
	static Application& Instance();
	bool Init();
	void Run();
	void ShutDown();

	//ImGui関数群
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

	//独自のウィンドウプロシージャ
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//シングルトン設計のため、アクセス禁止にしている
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
