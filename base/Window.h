#pragma once
#include <Windows.h>
#include <string>

/// <summary>
/// Window
/// </summary>
class Window
{
public:
	Window(const std::string& name, const int width = 1280, const int height = 720);
	~Window();

	void Create();
	void Finalize();

	const HWND& GetHwnd()	{return hwnd;}
	const WNDCLASSEX& GetWndClass()	{return w;}

	int GetWindowWifth()	{return window_width;}
	int GetWindowHeight()	{return window_height;}

public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


private:
	HWND hwnd = nullptr;	//ウィンドウハンドル
	WNDCLASSEX w{};		//ウィンドウクラス
	std::string window_name;
	int window_width;
	int window_height;
};

