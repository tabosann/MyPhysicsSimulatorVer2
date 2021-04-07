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
	void (*_MainFunc)();              //メインループ内で行う任意の処理

	DX12Wrapper*    GetDX12() const;
	ObjectRenderer* GetRenderer() const;

	void SetMainFunc(void(*MainFunc)());
	void SetPlay(bool play);

	static Application& Instance();
	bool Init();
	void BeginEdit();
	void EndEdit();
	void ImGui_Render();
	void Run();
	void ShutDown();

	virtual ~Application();

	//ImGui関数群
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

private:

	unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	unique_ptr<ObjectRenderer> _renderer = nullptr;
	bool                       _play = false;

	//独自のウィンドウプロシージャ
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//シングルトン設計のため、アクセス禁止にしている
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
