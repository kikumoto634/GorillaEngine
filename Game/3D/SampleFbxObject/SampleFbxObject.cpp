#include "SampleFbxObject.h"

SampleFbxObject::~SampleFbxObject()
{
	Finalize();
}

void SampleFbxObject::Initialize(std::string filePath)
{
	BaseObjects::Initialize(filePath);
}

void SampleFbxObject::Update(Camera *camera)
{
	this->camera = camera;

	BaseObjects::Update(camera);
}

void SampleFbxObject::Draw()
{
	BaseObjects::Draw();
}

void SampleFbxObject::Finalize()
{
	BaseObjects::Finalize();
}
