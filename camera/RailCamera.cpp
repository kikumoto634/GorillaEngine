#include "RailCamera.h"

RailCamera *RailCamera::GetInstance()
{
	static RailCamera instance;
	return &instance;
}

void RailCamera::Initialize(WorldTransform world, Vector3 Rot)
{
	window = Window::GetInstance();
	this->world = world;
	this->world.rotation = Rot;
	//更新
	this->world.UpdateMatrix();

	const float distance = 8.f;	//カメラの距離
	//カメラ
	view.eye = {0, 0, -distance};
	view.target = {0, 2.5f, 0};
	view.up = {0, 1, 0};
	//更新
	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}

void RailCamera::Update(Vector3 pos, Vector3 Rot)
{
	world.translation = pos;
	world.rotation = Rot;
	//更新
	world.UpdateMatrix();

	
	/*view.eye = world.translation;
	Vector3 forward ={0,0,1};
	forward = ;
	view.target= view.eye + forward;
	Vector3 up = {0,1,0};
	view.up = ;*/


	view.matViewProjection = view.matView * view.matProjection;
	//更新
	view.UpdateViewMatrix();
	view.UpdateProjectionMatrix(window->GetWindowWidth(),window->GetWindowHeight());
}
