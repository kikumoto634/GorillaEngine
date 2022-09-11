#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#include "Window.h"
#include "Vector2.h"
#include "Vector3.h"

//定数バッファ用データ構造
struct ConstBufferDataViewProjection{
	DirectX::XMMATRIX view;			//ワールド -> ビュー変換行列
	DirectX::XMMATRIX projection;	//ビュー -> プロジェクション変換行列
	Vector3 cameraPos;				//カメラ座標 (ワールド座標)
};

/// <summary>
/// ビュープロジェクション変換データ
/// </summary>
struct ViewProjection{

	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuffer;
	//マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;

	//アスペクト用
	Window* window = Window::GetInstance();

#pragma region ビュー行列
	//視点座標
	Vector3 eye = {0,0,-50.f};
	//注視点座標
	Vector3 target = {0,0,0};
	//上方向ベクトル
	Vector3 up = {0,1,0};
#pragma endregion

#pragma region 射影変換行列
	//垂直方向視野角
	float fovAngleY = DirectX::XMConvertToRadians(45.f);
	//ビューポートのアスペクト比
	float aspectRatio = (float)window->GetWindowWidth()/(float)window->GetWindowHeight();
	//深度限界(手前)
	float nearZ = 0.1f;
	//深度限界(奥)
	float farZ = 1000.f;
#pragma endregion

	//ビュー行列
	DirectX::XMMATRIX matView;
	//射影行列
	DirectX::XMMATRIX matProjection;

	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピング
	/// </summary>
	void Map();

	/// <summary>
	/// 行列更新
	/// </summary>
	void UpdateMatrix();
};