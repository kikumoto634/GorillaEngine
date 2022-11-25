#include "BaseGeometryObjects.h"

void BaseGeometryObjects::Initialize(UINT texNumber)
{
	object = GeometryObject::Create(texNumber);
	world.Initialize();
	world.UpdateMatrix();
}

void BaseGeometryObjects::Update(Camera *camera, bool IsBillboard)
{
	this->camera = camera;
	if(!IsBillboard)	    world.UpdateMatrix();
	else if(IsBillboard)	world.UpdateMatrix(this->camera->GetBillboard());
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
