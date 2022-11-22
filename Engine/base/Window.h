#pragma once
#include <Windows.h>
#include <string>

/// <summary>
/// Window
/// </summary>
class Window
{
public:
	Window();
	~Window();

	static Window* GetInstance();

	void Create(const std::string& name = "DirectXGame", const int width = 1280, const int height = 720);
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

	std::string name;
	int window_width;
	int window_height;
};

