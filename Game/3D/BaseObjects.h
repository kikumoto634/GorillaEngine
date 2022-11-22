#pragma once
#include "Camera.h"

#include <string>
#include "../../Engine/base/FbxModelManager.h"
#include "../../Engine/3D/FbxModelObject.h"

class BaseObjects
{
public:
	/// <summary>
	/// èâä˙âª
	/// </summary>
	virtual void Initialize(std::string filePath);

	/// <summary>
	/// çXêV
	/// </summary>
	virtual void Update(Camera* camera);

	/// <summary>
	/// ï`âÊ
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// å„èàóù
	/// </summary>
	virtual void Finalize();

	//Getter
	const Vector3& GetPosition()	{return world.translation;}
	const Vector3& GetRotation()	{return world.rotation;}

	//Setter
	void SetPosition(const Vector3& position)	{world.translation = position;}
	void SetRotation(const Vector3& rotation)	{world.rotation = rotation;}

protected:
	FbxModelManager* model;
	FbxModelObject* object;
	WorldTransform world;

	Camera* camera = nullptr;
};
