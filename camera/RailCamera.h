#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Window.h"
#include <Matrix4.cpp>

class RailCamera
{
public:
	static RailCamera* GetInstance();

	void Initialize(WorldTransform world, Vector3 Rot = {0,0,0});

	void  Update(Vector3 pos = {0,0,0}, Vector3 Rot = {0,0,0});

	//getter
	Matrix4& GetMatProjection()	{return view.matProjection;}
	Matrix4& GetMatView()	{return view.matView;}
	Matrix4& GetViewProjectionMatrix()	{return view.matViewProjection;}


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

