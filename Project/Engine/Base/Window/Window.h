#pragma once
#include <Windows.h>
#include <string>

/// <summary>
/// Window
/// </summary>
class Window
{
private:
	static Window* instance;

public:
	Window();
	~Window();

	static Window* GetInstance();
	static void Delete();

	void Create(const std::string& name = "DirectXGame", const int width = 1280, const int height = 720);
	void Finalize();

	const HWND& GetHwnd()	{return hwnd;}
	const WNDCLASSEX& GetWndClass()	{return w;}

	inline static int GetWindowWidth()		{return window_width;}
	inline static int GetWindowHeight()	{return window_height;}

public:
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


private:
	HWND hwnd = nullptr;	//ウィンドウハンドル
	WNDCLASSEX w{};		//ウィンドウクラス
	static wchar_t kWindowClassName[];

	std::string name;
	inline static int window_width = 1280;
	inline static int window_height = 720;
};

