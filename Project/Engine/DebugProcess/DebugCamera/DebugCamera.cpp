#include "DebugCamera.h"

DebugCamera *DebugCamera::GetInstance()
{
	static DebugCamera instance;
	return &instance;
}

void DebugCamera::Initialize()
{
	input = Input::GetInstance();

	world_.translation = {0,0,-10};
	world_.rotation = {0,0,0};
	world_.Initialize();
}

void DebugCamera::Update()
{
	//座標移動
	{
		if(input->MouseTrigger(0)){
			mousePos = input->GetMousePos();
		}
		else if(input->MousePush(0)){
			Vector2 moveValue = (input->GetMousePos() - mousePos) * 0.001f;
			world_.translation.x += moveValue.x;
			world_.translation.y -= moveValue.y;
		}
	}
	//ズーム
	{
		world_.translation.z += input->GetMouseWheel()*0.01f;
	}

	//回転
	{
		if(input->MouseTrigger(1)){
			mouseRot = input->GetMousePos();
		}
		else if(input->MousePush(1)){
			Vector2 moveValue = (input->GetMousePos() - mouseRot) * 0.0001f;
			world_.rotation.x += moveValue.y;
			world_.rotation.y += moveValue.x;
		}
	}

	world_.UpdateMatrix();
}
