#include "Input.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")

Input::Input()
{
}

Input *Input::GetInstance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize(HWND hwnd)
{
	HRESULT result;

	///DirectInPut
	//初期化 (他入力方法追加でもこのオブジェクトは一つのみ)
	IDirectInput8* directInput = nullptr;
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	result = DirectInput8Create(
		hInstance, 
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8,
		(void**)&directInput, 
		nullptr
	);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成 (GUID_Joystick (ジョイステック)、 GUID_SysMouse (マウス))
	result = directInput->CreateDevice(
		GUID_SysKeyboard,
		&keyboard,
		NULL
	);
	assert(SUCCEEDED(result));

	//入力データ形式のセット (入力デバイスの種類によって、あらかじめ何種類か用意する)
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(result));

	//排他的制御レベルのセット
	//DISCL_FOREGROUND		画面が手前にある場合のみ入力を受け付ける
	//DISCL_NONEXCLUSIVE	デバイスをこのアプリだけで専有しない
	//DISCL_NOWINKEY		Windowsキーを無効にする
	result = keyboard->SetCooperativeLevel(
		hwnd,
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	HRESULT result;

	//動作開始
	result = keyboard->Acquire();
	//assert(SUCCEEDED(result));

	//前回のキー入力情報コピー
	for(int i = 0; i <KeyNum; i++){
		prekey[i] = key[i];
	}

	//キー入力
	result = keyboard->GetDeviceState(sizeof(key), key); 
	//assert(SUCCEEDED(result));
}

bool Input::Push(int keyNumber)
{
	if(keyNumber < 0x00) return false;
	if(keyNumber > 0xff) return false;

	if(key[keyNumber])
	{
		return true;
	}
	return false;
}

bool Input::Trigger(int keyNumber)
{
	if(keyNumber < 0x00) return false;
	if(keyNumber > 0xff) return false;

	if(key[keyNumber] && !prekey[keyNumber])
	{
		return true;
	}
	return false;
}
