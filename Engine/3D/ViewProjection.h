#pragma once
#include "../Engine/math/Vector/Vector3.h"

#include <DirectXMath.h>

class ViewProjection
{
public:
	using XMMATRIX = DirectX::XMMATRIX;

public:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(int width, int height);

public:
	//ビルボード行列
	static XMMATRIX matBillboard;
	//Y軸周りビルボード行列
	static XMMATRIX matBillboardY;

	//カメラ情報
	//透視投影
	XMMATRIX matProjection;	//プロジェクション行列
	//ビュー変換行列
	XMMATRIX matView;		//ビュー行列
	Vector3 eye;			//視点座標
	Vector3 target;		//注視点座標
	Vector3 up;			//上方向ベクトル

	XMMATRIX matViewProjection;
};

