#pragma once

#include "../../camera/Camera.h"

#include "../../Engine/3D/ParticleObject.h"

class BaseParticleObjects
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(UINT texNumber);

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
	ParticleObject* object;
	WorldTransform world;

	Camera* camera = nullptr;
};

