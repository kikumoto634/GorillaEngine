#include "BaseSprites.h"
#include "Easing.h"

void BaseSprites::Initialize(UINT textureNumber)
{
	sprite = Sprite::Create(textureNumber, position);

	window = Window::GetInstance();
}

void BaseSprites::Update()
{
	sprite->SetPosition(position);
	sprite->SetSize(size);
	sprite->SetColor(color);
}

void BaseSprites::Draw()
{
	sprite->Draw();
}

void BaseSprites::Finalize()
{
	delete sprite;
	sprite = nullptr;
}

bool BaseSprites::ScaleChange(Vector2 sizeMax, Vector2 sizeMin, float EndTime)
{
	float ease = -(cosf(3.14159265f * scaleCurrentTime) - 1.f)/2.f;
	scale = Easing_Point2_Linear(sizeMin, sizeMax, ease);
	SetSize(scale);

	if(scaleCurrentTime >= 1.0f){
		scale = sizeMax;
		scaleCurrentTime = 0.f;
		return true;
	}

	scaleCurrentTime += 1.f/(60*EndTime);
	return false;
}

Vector2 BaseSprites::ChangeTransformation(Vector3 target, Camera* camera)
{
	targetPos = target;

	DirectX::XMMATRIX matViewport = 
	{
		(float)window->GetWindowWidth()/2, 0								  , 0, 0,
		0								 , -((float)window->GetWindowHeight())/2, 0, 0,
		0								 , 0								  , 1, 0, 
		(float)window->GetWindowWidth()/2, (float)window->GetWindowHeight()/2 , 0, 1,
	};
	DirectX::XMMATRIX matViewProjectionViewPort = camera->GetMatView() * camera->GetMatProjection() * matViewport;
	Vector3 positionreticle = Vector3Transform(target, matViewProjectionViewPort);
	return Vector2{positionreticle.x, positionreticle.y};
}

Vector3 BaseSprites::Vector3Transform(Vector3 &v, DirectX::XMMATRIX &m)
{
	float w = v.x * m.r[0].m128_f32[3] + v.y * m.r[1].m128_f32[3] + v.z * m.r[2].m128_f32[3] + m.r[3].m128_f32[3];

	Vector3 result
	{
		(v.x*m.r[0].m128_f32[0] + v.y*m.r[1].m128_f32[0] + v.z*m.r[2].m128_f32[0] + m.r[3].m128_f32[0])/w,
		(v.x*m.r[0].m128_f32[1] + v.y*m.r[1].m128_f32[1] + v.z*m.r[2].m128_f32[1] + m.r[3].m128_f32[1])/w,
		(v.x*m.r[0].m128_f32[2] + v.y*m.r[1].m128_f32[2] + v.z*m.r[2].m128_f32[2] + m.r[3].m128_f32[2])/w
	};

	return result;
}
