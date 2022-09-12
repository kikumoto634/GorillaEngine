#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <vector>

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Vector3.h"
#include "Vector2.h"

class GeometryManager
{
public:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//頂点データ
	struct VertexPosNormal{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(UINT texNumber);

	//Get
	const D3D12_VERTEX_BUFFER_VIEW& GetVbView()	{return vbView;}
	const D3D12_INDEX_BUFFER_VIEW& GetIbView()	{return ibView;}

private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffer;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};
	//頂点データ配列
	vector<VertexPosNormal> vertices;
	//頂点インデックス配列
	vector<unsigned short> indices;

	DirectXCommon* dxCommon = nullptr;
	TextureManager* textureManager = nullptr;
};

