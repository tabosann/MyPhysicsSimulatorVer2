#ifndef ___CLASS_APPLICATION
#define ___CLASS_APPLICATION

class DX12Wrapper;
class ObjectRenderer;
class MyRectangle;
class MySphere;

//�V���O���g���݌v
//DX12���C�u�����̏��������s���A�E�B���h�E�̍쐬���s���B
//'_dx12'�����L���邱�ƂŁA�쐬�����E�B���h�E��ւ̑����������B
class Application {
public:
	virtual ~Application();

	//�`�悷�镨�̂���
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

	//ImGui�֐��Q
	void ShowDebugConsoleMenu();
	void ShowSupervisorMenu();
	void ShowCreatorMenu();

	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LRESULT ApplicationWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//�V���O���g���݌v�̂��߁A�A�N�Z�X�֎~�ɂ��Ă���
	Application();
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};

#endif
