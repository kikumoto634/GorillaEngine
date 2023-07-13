#pragma once
#include "BaseObjObject.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"
#include "ExitSprite.h"

class Exit : public BaseObjObject
{
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;

private:
	//描画範囲(暗めの表示)
	const int DrawingRange_Half = 8;

	//const int TexNumberBegin = 3;	//テクスチャ番号
	//const int CoinSpNumSize = 3;	//サイズ

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns></returns>
	void Initialize(std::string filePath, bool IsSmmothing = false) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Camera* camera) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw2D();

	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	/// <param name="info">衝突情報</param>
	void OnCollision(const CollisionInfo& info) override;

	//当たり判定時処理
	void ContactUpdate() override;

	/// <summary>
	/// 出口解放
	/// </summary>
	void ExitOpen()	{isOpen_ = true;}

	/// <summary>
	/// 出口施錠
	/// </summary>
	void ExitClose()	{isOpen_ = false;}

	//必要コイン更新
	void NeedCoinSpriteUpdate() {sp_->NeedCoinSpriteUpdate();}

	//モデル変更
	void ModelChange()	{this->object_->SetModel(stairsModel_), isOpenAudioOnce_ = true;}

	//Getter
	inline bool GetIsPlayerContact() {return isPlayerContact_;}
	inline const int GetExitNeedCoinNum() {return sp_->GetExitNeedCoinNum();}
	inline bool GetIsOpenAudioOnce()	{return isOpenAudioOnce_;}
	inline BaseSprites* GetCoinSp()	{return sp_->GetCoinSp();}

	//Setter
	inline void SetCoinSpPosition(Vector2 pos)	{sp_->SetCoinSpPosition(pos);}
	inline void SetExitOpenNeedCoin(int num)	{sp_->SetExitOpenNeedCoin(num);}

private:
	//コライダー
	void ColliderSet();

private:

	//スプライト
	unique_ptr<ExitSprite> sp_;

	//プレイヤー接触
	bool isPlayerContact_ = false;

	//開閉
	bool isOpen_ = false;

	//階段モデル
	ObjModelManager* stairsModel_ = nullptr;
	bool isModelChange_ = false;
	bool isOpenAudioOnce_ = false;

	//コライダー
	MeshCollider* meshCollider_ = nullptr;
};

