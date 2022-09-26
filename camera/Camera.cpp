#include "Camera.h"

using namespace DirectX;

Camera *Camera::GetInstance()
{
	static Camera camera;
	return &camera;
}

void Camera::Initialize()
{
	viewProjection.Initialize();
}

void Camera::Update()
{
	viewProjection.UpdateMatrix();
}
