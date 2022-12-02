#pragma once
#include <wrl.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "../math//Vector/Vector2.h"
#include "../math//Vector/Vector3.h"
#include "../math//Vector/Vector4.h"
#include "../base/DirectXCommon.h"

class Light
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

//サブクラス
public:
	struct ConstBufferData
	{
		DirectX::XMVECTOR lightv;	//ライト向き
		Vector3 lightcolor;			//ライト色
	};

//静的メンバ関数
public:
	static Light* Create();

	static void StaticInitialize(DirectXCommon* dxCommon);

//メンバ関数
public:
	void Initialize();

	void Update();

	void Draw(UINT rootParameterIndex);

	void TransferConstBuffer();

	//Setter
	void SetLightDir(const DirectX::XMVECTOR& lightdir);
	void SetLightColor(const Vector3 lightcolor);

//静的メンバ変数
private:
	static DirectXCommon* dxCommon;

//メンバ変数
private:
	//定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	//ライト光線方向
	DirectX::XMVECTOR lightdir = {1,0,0,0};
	//ライト色
	Vector3 lightcolor = {1,1,1};
	//ダーティフラグ
	bool IsDirty = false;
};

