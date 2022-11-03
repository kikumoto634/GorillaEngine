#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Window.h"

class RailCamera
{
public:
	static RailCamera* GetInstance();

	void Initialize(WorldTransform world, Vector3 Rot = {0,0,0});

	void  Update(Vector3 pos = {0,0,0}, Vector3 Rot = {0,0,0});

	//getter
	DirectX::XMMATRIX& GetMatProjection()	{return view.matProjection;}
	DirectX::XMMATRIX& GetMatView()	{return view.matView;}
	DirectX::XMMATRIX& GetViewProjectionMatrix()	{return view.matViewProjection;}


	Vector3& GetEye() {return view.eye; }
	Vector3& GetTarget() {return view.target; }
	Vector3& GetUp() {return view.up; }

	//setter
	void SetEye(const Vector3& eye)	{this->view.eye = eye; }
	void SetTarget(const Vector3& target)	{this->view.target = target; }
	void SetUp(const Vector3& up)	{this->view.up = up; }

private:
	Window* window;

	WorldTransform world;
	ViewProjection view;
};

