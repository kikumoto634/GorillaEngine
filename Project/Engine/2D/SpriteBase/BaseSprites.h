#pragma once
#include "Sprite.h"
#include "Vector2.h"
#include "Camera.h"

class BaseSprites
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(UINT textureNumber);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 後処理
	/// </summary>
	virtual void Finalize();

	//Getter
	const Vector2& GetPosition()	{return position;}
	const Vector2& GetSize()	{return size;}
	const Vector3& GetColor()	{return {color.x,color.y,color.z};}
	const UINT GetTextureNumber()	{return sprite->GetTexNumber();}

	//Setter
	void SetPosition(const Vector2& position)	{this->position = position;}
	void SetSize(const Vector2& size)	{this->size = size;}
	void SetColor(DirectX::XMFLOAT4 color)	{this->color = color;}
	void SetTexNumber(UINT texNumber)	{sprite->SetTexNumber(texNumber);}
	void SetAnchorPoint(Vector2 anchorPoint)	{sprite->SetAnchorpoint(anchorPoint);}

public:
	/// <summary>
	/// リズムにあわせてサイズ変更 
	/// </summary>
	/// <param name="sizeMax">最大サイズ</param>
	/// <param name="sizeMin">最小サイズ</param>
	/// <param name="EndTime">終了時間 (x:秒)</param>
	/// <returns></returns>
	bool ScaleChange(Vector2 sizeMax = {1.f,1.f}, Vector2 sizeMin = {0.7f,0.7f}, float EndTime = 0.25f);

	//3D->2D変換
	Vector2 ChangeTransformation(Vector3 target, Camera* camera);

private:
	//ベクトル*行列
	Vector3 Vector3Transform( Vector3 &v,  DirectX::XMMATRIX &m);

private:
	Window* window = nullptr;

protected:
	Sprite* sprite = nullptr;
	Vector2 position = {0,0};
	Vector2 size = {100,100};
	DirectX::XMFLOAT4 color = {1,1,1,1};

	Vector3 targetPos = {};

	//リズム更新用
	//拍終了フラグ
	bool IsBeatEnd = false;

	//サイズ変更用
	//サイズ
	Vector2 baseSize = {1,1};
	Vector2 scale = {1,1};
	//現在時間
	float scaleCurrentTime = 0.f;
};