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

	//ユーザ定義関数群
	void (*_EditFunc)();   //描画以外の任意の処理
	void (*_RenderFunc)(); //描画処理

	void BeginEdit();
	void EndEdit();
	void ImGui_Render();

	//独自のウィンドウプロシージャ
	LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//シングルトン設計のため、アクセス禁止にしている
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};
