#pragma once
#include <DirectXMath.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// スポットライト
/// </summary>
class CircleShadow
{
//サブクラス
public:
	//定数バッファ用データ構造
	struct ConstBufferData
	{
		DirectX::XMVECTOR dir;
		Vector3 casterPos;
		float distanceCasterLight;
		Vector3 atten;
		float pad3;
		Vector2 factorAngleCos;
		unsigned int active;
		float pad4;
	};

//メンバ関数
public:

	//Getter
	inline const DirectX::XMVECTOR& GetDir()	{return dir;}
	inline const float& GetDistanceCasterLight()	{return distanceCasterLight;}
	inline const Vector3& GetCasterPos()	{return casterPos;}
	inline const Vector3& GetAtten()	{return atten;}
	inline const Vector2& GetFactorAngleCos()	{return factorAngleCos;}
	inline bool GetActive()	{return active;}

	//Setter
	inline void SetDir(const DirectX::XMVECTOR& dir)	{this->dir = DirectX::XMVector3Normalize(dir);;}
	inline void SetDistanceCasterLight(const float& distanceCasterLight)	{this->distanceCasterLight = distanceCasterLight;}
	inline void SetCasterPos(const Vector3& casterPos)	{this->casterPos = casterPos;}
	inline void SetAtten(const Vector3& atten)	{this->atten = atten;}
	inline void SetFactorAngleCos(const Vector2& factorAngleCos)	
	{
		this->factorAngleCos.x = cosf(DirectX::XMConvertToRadians(factorAngleCos.x));
		this->factorAngleCos.y = cosf(DirectX::XMConvertToRadians(factorAngleCos.y));
	}
	inline void SetActive(bool active)	{this->active = active;}

//メンバ変数
private:
	//方向
	DirectX::XMVECTOR dir = {1,0,0,0};
	//キャスターとライト距離
	float distanceCasterLight = 100.0f;
	//キャスター座標
	Vector3 casterPos = {0,0,0};
	//距離減衰係数
	Vector3 atten = {0.5f, 0.6f, 0.0f};
	//減衰角度(開始角度、終了角度)
	Vector2 factorAngleCos = {0.2f, 0.5f};
	//有効フラグ
	bool active = false;
};