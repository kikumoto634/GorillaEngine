#include "ViewProjection.h"
#include <DirectXMath.h>

Matrix4x4 ViewProjection::matBillboard  = MakeIdentityMatrix();
Matrix4x4 ViewProjection::matBillboardY  = MakeIdentityMatrix();

void ViewProjection::UpdateViewMatrix()
{
	Vector4 leye = {eye,0};
	Vector4 ltarget = {target,0};
	Vector4 lup = {up,0};

	//カメラZ軸
	Vector4 cameraAxisZ = (leye - ltarget);
	//0ベクトルだと向きが定まらないので除外
	/*assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));*/
	//ベクトルを正規化
	cameraAxisZ = cameraAxisZ.Normalize();
	//カメラのX軸
	Vector4 cameraAxisX;
	//X軸は上方向->Z軸の外積で求まる
	cameraAxisX = lup.CorssVector3(cameraAxisZ);
	//ベクトルを正規化
	cameraAxisX = cameraAxisX.Normalize();
	//カメラのY軸
	Vector4 cameraAxisY;
	cameraAxisY = cameraAxisZ.CorssVector3(cameraAxisX);
	//カメラ行列
	//カメラ座標系->ワールド座標系の変換行列
	Matrix4x4 matCameraRot = {
		cameraAxisX.x, cameraAxisX.y, cameraAxisX.z, cameraAxisX.w,
		cameraAxisY.x, cameraAxisY.y, cameraAxisY.z, cameraAxisY.w,
		cameraAxisZ.x, cameraAxisZ.y, cameraAxisZ.z, cameraAxisZ.w,
		0,0,0,1
	};

	//ビュー変換行列
	matView = matCameraRot;

	//視点座標に-1を掛けた座標
	Vector4 reverseEyePosition = -(leye);
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	Vector4 tX = cameraAxisX*reverseEyePosition;//X成分
	Vector4 tY = cameraAxisY*reverseEyePosition;//Y成分
	Vector4 tZ = cameraAxisZ*reverseEyePosition;//Z成分
	//一つのベクトルにまとめる
	Vector4 translation = {tX.x, tY.y, tZ.z, 1.0f};

	//ビュー行列に平行移動成分を設定
	matView.m[3][0] = translation.x;
	matView.m[3][1] = translation.y;
	matView.m[3][2] = translation.z;
	matView.m[3][3] = translation.w;

#pragma region ビルボード計算
	//カメラX軸、Y軸、Z軸
	Vector4 ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;
	//X軸は共通
	ybillCameraAxisX = cameraAxisX;
	//Y軸はワールド座標系のY軸
	ybillCameraAxisY = lup.Normalize();
	//Z軸はX軸->Y軸の外積で求まる
	ybillCameraAxisZ = cameraAxisX.CorssVector3(cameraAxisY);

	//ビルボード行列計算
	matBillboard = {
		ybillCameraAxisX.x,ybillCameraAxisX.y,ybillCameraAxisX.z,ybillCameraAxisX.w,
		ybillCameraAxisY.x,ybillCameraAxisY.y,ybillCameraAxisY.z,ybillCameraAxisY.w,
		ybillCameraAxisZ.x,ybillCameraAxisZ.y,ybillCameraAxisZ.z,ybillCameraAxisZ.w,
		0,0,0,1
	};
#pragma endregion ビルボード計算
}

void ViewProjection::UpdateProjectionMatrix(int width, int height)
{
	//透視投影

	//matProjection.MakePerspective(
	//	DirectX::XMConvertToRadians(45.0f),	//上下画角45°
	//	(float)width / height,			//aspect比(画面横幅/画面縦幅)
	//	0.1f, 1000.0f				//前端、奥端
	//);

	matProjection.SetXMMATRIX(DirectX::XMMatrixPerspectiveFovLH(
		DirectX::XMConvertToRadians(45.0f),	//上下画角45°
		(float)width / height,			//aspect比(画面横幅/画面縦幅)
		0.1f, 1000.0f				//前端、奥端
	));
}
