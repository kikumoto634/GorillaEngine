#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"

class LightGroup
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//平行光源の数
	static const int DirectionLightNum = 3;
	//点光源の数
	static const int PointLightNum = 3;
	//スポットライトの数
	static const int SpotLightNum = 3;
	//丸影の数
	static const int CircleShadowNum = 1;

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
		//スポットライト用
		SpotLight::ConstBufferData spotLights[SpotLightNum];
		//丸影用
		CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
	};

//静的メンバ関数
public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns>インスタンス</returns>
	static LightGroup* Create();


private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;

public: // メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParameterIndex);

	/// <summary>
	/// 定数バッファ転送
	/// </summary>
	void TransferConstBuffer();

	/// <summary>
	/// 標準のライト設定
	/// </summary>
	void DefaultLightSetting();

	//Setter
	void SetAmbientColor(const Vector3& color);

	//平行
	void SetDirLightActive(int index, bool active);
	void SetDirLightDir(int index, const DirectX::XMVECTOR& lightdir);
	void SetDirLightColor(int index, const Vector3& lightcolor);

	//点
	void SetPointLightActive(int index, bool active);
	void SetPointLightPos(int index, const Vector3& lightpos);
	void SetPointLightColor(int index, const Vector3& lightcolor);
	void SetPointLightAtten(int index, const Vector3& lightAtten);

	//スポット
	void SetSpotLightActive(int index, bool active);
	void SetSpotLightDir(int index, const DirectX::XMVECTOR& lightdir);
	void SetSpotLightPos(int index, const Vector3& lightpos);
	void SetSpotLightColor(int index, const Vector3& lightcolor);
	void SetSpotLightAtten(int index, const Vector3& lightAtten);
	void SetSpotLightFactorAngleCos(int index, const Vector2& lightFactorAngleCos);

	//丸影
	void SetCircleShadowActive(int index, bool active);
	void SetCircleShadowCasterPos(int index, const Vector3& casterPos);
	void SetCircleShadowDir(int index, const DirectX::XMVECTOR& lightdir);
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);
	void SetCircleShadowAtten(int index, const Vector3& lightAtten);
	void SetCircleShadowFactorAngle(int index, const Vector2& lightFactorAngle);

private: // メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;

	// 環境光の色
	Vector3 ambientColor = { 1,1,1 };

	// 平行光源の配列
	DirectionalLight dirLights[DirectionLightNum];

	// 点光源の配列
	PointLight pointLights[PointLightNum];

	//スポットライトの配列
	SpotLight spotLights[SpotLightNum];

	//丸影の配列
	CircleShadow circleShadows[CircleShadowNum];

	// ダーティフラグ
	bool dirty = false;
};