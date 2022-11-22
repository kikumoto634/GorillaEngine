#include "BaseGeometryObjects.h"

void BaseGeometryObjects::Initialize(UINT texNumber)
{
	object = GeometryObject::Create(texNumber);
	world.Initialize();
	world.UpdateMatrix();
}

void BaseGeometryObjects::Update(Camera *camera)
{
	this->camera = camera;
	world.UpdateMatrix();
	object->Update(world, this->camera);

}

void BaseGeometryObjects::Draw()
{
	object->Draw();
}

void BaseGeometryObjects::Finalize()
{
	delete object;
	object = nullptr;
	world = {};
}
