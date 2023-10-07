#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "DirectXCommon.h"

class DirectionalLight
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//サブクラス
public:
	struct ConstBufferData
	{
		Vector3 lightv;	//ライト向き
		float pad;
		Vector3 lightcolor;			//ライト色
		unsigned int active;
	};


//メンバ関数
public:

	//Getter
	inline const Vector3 GetLightDir()	{return lightdir.normalize();}
	inline const Vector3& GetLightColor()	{return lightcolor;}
	inline const bool& GetIsActive()	{return IsActive;}

	//Setter
	inline void SetLightDir(const Vector3& lightdir)	{this->lightdir = lightdir;}
	inline void SetLightColor(const Vector3 lightcolor)	{this->lightcolor = lightcolor;}
	inline void SetIsActive(const bool IsActive)	{this->IsActive = IsActive;}

//静的メンバ変数
private:
	static DirectXCommon* dxCommon;

//メンバ変数
private:
	//ライト光線方向
	Vector3 lightdir = {1,0,0};
	//ライト色
	Vector3 lightcolor = {1,1,1};
	//ダーティフラグ
	bool IsActive = false;
};

