#pragma once

#include "fbxsdk.h"

#include <d3d12.h>
#include <d3dx12.h>

#include <string>
#include "FbxModelManager.h"

class FbxLoader
{
/// <summary>
/// エイリアス
/// </summary>
private:
	using string = std::string;

/// <summary>
/// 定数
/// </summary>
public:
	//モデル格納ルートパス
	static const string baseDirectory;

	//テクスチャがない場合の標準テクスチャファイル名
	static const string defaultTextureFileName;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// FBXの行列をXMAtrixに変換
	/// </summary>
	/// <param name="dst">書き込み先</param>
	/// <param name="src">もととなるFBX行列</param>
	static void ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src);

private:
	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader& obj) = delete;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// ファイルからFBXモデル読込
	/// </summary>
	/// <param name="modelName">モデル名</param>
	FbxModelManager* LoadModeFromFile(const string& modelName);

	/// <summary>
	/// 解放
	/// </summary>
	void Finalize();

private:
	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name="model">読込先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseNodeRecursive(FbxModelManager* model,FbxNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseMesh(FbxModelManager* model, FbxNode* fbxNode);

	//頂点座標読み取り
	void ParseMeshVertices(FbxModelManager* model, FbxMesh* fbxMesh);

	//面情報読み取り
	void ParseMeshFaces(FbxModelManager* model,FbxMesh* fbxMesh);

	//マテリアル読み取り
	void ParseMaterial(FbxModelManager* model, FbxNode* fbxNode);

	//テクスチャ読み込み
	void LoadTexture(FbxModelManager* model, const std::string& fullpath);

	//スキニング情報の読み取り
	void ParseSkin(FbxModelManager* model,FbxMesh* fbxMesh);

	//ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);

private:
	//デバイス
	ID3D12Device* device = nullptr;
	
	//FBXマネージャー
	FbxManager* fbxManager = nullptr;

	//FBXインポータ
	FbxImporter* fbxImporter = nullptr;
};