#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>


#include "Vector2.h"
#include "Vector3.h"

class GeometryObjectManager
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//定数
	//テクスチャの最大枚数
	static const int maxObjectCount = 512;

public:
	struct VertexPosNormalUv
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

public:
	static GeometryObjectManager* GetInstance();

public:
	/// <summary>
	/// バッファ生成
	/// </summary>
	void CreateBuffer();

	//Get
	
	int GetMaxObjectCount()	{return maxObjectCount;}
	
	//インデックス
	uint16_t GetIndices()	{return _countof(indices);}

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW GetvbView()	{return vbView;}
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW GetibView()		{return ibView;}


private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffer;

	//頂点マップ
	VertexPosNormalUv* vertMap = nullptr;
	//インデックスマップ
	uint16_t* indexMap = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//頂点データ
	VertexPosNormalUv vertices[24];
	//インデックスデータ
	uint16_t indices[36];
};

