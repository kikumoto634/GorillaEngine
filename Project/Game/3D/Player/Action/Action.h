#pragma once
#include "BaseObjObject.h"
#include "Input.h"


class Action : public BaseObjObject
{
public:
	void Initialize();
	void Update(Camera* camera);

private:
	void Rotation();
	void Movement();

private:
	Input* input = nullptr;

	Vector3 moveValue = {};
};

