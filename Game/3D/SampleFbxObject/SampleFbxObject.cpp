#include "SampleFbxObject.h"

SampleFbxObject::~SampleFbxObject()
{
	Finalize();
}

void SampleFbxObject::Initialize(std::string filePath)
{
	BaseFbxObjects::Initialize(filePath);
}

void SampleFbxObject::Update(Camera *camera)
{
	this->camera = camera;

	BaseFbxObjects::Update(camera);
}

void SampleFbxObject::Draw()
{
	BaseFbxObjects::Draw();
}

void SampleFbxObject::Finalize()
{
	BaseFbxObjects::Finalize();
}
