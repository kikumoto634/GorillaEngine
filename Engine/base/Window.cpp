#include "Window.h"
#include <imgui_impl_win32.h>

#pragma comment(lib, "winmm.lib")

wchar_t Window::kWindowClassName[] = L"DirectXGame";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

Window::Window()
{
}

Window::~Window()
{
}

Window *Window::GetInstance()
{
	static Window instance;
	return &instance;
}

void Window::Create(const std::string& name, const int width, const int height)
{
	this->name = name;
	this->window_width = width;
	this->window_height = height;

	//ウィンドウクラス設定
	//WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;		//ウィンドウプロシージャ設定
	w.lpszClassName = kWindowClassName;			//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定

	//ウィンドウクラスをOSに登録する
	RegisterClassEx(&w);
	//ウィンドウサイズ{ X座標 Y座標 横幅 縦幅}
	RECT wrc = {0, 0, window_width, window_height};
	//自動でサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	///ウィンドウオブジェクトの生成
	//HWND hwnd = nullptr;
	wchar_t wName[256];
	MultiByteToWideChar(CP_ACP, 0, this->name.c_str(), -1, wName, _countof(wName));

	hwnd = CreateWindow(
		w.lpszClassName,			//クラス名
		wName,				//タイトルバー文字
		WS_OVERLAPPEDWINDOW,		//標準的なウィンドウスタイル
		CW_USEDEFAULT,				//表示X座標(OSに任せる)
		CW_USEDEFAULT,				//表示Y座標(OSに任せる)
		wrc.right  - wrc.left,		//ウィンドウ横幅
		wrc.bottom - wrc.top,		//ウィンドウ縦幅
		nullptr,					//親ウィンドウハンドル
		nullptr,					//メニューハンドル
		w.hInstance,				//呼び出しアプリケーションハンドル
		nullptr						//オプション
	);
	//ウィンドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);

	// システムタイマーの分解能を上げる
    timeBeginPeriod(1);
}

void Window::Finalize()
{
	///ウィンドウクラスを登録解除
	UnregisterClass(w.lpszClassName, w.hInstance);
}

LRESULT Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//ImGui用ウィンドウプロシージャ呼び出し
	if(ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)){
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg){
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
