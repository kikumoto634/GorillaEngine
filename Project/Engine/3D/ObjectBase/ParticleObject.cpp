#include "ParticleObject.h"
#include "DirectXCommon.h"

ParticleObject::ParticleObject(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale, UINT texNumber, Vector4 color):
	life(life),
	position(position),
	velocity(velocity),
	accel(accel),
	start_scale(start_scale),
	end_scale(end_scale),
	texNumber(texNumber)
{
	particle->SetColor(color);
}

ParticleObject::ParticleObject(Particle obj)
{
	life = obj.life;
	position = obj.position;
	velocity = obj.velocity;
	accel = obj.accel;
	start_scale = obj.start_scale;
	end_scale = obj.end_scale;
	texNumber = obj.texNumber;
	particle->SetColor(obj.color);
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

void ParticleObject::ParticleSet(Particle obj)
{
	life = obj.life;
	position = obj.position;
	velocity = obj.velocity;
	accel = obj.accel;
	start_scale = obj.start_scale;
	end_scale = obj.end_scale;
	texNumber = obj.texNumber;
	particle->SetColor(obj.color);
}
