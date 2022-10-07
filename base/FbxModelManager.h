#pragma once

#include "Vector3.h"

#include <DirectXMath.h>
#include <string>
#include <wrl.h>
#include <DirectXTex.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <fbxsdk.h>

struct Node{
	//名前
	std::string name;
	DirectX::XMVECTOR scaling = {1,1,1,0};
	DirectX::XMVECTOR rotation = {0,0,0,0};
	DirectX::XMVECTOR translation = {0,0,0,1};
	DirectX::XMMATRIX transform;
	DirectX::XMMATRIX globalTransform;
	Node* parent = nullptr;
};

struct Bone{
	std::string name;
	//初期姿勢
	DirectX::XMMATRIX invInitialPose;
	//クラスター
	FbxCluster* fbxCluster;
	//コンストラクタ
	Bone(const std::string& name){
		this->name = name;
	}
};

/// <summary>
/// モデル
/// </summary>
class FbxModelManager
{
/// <summary>
/// 定数
/// </summary>
public:
	static const int MAX_BONE_INDICES = 4;

/// <summary>
/// エイリアス
/// </summary>
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	template <class T> using vector = std::vector<T>;
	using XMMATRIX = DirectX::XMMATRIX;
	using TexMetadata = DirectX::TexMetadata;
	using ScratchImage = DirectX::ScratchImage;
	using string = std::string;

/// <summary>
/// サブクラス・フレンドクラス
/// </summary>
public:
	friend class FbxLoader;

	struct VertexPosNormalUvSkin
	{
		Vector3 pos;
		Vector3 normal;
		Vector3 uv;
		UINT boneIndex[MAX_BONE_INDICES];	//ボーン番号
		float boneWeight[MAX_BONE_INDICES];	//ボーン重み
	};

public:
	static FbxModelManager* GetInstance();

/// <summary>
/// メンバ関数
/// </summary>
public:

	/// <summary>
	/// バッファ生成
	/// </summary>
	void CreateBuffers();


	//getter

	//モデルの変形行列
	const XMMATRIX& GetModelTransform() {return meshNode->globalTransform; }
	//FBXシーン
	FbxScene* GetFbxScene()	{return fbxScene;}
	//ボーン
	vector<Bone>& GetBones() {return bones;}
	//インデックス
	uint16_t GetIndices()	{return (UINT)indices.size();}
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW GetvbView()	{return vbView;}
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW GetibView()		{return ibView;}
	//デスクリプタヒープ
	ID3D12DescriptorHeap* GetDescriptorHeap()	{return descriptorHeapSRV.Get();}

/// <summary>
/// メンバ変数
/// </summary>
private:
	//FBX
	FbxScene* fbxScene = nullptr;

	//モデル名
	string name;
	//ノード配列
	vector<Node> nodes;

	//メッシュを持つノード
	Node* meshNode = nullptr;

	//頂点データ
	vector<VertexPosNormalUvSkin> vertices;
	//インデックス
	vector<uint16_t> indices;

	//頂点バッファ
	ComPtr<ID3D12Resource> verticesBuffer;
	//インデックスバッファ
	ComPtr<ID3D12Resource> indexBuffer;
	//テクスチャバッファ
	ComPtr<ID3D12Resource> textureBuffer;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView{};

	//頂点マップ
	VertexPosNormalUvSkin* verticesMap = nullptr;
	//インデックスマップ
	uint16_t* indicesMap = nullptr;

	//SRV用デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descriptorHeapSRV;

	//ボーン配列
	vector<Bone> bones;

	//アンビエント係数
	Vector3 ambient = {1,1,1};
	//ディヒューズ係数
	Vector3 diffuse = {1,1,1};
	//テクスチャメタデータ
	TexMetadata metaData{};
	//スクラッチイメージ
	ScratchImage scratchImage{};
};

