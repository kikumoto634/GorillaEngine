#pragma once
#include "Camera.h"
#include "Input.h"

/// <summary>
/// 追従カメラ
/// </summary>
class FollowCamera : public Camera
{
public:
	static FollowCamera* GetInstance();

	void Initialize(Vector3 pos = {0,0,0}, Vector3 rot = {0,0,0}) override;
	void Update(WorldTransform target, Vector3 offset = {0,0,-20});

private:
	void Movement();
	void Rotation();

private:
	Input* input_ = nullptr;

	Vector3 target_ = {};
	Vector3 offset_ = {};

	//回転値
	Vector2 rotation = {};
};

