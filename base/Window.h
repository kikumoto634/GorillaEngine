#pragma once
#include <Windows.h>
#include <string>

/// <summary>
/// Window
/// </summary>
class Window
{
public:
	Window(const int width = 1280, const int height = 720);
	~Window();

	void Create(const wchar_t* name = L"DirectXGame");
	void Finalize();

	const HWND& GetHwnd()	{return hwnd;}
	const WNDCLASSEX& GetWndClass()	{return w;}

	int GetWindowWidth()	{return window_width;}
	int GetWindowHeight()	{return window_height;}

public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


private:
	HWND hwnd = nullptr;	//ウィンドウハンドル
	WNDCLASSEX w{};		//ウィンドウクラス
	static wchar_t kWindowClassName[];
	int window_width;
	int window_height;
};

