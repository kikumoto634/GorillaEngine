#include "BaseParticleObjects.h"

void BaseParticleObjects::Initialize(UINT texNumber)
{
	object = ParticleObject::Create(texNumber);
	world.Initialize();
	world.UpdateMatrix();
}

void BaseParticleObjects::Update(Camera *camera)
{
	this->camera = camera;
	world.UpdateMatrix();
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
