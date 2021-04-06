#pragma once

#include<Windows.h>

class MyWindowBase 
{
public:

	int _x, _y, _width, _height;
	LPCWSTR _name;

	MyWindowBase();
	virtual ~MyWindowBase();

	UINT       GetMsg() const;
	static int GetCount();

	bool Init(LPCWSTR name, int posX = 0, int posY = 0, int width = 0, int height = 0);
	bool Show() const;
	bool Update() const;
	bool Close(unsigned short key) const;
	bool CatchMsg();
	void Quit();
	bool ShutDown() const;

	static bool    HelperForMsgProcess(MyWindowBase& wnd);
	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:

	HWND _hwnd = {};
	virtual LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	static int _count;
	WNDCLASSEX _wndClass = {};
	MSG        _msg = {};

	MyWindowBase(const MyWindowBase&) = delete;
	MyWindowBase& operator=(const MyWindowBase&) = delete;
};

using WindowBase = MyWindowBase;
using WndBase = WindowBase;
