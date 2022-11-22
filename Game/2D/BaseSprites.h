#pragma once
#include "../Engine/2D/Sprite.h"
#include "../Engine/math/Vector/Vector2.h"

class BaseSprites
{
public:
	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	virtual void Initialize(UINT textureNumber);

	/// <summary>
	/// XV
	/// </summary>
	virtual void Update();

	/// <summary>
	/// •`‰æ
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// Œãˆ—
	/// </summary>
	virtual void Finalize();

	//Getter
	const Vector2& GetPosition()	{return position;}
	const Vector2& GetSize()	{return size;}

	//Setter
	void SetPosition(const Vector2& position)	{this->position = position;}
	void SetSize(const Vector2& size)	{this->size = size;}

protected:
	Sprite* sprite;
	Vector2 position = {0,0};
	Vector2 size = {100,100};
};