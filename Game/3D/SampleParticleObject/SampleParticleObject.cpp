#include "SampleParticleObject.h"

SampleParticleObject::~SampleParticleObject()
{
	Finalize();
}

void SampleParticleObject::Initialize(UINT texNumber)
{
	BaseParticleObjects::Initialize(texNumber);
}

void SampleParticleObject::Update(Camera *camera)
{
	this->camera = camera;
	BaseParticleObjects::Update(this->camera, false);
}

void SampleParticleObject::Draw()
{
	BaseParticleObjects::Draw();
}

void SampleParticleObject::Finalize()
{
	BaseParticleObjects::Finalize();
}
