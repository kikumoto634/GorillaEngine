#pragma once

#include <DirectXMath.h>

#include "Window.h"
#include "Vector3.h"


/// <summary>
/// ビュープロジェクション変換データ
/// </summary>
struct ViewProjection{


	//アスペクト用
	Window* window = Window::GetInstance();

#pragma region ビュー行列
	//視点座標
	Vector3 eye = {0,0,-100.f};
	//注視点座標
	Vector3 target = {0,20,0};
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
	/// 行列更新
	/// </summary>
	void UpdateMatrix();
};