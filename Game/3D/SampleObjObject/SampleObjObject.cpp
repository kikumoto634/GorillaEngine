#include "SampleObjObject.h"

SampleObjObject::~SampleObjObject()
{
	Finalize();
}

void SampleObjObject::Initialize(std::string filePath, bool IsSmoothing)
{
	BaseObjObject::Initialize(filePath, IsSmoothing);

	object->SetLightMask(1.0f);
}

void SampleObjObject::Update(Camera *camera)
{
	this->camera = camera;
	BaseObjObject::Update(camera);
}

void SampleObjObject::Draw()
{
	BaseObjObject::Draw();
}

void SampleObjObject::Finalize()
{
	BaseObjObject::Finalize();
}

