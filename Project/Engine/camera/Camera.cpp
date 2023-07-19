#include "Camera.h"
#include "Easing.h"

#include <typeinfo>

using namespace DirectX;

Camera *Camera::GetInstance()
{
	static Camera instance;
	return &instance;
}

void Camera::Initialize(Window* window)
{
	this->window = window;
	//クラス名の文字列を取得
	name = typeid(*this).name();

	//カメラ
	world.translation = {0,0,-distance};
	world.rotation = {0,0,0};
	world.Initialize();

	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
	view.matView = XMMatrixInverse(nullptr, world.matWorld);
	view.UpdateViewMatrix();
}

void Camera::Update()
{
	Shake();
	
	view.matView = XMMatrixInverse(nullptr, world.matWorld);
	view.UpdateViewMatrix();
}

void Camera::Tracking(Vector3 target, Vector3 offset)
{
	Vector3 cameraPosXZ = view.eye - view.target;
	float height = cameraPosXZ.y;
	cameraPosXZ.y = 0.0f;
	float cameraPosXZLen = cameraPosXZ.length();
	cameraPosXZ.normalize();

	Vector3 ltarget = target;
	ltarget.y += 0.0f;

	Vector3 newCameraPos = view.target - ltarget;
	newCameraPos.y = 50.0f;
	newCameraPos.normalize();

	float weight = 0.0f;
	newCameraPos = newCameraPos * weight + cameraPosXZ * (1.0f - weight);
	newCameraPos.normalize();
	newCameraPos *= cameraPosXZLen;
	newCameraPos.y = height;
	Vector3 pos = ltarget + newCameraPos;

	SetPosition(pos + offset);
}




void Camera::ShakeStart()
{
	if(isShake) return;

	isShake = true;
	savePos = GetPosition();

	shakeMaxPower = shakeMaxPower<<4;
}


void Camera::Shake()
{
	if(!isShake) return;

	world.translation = savePos;

	if(shakeFrame >= shakeMaxFrame){
		shakeFrame = 0;
		shakePower = 0;
		shakeMaxPower = shakeMaxPower>>4;
		isShake = false;
		return;
	}

	shakeFrame++;
	shakePower = (int)Easing_Point2_Linear((float)shakeMaxPower, 1.0f, shakeFrame/shakeMaxFrame);

	int randomPowerX = rand()%shakePower-(shakePower/2);
	float tempPowerX = float(randomPowerX)/16.f;

	int randomPowerY = rand()%shakePower-(shakePower/2);
	float tempPowerY = float(randomPowerY)/16.f;

	int randomPowerZ = rand()%shakePower-(shakePower/2);
	float tempPowerZ = float(randomPowerZ)/16.f;

	Vector3 temp = {tempPowerX,tempPowerY,tempPowerZ};
	Movement(temp);
}


