#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>


#include "../Engine/math/Vector/Vector2.h"
#include "../Engine/math/Vector/Vector3.h"

class GeometryObjectManager
{
public://エイリアス
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:	//定数
	//テクスチャの最大枚数
	static const int maxObjectCount = 512;
	static const int vertexCount = 1;
public:
	struct VertexPos{
		Vector3 pos;
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
	//頂点情報
	int Getvertices()	{return _countof(vertices);}
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW GetvbView()	{return vbView;}


private:
	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuffer;
	//頂点マップ
	VertexPos* vertMap = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//頂点データ
	VertexPos vertices[vertexCount];
};

