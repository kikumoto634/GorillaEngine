#pragma once
#include "../../camera/Camera.h"

#include <string>
#include "../../Engine/base/FbxModelManager.h"
#include "../../Engine/3D/FbxModelObject.h"

class BaseFbxObjects
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(std::string filePath);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="matBillboard">ビルボード開始</param>
	virtual void Update(Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 後処理
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
