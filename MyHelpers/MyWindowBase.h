#pragma once

#include<Windows.h>

class MyWindowBase 
{
public:

	MyWindowBase();
	virtual ~MyWindowBase();

	POINT      GetWindowPos() const;
	SIZE       GetWindowSize() const;
	UINT       GetMsg() const;
	static int GetCount();

	bool Init(LPCWSTR name, LONG pos_x = 0, LONG pos_y = 0, LONG width = 0, LONG height = 0);
	bool Show() const;
	bool Update() const;
	void Close(unsigned short key) const;
	void Quit();
	bool ShutDown() const;

	static void MsgProcessor(MyWindowBase* wnd);

protected:

	HWND _hwnd = {};
	virtual LRESULT LocalWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:

	static int _count;
	LONG       _x, _y, _w, _h;
	LPCWSTR    _name;
	WNDCLASSEX _wndClass = {};
	MSG        _msg = {};

	static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	MyWindowBase(const MyWindowBase&) = delete;
	MyWindowBase& operator=(const MyWindowBase&) = delete;
};

using WindowBase = MyWindowBase;
using WndBase = WindowBase;
