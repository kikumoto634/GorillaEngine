#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <DirectXTex.h>

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include <fbxsdk.h>

#include "Vector2.h"
#include "Vector3.h"

struct Node
{
	//名前
	std::string name;
	//ローカルスケール
	DirectX::XMVECTOR scaling = {1, 1, 1, 0};
	//ローカル回転角
	DirectX::XMVECTOR rotation = {0,0, 0, 0};
	//ローカル移動
	DirectX::XMVECTOR translation = {0, 0, 0, 1};
	//ローカル変形行列
	DirectX::XMMATRIX transform;
	//グローバル変形行列
	DirectX::XMMATRIX globalTransform;
	//親ノード
	Node* parent = nullptr;
};


//ボーン構造体
struct Bone
{
	//名前
	std::string name;
	//初期姿勢の逆行列
	DirectX::XMMATRIX invInitialPose;
	//クラスター(FBX側のボーン情報)
	FbxCluster* fbxCluster;
	//コンストラクタ
	Bone(const std::string& name)
	{
		this->name = name;
	}
};

/// <summary>
/// FBXモデル
/// </summary>
class FbxObjectManager
{
/// <summary>
/// 定数
/// </summary>
public:
	//ボーンインデックスの最大数
	static const int MAX_BONE_INDICES = 4;

/// <summary>
/// エイリアス
/// </summary>
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	using string = std::string;
	template <class T> using vector = std::vector<T>;

public:
	//フレンドクラス
	friend class FbxLoader;

	//サブクラス
	struct VertexPosNormalUvSkin
	{
		Vector3 pos;
		Vector3 normal;
		Vector3 uv;
		UINT boneIndex[MAX_BONE_INDICES];	//ボーン番号
		float boneWeight[MAX_BONE_INDICES];	//ボーン重み
	};

public:
	//デストラクタ
	~FbxObjectManager();

	/// <summary>
	/// バッファ生成
	/// </summary>
	/// <param name="device">デバイス</param>
	void CreateBuffers(ID3D12Device* device);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">グラフィックスコマンドリスト</param>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// モデルの変形行列取得
	/// </summary>
	/// <returns></returns>
	const XMMATRIX& GetModelTransform() {return meshNode->globalTransform; }

	//getter
	FbxScene* GetFbxScene()	{return fbxScene;}

	std::vector<Bone>& GetBones() {return bones;}

private:
	//FBXシーン
	FbxScene* fbxScene = nullptr;

	//モデル名
	std::string name;
	//ノード配列
	std::vector<Node> nodes;

	//メッシュを持つノード
	Node* meshNode = nullptr;
	//頂点データ配列
	std::vector<VertexPosNormalUvSkin> vertices;
	//頂点インデックス配列
	std::vector<unsigned short> indices;

	//アンビエント係数
	Vector3 ambient = {1, 1, 1};
	//ディヒューズ係数
	Vector3 diffuse = {1, 1, 1};
	//テクスチャメタデータ
	DirectX::TexMetadata metadata = {};
	//スクラッチイメージ
	DirectX::ScratchImage scratchImg = {};


	//頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> texBuff;
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeapSRV;

	//ボーン配列
	std::vector<Bone> bones;
};

