#pragma once
#include <DirectXMath.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// 光光源
/// </summary>
class PointLight
{
//サブクラス
public:
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector3 lightpos;
		float pad1;
		Vector3 lightcolor;
		float pad2;
		Vector3 lightatten;
		unsigned int active;
	};

//メンバ関数
public:

	//Getter
	inline const Vector3& GetLightPos()	{return lightpos;}
	inline const Vector3& GetLightColor()	{return lightcolor;}
	inline const Vector3& GetLightAten()	{return lightAtten;}
	inline bool GetIsActive()	{return IsActive;}

	//Setter
	inline void SetLightPos(const Vector3& lightpos)	{this->lightpos = lightpos;}
	inline void SetLightColor(const Vector3& lightcolor)	{this->lightcolor = lightcolor;}
	inline void SetLightAtten(const Vector3& lightAtten)	{this->lightAtten = lightAtten;}
	inline void SetIsActive(bool IsActive)	{this->IsActive = IsActive;}

//メンバ変数
private:
	//ライト座標
	Vector3 lightpos = {0,0,0};
	//ライト色
	Vector3 lightcolor = {1,1,1};
	//ライト距離減衰係数
	Vector3 lightAtten = {1.0f, 1.0f, 1.0f};
	//有効フラグ
	bool IsActive = false;
};

