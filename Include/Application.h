#pragma once

#include<MyWindowBase.h>

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

//シングルトン設計
//DX12ライブラリの初期化を行い、ウィンドウの作成も行う。
//'_dx12'を共有することで、作成したウィンドウ上への操作を許可する。
class Application : public WndBase
{
	template<class T>
	using vector = std::vector<T>;
	template<class T>
	using unique_ptr = std::unique_ptr<T>;

public:

	vector<MyRectangle*> _rectangles; //直方体のオブジェクト
	vector<MySphere*>	 _spheres;    //球体のオブジェクト

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

	unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	unique_ptr<ObjectRenderer> _renderer = nullptr;

	void BeginEdit();
	void EndEdit();

	//独自のウィンドウプロシージャ
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//シングルトン設計のため、アクセス禁止にしている
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
