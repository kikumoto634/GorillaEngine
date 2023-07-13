#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#include "Vector2.h"
#include "Vector3.h"

//定数バッファ用データ構造体
struct ConstBufferDataWorldTransform{
	DirectX::XMMATRIX matWorld;	//ローカル -> ワールド変換行列
};

/// <summary>
/// ワールド変換行列
/// </summary>
struct WorldTransform{
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffer;
	//マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap = nullptr;

	//ローカルスケール
	Vector3 scale = {1,1,1};
	//ローカル回転角
	Vector3 rotation = {0,0,0};
	//ローカル座標
	Vector3 translation = {0,0,0};
	//ローカル -> ワールド変換行列
	DirectX::XMMATRIX matWorld = {};
	//親
	WorldTransform* parent = nullptr;


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピング
	/// </summary>
	void Map();

	/// <summary>
	/// 行列の更新
	/// </summary>
	void UpdateMatrix(DirectX::XMMATRIX matBillboard = DirectX::XMMatrixIdentity());
};

