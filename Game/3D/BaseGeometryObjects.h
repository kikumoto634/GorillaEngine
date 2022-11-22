#pragma once

#include "Camera.h"

#include "../../Engine/3D/GeometryObject.h"

class BaseGeometryObjects
{
public:
	/// <summary>
	/// ‰Šú‰»
	/// </summary>
	virtual void Initialize(UINT texNumber);

	/// <summary>
	/// XV
	/// </summary>
	virtual void Update(Camera* camera);

	/// <summary>
	/// •`‰æ
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// Œãˆ—
	/// </summary>
	virtual void Finalize();

	//Getter
	const Vector3& GetPosition()	{return world.translation;}
	const Vector3& GetRotation()	{return world.rotation;}

	//Setter
	void SetPosition(const Vector3& position)	{world.translation = position;}
	void SetRotation(const Vector3& rotation)	{world.rotation = rotation;}

protected:
	GeometryObject* object;
	WorldTransform world;

	Camera* camera = nullptr;
};

