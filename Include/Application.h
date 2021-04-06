#ifndef ___CLASS_APPLICATION
#define ___CLASS_APPLICATION

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

//シングルトン設計
//DX12ライブラリの初期化を行い、ウィンドウの作成も行う。
//'_dx12'を共有することで、作成したウィンドウ上への操作を許可する。
class Application {
public:
	virtual ~Application();

	//描画する物体たち
	std::vector<MyRectangle*> _rectangles;
	std::vector<MySphere*>	  _spheres;

	static Application& Instance();
	bool Init();
	void BeginEdit();
	void EndEdit();
	void ImGui_Render();
	void Run();
	void Terminate();

private:
	WNDCLASSEX                      _windowClass = {};
	HWND                            _hwnd = {};
	std::unique_ptr<DX12Wrapper>    _dx12 = nullptr;
	std::unique_ptr<ObjectRenderer> _renderer = nullptr;
	bool                            _play = false;


	void InitWindow(HWND& hwnd, WNDCLASSEX& windowClass);
	void CloseWindow(MSG& msg);

	//ImGui関数群
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT ApplicationWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//シングルトン設計のため、アクセス禁止にしている
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};

#endif
