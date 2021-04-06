#include"MyWindowBase.h"
#include<assert.h>
#include<tchar.h>

int MyWindowBase::_count = 0; //ウィンドウの合計数をカウント

MyWindowBase::MyWindowBase()
{
	ZeroMemory(&_wndClass, sizeof(WNDCLASSEX));
	ZeroMemory(&_hwnd, sizeof(HWND));
	ZeroMemory(&_msg, sizeof(MSG));

	auto result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	assert(SUCCEEDED(result));
}

MyWindowBase::~MyWindowBase()
{
	assert(ShutDown());
}

UINT MyWindowBase::GetMsg() const
{
	return _msg.message;
}

//作成されたウィンドウの合計数を返す
int MyWindowBase::GetCount()
{
	return WndBase::_count;
}

bool MyWindowBase::Init(LPCWSTR name, int posX, int posY, int width, int height)
{
	_name = name;
	_x = posX; _y = posY;
	_width = width; _height = height;

	//ウィンドウ詳細設定
	_wndClass.cbSize = sizeof(WNDCLASSEX);
	_wndClass.style = CS_HREDRAW | CS_VREDRAW;
	_wndClass.lpfnWndProc = (WNDPROC)MyWindowBase::WndProc;
	_wndClass.hInstance = GetModuleHandle(0);
	_wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	_wndClass.lpszClassName = _name;
	if (!RegisterClassEx(&_wndClass)) return false;

	//ウィンドウサイズ補正
	RECT rect = { 0, 0, _width, _height };
	if (!AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false)) return false;
	_width = rect.right - rect.left;
	_height = rect.bottom - rect.top;

	//ウィンドウ作成
	_hwnd = CreateWindow(
		_wndClass.lpszClassName, _wndClass.lpszClassName, WS_OVERLAPPEDWINDOW,
		_x, _y, _width, _height, NULL, NULL, _wndClass.hInstance, this
	);
	if (!_hwnd) return false;

	++MyWindowBase::_count;
	return true;
}

//ウィンドウ表示
bool MyWindowBase::Show() const
{
	return ::ShowWindow(_hwnd, SW_SHOW) ? true : false;
}

//ウィンドウの再描画
bool MyWindowBase::Update() const
{
	return ::UpdateWindow(_hwnd) ? true : false;
}

// TODO: まだ試作品段階
bool MyWindowBase::Close(unsigned short key) const
{
	BYTE keyState[256];
	if (::GetKeyState(key))
	{
		::SendMessage(_hwnd, WM_CLOSE, 0, 0);
		return true;
	}
	return false;
}

bool MyWindowBase::CatchMsg()
{
	if (::PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&_msg);
		::DispatchMessage(&_msg);
		return true;
	}
	return false;
}

void MyWindowBase::Quit()
{
	--MyWindowBase::_count;
}

bool MyWindowBase::ShutDown() const
{
	if (!::UnregisterClass(_wndClass.lpszClassName, _wndClass.hInstance)) return false;
	return true;
}

bool MyWindowBase::HelperForMsgProcess(MyWindowBase& wnd)
{
	if (wnd.CatchMsg())
	{
		if (wnd.GetMsg() == WM_QUIT)
			wnd.Quit();
		return true;
	}
	return false;
}

LRESULT MyWindowBase::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MyWindowBase* temp = (MyWindowBase*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);

	//取得できた場合(ウィンドウ作成後)
	if (temp) return temp->LocalWndProc(hWnd, msg, wParam, lParam);

	//取得できなかった場合(ウィンドウ初期化時)
	if (msg == WM_CREATE) {
		temp = (MyWindowBase*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		if (temp) {
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)temp);
			return temp->LocalWndProc(hWnd, msg, wParam, lParam);
		}
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT MyWindowBase::LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		assert(::DestroyWindow(_hwnd));
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
