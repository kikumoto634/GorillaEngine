#pragma once
#include <string>
#include <Windows.h>
#include <d3d12.h>

#include "../2D/Sprite.h"

//デバック用文字列クラスの定義
class DebugText
{
public:

	static const int maxCharCount = 512;	//最大文字数
	static const int fontWidth = 9;			//フォント画像内1文字内の横幅
	static const int fontHeight = 18;		//フォント画像内1文字内の縦幅
	static const int fontLineCount = 14;	//フォント画像内1行分の文字数
	static const int kBufferSize = 256;		//書式つき文字列展開用バッファサイズ

	~DebugText();

	void Initialize(UINT texnumber);

	void Print(const std::string& text, float x, float y, float scale = 1.f);
	void Printf(float x, float y, float scale, const char* fmt, ...);

	void DrawAll();

private:
	Sprite* sprites[maxCharCount];
	int spriteIndex = 0;
	float posX;
	float posY;
	float scale;
	const char* text;
	// 書式付き文字列展開用バッファ
	char buffer[kBufferSize];
};