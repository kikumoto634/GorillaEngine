#include "Camera.h"

//using namespace DirectX;

Camera *Camera::GetInstance()
{
	static Camera instance;
	return &instance;
}

void Camera::Initialize()
{
	window = Window::GetInstance();

	const float distance = 20.f;	//ƒJƒƒ‰‚Ì‹——£

	//ƒJƒƒ‰
	view.eye = {0, 0, -distance};
	view.target = {0, 0, 0};
	view.up = {0, 1, 0};


	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}

void Camera::Update()
{
	view.matViewProjection = view.matView * view.matProjection;

	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}


