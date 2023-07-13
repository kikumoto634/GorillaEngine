#include "ParticleObject.h"
#include "DirectXCommon.h"

ParticleObject::ParticleObject(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale, UINT texNumber, Vector4 color)
{
	this->life = life;
	this->position = position;
	this->velocity = velocity;
	this->accel = accel;
	this->start_scale = start_scale;
	this->end_scale = end_scale;
	this->texNumber = texNumber;
	particle->SetColor(color);
}

ParticleObject::~ParticleObject()
{
	Finalize();
}

void ParticleObject::Initialize()
{
	particle = new ParticleManager();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	particle->Initialize(dxCommon);
	world.Initialize();
}

void ParticleObject::Update(Camera *camera)
{
	world.UpdateMatrix();
	particle->Update(world, camera);
}

void ParticleObject::Draw()
{
	particle->Draw();
}

void ParticleObject::Finalize()
{
	delete particle;
	particle = nullptr;
	world = {};
}

void ParticleObject::ParticleAppearance()
{
	particle->Add(life, position,velocity,accel,start_scale,end_scale,texNumber);
}

void ParticleObject::ParticleSet(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale, UINT texNumber, Vector4 color)
{
	this->life = life;
	this->position = position;
	this->velocity = velocity;
	this->accel = accel;
	this->start_scale = start_scale;
	this->end_scale = end_scale;
	this->texNumber = texNumber;
	particle->SetColor(color);
}
