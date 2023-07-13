#pragma once

#include "ObjModelObject.h"
#include "ObjModelManager.h"

#include "CollisionInfo.h"


class BaseCollider;

class BaseObjObject
{
protected:
	//終了時間(x(s/秒))
	const float ScaleEndTime = 0.25f;

	//削除後の位置
	const Vector3 NotAlivePos = {0,-50,0};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BaseObjObject() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseObjObject();


	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(std::string filePath, bool IsSmmothing = false);
	virtual void Initialize(ObjModelManager* model);

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

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	virtual void OnCollision(const CollisionInfo& info){};

	virtual void Pop(Vector3 pos);

	//当たり判定時処理
	virtual void ContactUpdate();

	inline void IsBeatEndOn()	{isBeatEnd_ = true;}

	//光計算
	inline void CaveLightOn()	{isLightCal_ = true;}
	inline void CaveLightOff()	{isLightCal_ = false;}

	//Getter
	bool GetIsContactTrigger();
	inline bool GetIsAlive()	{return isAlive_;}
	inline bool GetIsPopsPmposibble_() {return isPopsPosibble_;}
	inline const Vector3& GetPosition()	{return world_.translation;}
	inline const Vector3& GetRotation()	{return world_.rotation;}
	inline const Vector3& GetScale()		{return world_.scale;}
	inline WorldTransform& GetmatWorld()	{return world_;}
	inline const ObjModelObject* GetObjObject()	{return object_;}
	inline const char* GetName()	{return name;}

	//Setter
	void SetPosition(const Vector3& position);
	void SetCollider(BaseCollider* collider);
	inline void SetRotation(const Vector3& rotation)	{world_.rotation = rotation, world_.UpdateMatrix();}
	inline void SetScale(const Vector3& scale)			{world_.scale = scale, world_.UpdateMatrix();}
	inline void SetObject(ObjModelObject* object)	{this->object_ = object;}
	inline void SetModel(ObjModelManager* model)	{this->model_ = model;}

protected:
	/// <summary>
	/// リズムにあわせてサイズ変更 
	/// </summary>
	/// <param name="sizeMax">最大サイズ</param>
	/// <param name="sizeMin">最小サイズ</param>
	/// <param name="EndTime">終了時間 (x:秒)</param>
	/// <returns></returns>
	bool ScaleChange(Vector3& sizeMax, Vector3& sizeMin, const float& EndTime);

	//距離計算
	virtual void DistanceUpdate();

protected:
	//クラス名(デバック用)
	const char* name = nullptr;

	bool isLendModel_ = false;
	ObjModelManager* model_ = nullptr;
	ObjModelObject* object_ = nullptr;
	WorldTransform world_ = {};

	//コライダー
	BaseCollider* baseCollider_ = nullptr;

	//カメラ(借り物)
	Camera* camera_ = nullptr;

	//生存フラグ
	bool isAlive_ = false;

	//生成可能
	bool isPopsPosibble_ = true;

	//接触
	bool isContactTrigger_ = false;

	//光計算
	bool isLightCal_ = false;

	//リズム更新用
	//拍終了フラグ
	bool isBeatEnd_ = false;

	//サイズ変更用
	Vector3 scaleMax_ = {1, 1, 1};
	Vector3 scaleMin_ = {0.7f, 1, 0.7f};
	//スケール
	Vector3 scale_ = {scaleMax_};
	//現在時間
	float scaleCurrentTime_ = 0.f;
};

