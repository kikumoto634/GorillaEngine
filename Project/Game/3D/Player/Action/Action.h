#pragma once
#include "BaseObjObject.h"
#include "Input.h"

#include "MeshCollider.h"


class Action : public BaseObjObject
{
public:
	~Action();

	void Initialize();
	void Update(Camera* camera);

private:
	void Rotation();
	void Movement();

private:
	Input* input = nullptr;

	Vector3 moveValue = {};

	MeshCollider* meshCollider = nullptr;;
};

