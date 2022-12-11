#include "SampleParticleObject.h"

SampleParticleObject::~SampleParticleObject()
{
	Finalize();
}

void SampleParticleObject::Initialize(UINT texNumber)
{
	//for(int i =0; i < 100; i++){
	//	//X,Y,Zすべて[-5.f,+5.f]でランダムに分布
	//	const float md_pos = 10.f;
	//	Vector3 pos{};
	//	pos.x = (float)rand()/RAND_MAX*md_pos-md_pos/2.0f;
	//	pos.y = (float)rand()/RAND_MAX*md_pos-md_pos/2.0f;
	//	pos.z = (float)rand()/RAND_MAX*md_pos-md_pos/2.0f;
	//	//X,Y,Zすべて[-0.05f,+0.05f]でランダムに分布
	//	const float md_vel = 0.1f;
	//	Vector3 vel{};
	//	vel.x = (float)rand()/RAND_MAX*md_vel-md_vel/2.0f;
	//	vel.y = (float)rand()/RAND_MAX*md_vel-md_vel/2.0f;
	//	vel.z = (float)rand()/RAND_MAX*md_vel-md_vel/2.0f;
	//	//重力を見立ててYのみ[-0.001f, 0]でランダムに分布
	//	Vector3 acc{};
	//	const float md_acc = 0.001f;
	//	acc.y = (float)rand()/RAND_MAX*md_acc;

	//	//追加
	//	object->Add(60, pos, vel, acc, 1.f, 0.f);
	//}
	BaseParticleObjects::Initialize(texNumber);
}

void SampleParticleObject::Update(Camera *camera)
{
	this->camera = camera;
	BaseParticleObjects::Update(this->camera);
}

void SampleParticleObject::Draw()
{
	BaseParticleObjects::Draw();
}

void SampleParticleObject::Finalize()
{
	BaseParticleObjects::Finalize();
}

void SampleParticleObject::Add(int life, Vector3 position, Vector3 velocity, Vector3 accel, float start_scale, float end_scale)
{
	object->Add(life, position, velocity, accel, start_scale, end_scale);
}
