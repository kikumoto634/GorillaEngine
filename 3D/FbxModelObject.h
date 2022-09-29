#pragma once
#include "FbxModelManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "FbxLoader.h"
#include "DirectXCommon.h"

#include <DirectXMath.h>
#include <wrl.h>

class FbxModelObject
{
/// <summary>
/// 定数
/// </summary>
public:
	static const int MAX_BONES = 32;

/// <summary>
/// エイリアス
/// </summary>
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMMATRIX = DirectX::XMMATRIX;

/// <summary>
/// サブクラス・インナークラス
/// </summary>
public:
	class CommonFbx{
		friend class FbxModelObject;

	public:
		/// <summary>
		/// グラフィックパイプライン初期化
		/// </summary>
		//void InitializeGraphicsPipeline();

		/// <summary>
		/// デストラクタヒープ初期化
		/// </summary>
		//void InitializeDescriptorHeap();

	private:
		//DirectXCommon
		DirectXCommon* dxCommon = nullptr;

		//パイプラインステートオブジェクト
		ComPtr<ID3D12PipelineState> pipelineState;
		//ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootSignature;

		FbxModelManager* FbxModelManager;

	};

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

public:
	/// <summary>
	/// 初期化
	/// </summary>
	//static void StaticInitialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 静的メンバ解法
	/// </summary>
	//static void StaticFinalize();

	//static FbxModelObject* Create();


/// <summary>
/// メンバ関数
/// </summary>
public:

	void Initialize();

	void Update(WorldTransform worldTransform, Camera* camera);

	void Draw();

/// <summary>
/// 静的メンバ変数
/// </summary>
private:
	//static CommonFbx* common;

/// <summary>
/// メンバ変数
/// </summary>
public:
	//定数バッファ
	ComPtr<ID3D12Resource> constBufferTransform;
	ComPtr<ID3D12Resource> constBufferSkin;

	//モデル
	FbxModelManager* model = nullptr;

	//アニメーション
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

