#pragma once

#include"../MyHelpers/MyWindowBase.h"

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

//シングルトン設計
//DX12ライブラリの初期化を行い、ウィンドウの作成も行う。
//'_dx12'を共有することで、作成したウィンドウ上への操作を許可する。
class Application : public WndBase
{
public:

	//描画する物体たち
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

	//ImGui関数群
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

	//独自のウィンドウプロシージャ
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//シングルトン設計のため、アクセス禁止にしている
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
