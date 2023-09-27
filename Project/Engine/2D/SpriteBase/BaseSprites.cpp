#include "BaseSprites.h"
#include "Easing.h"

void BaseSprites::Initialize(UINT textureNumber)
{
	sprite = Sprite::Create(textureNumber, position);

	window = Window::GetInstance();

	//クラス名の文字列を取得
	name = typeid(*this).name();
}

void BaseSprites::Update()
{
	sprite->SetPosition(position);
	sprite->SetSize(size);
	sprite->SetColor({color.x,color.y,color.z,color.w});
}

void BaseSprites::Draw()
{
	sprite->Draw();
}

void BaseSprites::Finalize()
{
	delete sprite;
	sprite = nullptr;
}

bool BaseSprites::ScaleChange(Vector2 sizeMax, Vector2 sizeMin, float EndTime)
{
	float ease = -(cosf(3.14159265f * scaleCurrentTime) - 1.f)/2.f;
	scale = Easing_Point2_Linear(sizeMin, sizeMax, ease);
	SetSize(scale);

	if(scaleCurrentTime >= 1.0f){
		scale = sizeMax;
		scaleCurrentTime = 0.f;
		return true;
	}

	scaleCurrentTime += 1.f/(60*EndTime);
	return false;
}
