#pragma once

//キーボード入力
#define DIRECTINPUT_VERSION 0x0800	//DirectInputのバージョン指定
#include <dinput.h>
const int KeyNum = 256;

class Input
{
public:
	Input();
	~Input() = default;

	static Input* GetInstance();

	//初期化
	void Initialize(HWND hwnd);

	//更新
	void Update();

#pragma region キーボード入力
	bool Push(int keyNumber);
	bool Trigger(int keyNumber);
#pragma endregion

private:
#pragma region キーボード
	//キーボードデバイス
	IDirectInputDevice8* keyboard = nullptr;
	//キー情報
	BYTE key[KeyNum] = {};
	//前フレームキー情報
	BYTE prekey[KeyNum] = {};
#pragma endregion
};

