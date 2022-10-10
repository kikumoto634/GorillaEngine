#pragma once

#include "fbxsdk.h"
#include "FbxModelManager.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

class FbxLoader
{
private:
	using string = std::string;
public:
	static const string baseDirectory;
	static const string defaultTextureFileName;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize();

	/// <summary>
	/// fileからFBXモデル読み込み
	/// </summary>
	/// <param name="modelName">モデル名</param>
	FbxModelManager* LoadModelFromFile(const string& modelName);

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="fbxNode">ノード</param>
	void ParseNodeRecursive(FbxModelManager* model, FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="fbxNode">ノード</param>
	void ParseMesh(FbxModelManager* model, FbxNode* fbxNode);

	//頂点座標読み取り
	void ParseMeshVertices(FbxModelManager* model, FbxMesh* fbxMesh);

	//面情報読み取り
	void ParseMeshFaces(FbxModelManager* model, FbxMesh* fbxMesh);

	//マテリアル読み取り
	void ParseMaterial(FbxModelManager* model, FbxNode* fbxNode);

	//テクスチャ読み込み
	void LoadTexture(FbxModelManager* model, const std::string& fullpath);

	//ディレクトリを含んだfileパスからファイル名を抽出する。
	std::string ExtractFileName(const std::string& path);

private:
	//デバイス
	ID3D12Device* device = nullptr;
	//マネージャー
	FbxManager* fbxManager = nullptr;
	//インポータ
	FbxImporter* fbxImporter = nullptr;
};