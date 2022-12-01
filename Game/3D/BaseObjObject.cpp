#include "BaseObjObject.h"

void BaseObjObject::Initialize(std::string filePath, bool IsSmmothing)
{
	model = new ObjModelManager();
	model->CreateModel(filePath, IsSmmothing);
	object = ObjModelObject::Create(model);
	world.Initialize();
}

void BaseObjObject::Update(Camera *camera)
{
	this->camera = camera;
	world.UpdateMatrix();
	object->Update(world, this->camera);
}

void BaseObjObject::Draw()
{
	object->Draw();
}

void BaseObjObject::Finalize()
{
	delete model;
	model = nullptr;
	delete object;
	object = nullptr;
	world = {};
}
