#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

class ViewProjection
{
public:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(int width, int height);

public:
	//ビルボード行列
	static Matrix4x4 matBillboard;
	//Y軸周りビルボード行列
	static Matrix4x4 matBillboardY;

	//カメラ情報
	//透視投影
	Matrix4x4 matProjection;	//プロジェクション行列
	//ビュー変換行列
	Matrix4x4 matView;		//ビュー行列
	Vector3 eye;			//視点座標
	Vector3 target;		//注視点座標
	Vector3 up;			//上方向ベクトル

	Matrix4x4 matViewProjection;
};

