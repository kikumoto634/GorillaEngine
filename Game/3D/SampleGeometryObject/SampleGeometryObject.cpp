#include "SampleGeometryObject.h"

SampleGeometryObject::~SampleGeometryObject()
{
	Finalize();
}

void SampleGeometryObject::Initialize(UINT texNumber)
{
	BaseGeometryObjects::Initialize(texNumber);
}

void SampleGeometryObject::Update(Camera *camera)
{
	this->camera = camera;
	BaseGeometryObjects::Update(this->camera);
}

void SampleGeometryObject::Draw()
{
	BaseGeometryObjects::Draw();
}

void SampleGeometryObject::Finalize()
{
	BaseGeometryObjects::Finalize();
}
