#include "SampleSprite.h"

SampleSprite::~SampleSprite()
{
	Finalize();
}

void SampleSprite::Initialize(UINT textureNumber)
{
	BaseSprites::Initialize(textureNumber);
}

void SampleSprite::Update()
{
	BaseSprites::Update();
}

void SampleSprite::Draw()
{
	BaseSprites::Draw();
}

void SampleSprite::Finalize()
{
	BaseSprites::Finalize();
}
