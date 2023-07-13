#pragma once

#include <wrl.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "DirectXCommon.h"
#include "ObjModelMesh.h"

class ObjModelManager{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:
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
			ambient = {1.0f, 1.0f, 1.0f};
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

	//Getter
	
	/// <summary>
	/// マテリアル取得
	/// </summary>
	/// <returns></returns>
	Material GetMaterial()	{return material;}

	/// <summary>
	/// メッシュコンテナ取得
	/// </summary>
	/// <returns>マッシュコンテナ</returns>
	inline const std::vector<ObjModelMesh*>& GetMeshes() {return meshs;}

private:

	/// <summary>
	/// モデル読み込み
	/// </summary>
	void LoadModel(const std::string& filePath, bool smmothing);

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

	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;

	//メッシュ
	std::vector<ObjModelMesh*> meshs;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;

	//マテリアル
	Material material;
};