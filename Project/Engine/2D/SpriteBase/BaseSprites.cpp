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

	Matrix4x4 matViewport = 
	{
		(float)window->GetWindowWidth()/2, 0								  , 0, 0,
		0								 , -((float)window->GetWindowHeight())/2, 0, 0,
		0								 , 0								  , 1, 0, 
		(float)window->GetWindowWidth()/2, (float)window->GetWindowHeight()/2 , 0, 1,
	};
	Matrix4x4 matViewProjectionViewPort = camera->GetMatView() * camera->GetMatProjection() * matViewport;
	Vector3 positionreticle = Vector3Transform(target, matViewProjectionViewPort);
	return Vector2{positionreticle.x, positionreticle.y};
}

Vector3 BaseSprites::Vector3Transform(Vector3 &v, Matrix4x4 &m)
{
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	Vector3 result
	{
		(v.x*m.m[0][0] + v.y*m.m[1][0] + v.z*m.m[2][0] + m.m[3][0])/w,
		(v.x*m.m[0][1] + v.y*m.m[1][1] + v.z*m.m[2][1] + m.m[3][1])/w,
		(v.x*m.m[0][2] + v.y*m.m[1][2] + v.z*m.m[2][2] + m.m[3][2])/w
	};

	return result;
}
