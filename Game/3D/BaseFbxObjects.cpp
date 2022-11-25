#include "BaseFbxObjects.h"

void BaseFbxObjects::Initialize(std::string filePath)
{
	model = FbxLoader::GetInstance()->LoadModeFromFile(filePath);
	object = FbxModelObject::Create(model);
	world.Initialize();
	world.UpdateMatrix();
}

void BaseFbxObjects::Update(Camera *camera, bool IsBillboard)
{
	this->camera = camera;
	if(!IsBillboard)		world.UpdateMatrix();
	else if(IsBillboard)	world.UpdateMatrix(this->camera->GetBillboard());
	object->Update(world, this->camera);
}

void BaseFbxObjects::Draw()
{
	//ƒvƒŒƒCƒ„[
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
