#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <d3dx12.h>
#include <string>
#include <unordered_map>

#include "../math/Vector/Vector2.h"
#include "../math/Vector/Vector3.h"
#include "../math/Vector/Vector4.h"
#include "DirectXCommon.h"

class ObjModelManager{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	//マテリアル
	struct Material
	{
		std::string name;//マテリアル名
		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;
		float alpha;
		std::string textureFilename;	//テクスチャファイル
		//コンストラクタ
		Material(){
			ambient = {0.3f, 0.3f, 0.3f};
			diffuse = {0.0f, 0.0f, 0.0f};
			specular = {0.0f, 0.0f, 0.0f};
			alpha = 1.0f;
		}
	};

public:
	static void StaticInitialize(DirectXCommon* dxCommon_);

public:
	~ObjModelManager();

	void CreateModel(std::string filePath, bool smmothing = false);

	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// エッジ平坦化データの追加
	/// </summary>
	/// <param name="indexPosition">座標インデックス</param>
	/// <param name="indexVertex">頂点インデックス</param>
	void AddSmmpthData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// 平坦化された頂点法線の計算
	/// </summary>
	void CalculateSmoothedVertexNormals();

	//Getter
	Material GetMaterial()	{return material;}

	/// <summary>
	/// 頂点データ数を取得
	/// </summary>
	/// <returns>頂点データの数</returns>
	inline size_t GetVertexCount()	{return vertices.size();}

private:
	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

private:

private:
	// デバイス
	static DirectXCommon* dxCommon;

private:
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;
	// 頂点インデックス配列
	std::vector<unsigned short> indices;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;

	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;

	//マテリアル
	Material material;

	//頂点用法線スムーシング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;
};