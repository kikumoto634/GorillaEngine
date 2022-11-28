#include "BaseFbxObjects.h"

void BaseFbxObjects::Initialize(std::string filePath)
{
	model = FbxLoader::GetInstance()->LoadModeFromFile(filePath);
	object = FbxModelObject::Create(model);
	world.Initialize();
	world.UpdateMatrix();
}

void BaseFbxObjects::Update(Camera *camera)
{
	this->camera = camera;
	world.UpdateMatrix();
	object->Update(world, this->camera);
}

void BaseFbxObjects::Draw()
{
	object->Draw();
}

void BaseFbxObjects::Finalize()
{
	delete model;
	model = nullptr;
	delete object;
	object = nullptr;
	world = {};
}
