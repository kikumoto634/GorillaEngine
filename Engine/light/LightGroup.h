#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"

class LightGroup
{
public:
	//平行光源の数
	static const int DirectionLightNum = 1;
	//点光源の数
	static const int PointLightNum = 3;

public:
	//定数バッファ用データ構造体
	struct ConstBufferData
	{
		//環境光の色
		Vector3 ambientColor;
		float pad1;
		//平行光源用
		DirectionalLight::ConstBufferData dirLights[DirectionLightNum];
		//点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
	};

//メンバ関数
public:
	
	//Setter
	void SetPointLightActive(int index, bool IsActive);
	void SetPointLightPos(int index, const Vector3& lightpos);
	void SetPointLightColor(int index, const Vector3& lightcolor);
	void SetPointLightAtten(int index, const Vector3& lightAtten);

//メンバ変数
private:
	//点光源の配列
	PointLight pointLights[PointLightNum];
};

