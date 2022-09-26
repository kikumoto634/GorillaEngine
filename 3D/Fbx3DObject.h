#pragma once
#include "FbxObjectManager.h"
#include "Camera.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>

#include "FbxLoader.h"

class Fbx3DObject
{
/// <summary>
/// 定数
/// </summary>
public:
	//ボーンの最大数
	static const int MAX_BONES = 32;

/// <summary>
/// エイリアス
/// </summary>
protected:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	//サブクラス
	//定数バッファ用データ構造体(座標変換行列用)
	struct ConstBufferDataTransform
	{
		XMMATRIX viewproj;	//ビュープロジェクション
		XMMATRIX world;		//ワールド行列
		Vector3 cameraPos;	//カメラ座標(ワールド座標)
	};

	//定数バッファ用データ構造体(スキニング)
	struct ConstBufferDataSkin
	{
		XMMATRIX bones[MAX_BONES];
	};

/// <summary>
/// 静的メンバ関数
/// </summary>
public:
	//setter
	static void SetDevice(ID3D12Device* device) {Fbx3DObject::device = device; }
	static void SetCamera(Camera* camera)	{Fbx3DObject::camera = camera;}

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	static void CreateGraphicsPipeline();

/// <summary>
/// メンバ関数
/// </summary>
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// モデルセット
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(FbxObjectManager* model)	{this->model = model;}

	/// <summary>
	/// アニメーション開始
	/// </summary>
	void PlayAnimation();


	//setter
	void SetScale(Vector3 scale)	{this->scale = scale;}
	void SetRotation(Vector3 rotation)	{this->rotation = rotation;}
	void SetPosition(Vector3 position)	{this->position = position;}

	//getter
	Vector3 GetScale()	{return this->scale;}
	Vector3 GetRotation()	{return this->rotation;}
	Vector3 GetPosition()	{return this->position;}


/// <summary>
/// 静的メンバ変数
/// </summary>
private:
	static ID3D12Device* device;
	static Camera* camera;

	//ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	//パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;

/// <summary>
/// メンバ変数
/// </summary>
private:

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffTransform;
	//定数バッファ(スキン)
	ComPtr<ID3D12Resource> constBufferSkin;

	//ローカルスケール
	Vector3 scale = {1, 1, 1};
	//X,Y,Z軸周りのローカル回転角
	Vector3 rotation = {0, 0, 0};
	//ローカル座標
	Vector3 position = {0, 0, 0};
	//ローカルワールド変換行列
	XMMATRIX matWorld{};
	//モデル
	FbxObjectManager* model = nullptr;

	//1frameの時間
	FbxTime frameTime;
	//アニメーション開始時間
	FbxTime startTime;
	//アニメーション終了時間
	FbxTime endTime;
	//現在時間(アニメーション)
	FbxTime currentTime;
	//アニメーション再生中
	bool isPlay = false;
};

