#include "DebugScreenText.h"

DebugScreenText::~DebugScreenText()
{
	for(int i = 0; i < _countof(sprites); i++)
	{
		//スプライトを解放
		delete sprites[i];
		sprites[i] = nullptr;
	}
}

void DebugScreenText::Initialize(UINT texnumber)
{
	for(int i = 0; i < _countof(sprites); i++)
	{
		//スプライト生成
		sprites[i] = new Sprite();
		//スプライトを初期化する
		sprites[i]->Initialize(texnumber);
		sprites[i]->SetColor({0,1,0,1});
	}
}

//一文字追加
void DebugScreenText::Print2D(const std::string& text, Vector2 pos, float scale)
{
	pos2D = pos;
	this->scale = scale;
	this->text = text.c_str();

	//すべての文字について
	for(int i = 0; i < (int)text.size(); i++)
	{
		//最大文字数
		if(spriteIndex >= maxCharCount)
		{
			break;
		}

		//一文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char& character = this->text[i];

		int fontIndex = character - 32;
		if(character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		sprites[spriteIndex]->SetPosition({pos2D.x + fontWidth * this->scale * i, pos2D.y});
		sprites[spriteIndex]->SetTextureRect((float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight, (float)fontWidth, (float)fontHeight);
		sprites[spriteIndex]->SetSize({fontWidth * this->scale, fontHeight * this->scale});

		//文字を一つ進める
		spriteIndex++;
	}
}

void DebugScreenText::Printf2D(Vector2 pos, float scale, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int w = vsnprintf(buffer, kBufferSize - 1, fmt, args);
	Print2D(buffer, pos, scale);
	va_end(args);
}

void DebugScreenText::Print3D(const std::string &text, Vector3 pos, Camera* camera,float scale)
{
	pos3D = pos;
	this->scale = scale;
	this->text = text.c_str();

	//すべての文字について
	for(int i = 0; i < (int)text.size(); i++)
	{
		//最大文字数
		if(spriteIndex >= maxCharCount)
		{
			break;
		}

		//一文字取り出す(※ASCIIコードでしか成り立たない)
		const unsigned char& character = this->text[i];

		int fontIndex = character - 32;
		if(character >= 0x7f)
		{
			fontIndex = 0;
		}

		int fontIndexY = fontIndex / fontLineCount;
		int fontIndexX = fontIndex % fontLineCount;

		//座標計算
		Vector2 lpos = sprites[spriteIndex]->Vector2Transform(pos3D,camera);
		lpos.x = lpos.x + fontWidth * this->scale * (i-((int)text.size()>>1));

		sprites[spriteIndex]->SetPosition(lpos);
		sprites[spriteIndex]->SetTextureRect((float)fontIndexX * fontWidth, (float)fontIndexY * fontHeight, (float)fontWidth, (float)fontHeight);
		sprites[spriteIndex]->SetSize({fontWidth * this->scale, fontHeight * this->scale});

		//文字を一つ進める
		spriteIndex++;
	}
}

void DebugScreenText::Printf3D(Vector3 pos, float scale, Camera* camera, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int w = vsnprintf(buffer, kBufferSize - 1, fmt, args);
	Print3D(buffer, pos, camera, scale);
	va_end(args);
}

void DebugScreenText::DrawAll()
{
	//すべての文字のスプライトについて
	for(int i = 0; i < spriteIndex; i++)
	{
		//スプライト描画
		sprites[i]->Draw();
	}
	spriteIndex = 0;
}
