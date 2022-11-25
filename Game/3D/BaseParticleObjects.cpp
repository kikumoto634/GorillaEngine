#include "BaseParticleObjects.h"

void BaseParticleObjects::Initialize(UINT texNumber)
{
	object = ParticleObject::Create(texNumber);
	world.Initialize();
	world.UpdateMatrix();
}

void BaseParticleObjects::Update(Camera *camera, bool IsBillboard)
{
	this->camera = camera;
	if(!IsBillboard)	    world.UpdateMatrix();
	else if(IsBillboard)	world.UpdateMatrix(this->camera->GetBillboard());
	object->Update(world, this->camera);
}

void BaseParticleObjects::Draw()
{
	object->Draw();
}

void BaseParticleObjects::Finalize()
{
	delete object;
	object = nullptr;
	world = {};
}
