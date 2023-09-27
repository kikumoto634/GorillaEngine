#pragma once
#include <string>
#include <Windows.h>
#include <d3d12.h>

#include "Sprite.h"
#include "Camera.h"

//デバック用文字列クラスの定義
class DebugScreenText
{
public:

	static const int maxCharCount = 512;	//最大文字数
	static const int fontWidth = 9;			//フォント画像内1文字内の横幅
	static const int fontHeight = 18;		//フォント画像内1文字内の縦幅
	static const int fontLineCount = 14;	//フォント画像内1行分の文字数
	static const int kBufferSize = 256;		//書式つき文字列展開用バッファサイズ

	~DebugScreenText();

	void Initialize(UINT texnumber);

	void Print2D(const std::string& text, Vector2 pos, float scale = 1.f);
	void Printf2D(Vector2 pos, float scale, const char* fmt, ...);

	void Print3D(const std::string& text, Vector3 pos, Camera* camera, float scale = 1.f);
	void Printf3D(Vector3 pos, float scale, Camera* camera,const char* fmt, ...);

	void DrawAll();

private:
	Sprite* sprites[maxCharCount] {};
	int spriteIndex = 0;
	Vector2 pos2D;
	Vector3 pos3D;
	float scale;
	const char* text = {};
	// 書式付き文字列展開用バッファ
	char buffer[kBufferSize] = {};
};