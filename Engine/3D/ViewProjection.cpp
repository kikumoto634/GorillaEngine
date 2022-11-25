#include "ViewProjection.h"

using namespace DirectX;

XMMATRIX ViewProjection::matBillboard  = DirectX::XMMatrixIdentity();
XMMATRIX ViewProjection::matBillboardY  = DirectX::XMMatrixIdentity();

void ViewProjection::UpdateViewMatrix()
{
	XMFLOAT3 leye = {eye.x,eye.y,eye.z};
	XMFLOAT3 ltarget = {target.x,target.y,target.z};
	XMFLOAT3 lup = {up.x,up.y,up.z};

	//視点座標
	XMVECTOR eyePosition = XMLoadFloat3(&leye);
	XMVECTOR targetPosition = XMLoadFloat3(&ltarget);
	XMVECTOR upVector = XMLoadFloat3(&lup);

	//カメラZ軸
	XMVECTOR cameraAxisZ = XMVectorSubtract(targetPosition, eyePosition);
	//0ベクトルだと向きが定まらないので除外
	assert(!XMVector3Equal(cameraAxisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(cameraAxisZ));
	assert(!XMVector3Equal(upVector, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVector));
	//ベクトルを正規化
	cameraAxisZ = XMVector3Normalize(cameraAxisZ);
	//カメラのX軸
	XMVECTOR cameraAxisX;
	//X軸は上方向->Z軸の外積で求まる
	cameraAxisX = XMVector3Cross(upVector, cameraAxisZ);
	//ベクトルを正規化
	cameraAxisX = XMVector3Normalize(cameraAxisX);
	//カメラのY軸
	XMVECTOR cameraAxisY;
	cameraAxisY = XMVector3Cross(cameraAxisZ, cameraAxisX);
	//カメラ行列
	XMMATRIX matCameraRot;
	//カメラ座標系->ワールド座標系の変換行列
	matCameraRot.r[0] = cameraAxisX;
	matCameraRot.r[1] = cameraAxisY;
	matCameraRot.r[2] = cameraAxisZ;
	matCameraRot.r[3] = XMVectorSet(0,0,0,1);

	//ビュー変換行列
	matView = XMMatrixTranspose(matCameraRot);

	//視点座標に-1を掛けた座標
	XMVECTOR reverseEyePosition = XMVectorNegate(eyePosition);
	//カメラの位置からワールド原点へのベクトル(カメラ座標系)
	XMVECTOR tX = XMVector3Dot(cameraAxisX, reverseEyePosition);//X成分
	XMVECTOR tY = XMVector3Dot(cameraAxisY, reverseEyePosition);//Y成分
	XMVECTOR tZ = XMVector3Dot(cameraAxisZ, reverseEyePosition);//Z成分
	//一つのベクトルにまとめる
	XMVECTOR translation = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);

	//ビュー行列に平行移動成分を設定
	matView.r[3] = translation;

#pragma region ビルボード計算
	//カメラX軸、Y軸、Z軸
	XMVECTOR ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;
	//X軸は共通
	ybillCameraAxisX = cameraAxisX;
	//Y軸はワールド座標系のY軸
	ybillCameraAxisY = XMVector3Normalize(upVector);
	//Z軸はX軸->Y軸の外積で求まる
	ybillCameraAxisZ = XMVector3Cross(cameraAxisX, cameraAxisY);

	//ビルボード行列計算
	matBillboard.r[0] = ybillCameraAxisX;
	matBillboard.r[1] = ybillCameraAxisY;
	matBillboard.r[2] = ybillCameraAxisZ;
	matBillboard.r[3] = XMVectorSet(0,0,0,1);
#pragma endregion ビルボード計算
}

void ViewProjection::UpdateProjectionMatrix(int width, int height)
{
	//透視投影
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),	//上下画角45°
		(float)width / height,			//aspect比(画面横幅/画面縦幅)
		0.1f, 1000.0f				//前端、奥端
	);
}
