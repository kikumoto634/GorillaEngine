#include "Player.h"
#include "../../../Engine/input/Input.h"
#include "../../Collision/SphereCollider.h"
#include "../../../Engine/base/ParticleManager.h"

#include "../../Collision/CollisionSystem/CollisionManager.h"
#include "../../Collision/CollisionSystem/CollisionAttribute.h"

using namespace DirectX;

class PlayerQueryCallback : public QueryCallback
{
public:
	PlayerQueryCallback(Sphere* sphere): sphere(sphere){};

	//衝突時コールバック関数
	bool OnQueryHit(const QueryHit& info){
		//ワールドの上方向
		const XMVECTOR up = {0,1,0,0};
		//排斥方向
		XMVECTOR rejectDir = XMVector3Normalize(info.reject);
		//上方向と排斥方向の角度差のコサイン値
		float cos = XMVector3Dot(rejectDir, up).m128_f32[0];

		//地面判定閾値角度
		const float threshold = cosf(XMConvertToRadians(30.0f));	//30度以上勾配だと壁として認識する
		//角度差によって天井または地面と判定される場合を除いて
		if(-threshold < cos && cos < threshold){
			//球を排斥(押し出す)
			sphere->center += info.reject;
			move += info.reject;
		}
		return true;
	}

	//クエリ―に使用する球
	Sphere* sphere = nullptr;
	//排斥による移動量(累積地)
	DirectX::XMVECTOR move = {};
};

Player::~Player()
{
	Finalize();
}

void Player::Initialize(std::string filePath, bool IsSmoothing)
{
	BaseObjObject::Initialize(filePath, IsSmoothing);

	//コライダーの追加
	float radius = 0.6f;
	//半径文だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(XMVECTOR{0,radius,0,0}, radius));

	//当たり判定属性
	collider->SetAttribute(COLLISION_ATTR_ALLIES);
}

void Player::Update(Camera *camera)
{
	this->camera = camera;

	//入力
	Input* input = Input::GetInstance();
	//A,Dで旋回
	if(input->Push(DIK_LEFT)){
		world.rotation.y -= 0.1f;
	}
	else if(input->Push(DIK_RIGHT)){
		world.rotation.y += 0.1f;
	}

	// 移動ベクトルをY軸周りの角度で回転
    XMVECTOR move = { 0,0,0.1f,0};
    XMMATRIX matRot = XMMatrixRotationY(world.rotation.y);
    move = XMVector3TransformNormal(move, matRot);

	//向いている方向に移動
	if(input->Push(DIK_DOWN)){
		world.translation.x -= move.m128_f32[0];
		world.translation.y -= move.m128_f32[1];
		world.translation.z -= move.m128_f32[2];
	}
	else if(input->Push(DIK_UP)){
		world.translation.x += move.m128_f32[0];
		world.translation.y += move.m128_f32[1];
		world.translation.z += move.m128_f32[2];
	}

	//落下処理
	if(!IsGround){
		//下向き加速度
		const float fallAcc = -0.01f;
		const float fallVYMin = -0.5f;
		//加速
		fallV.y = max(fallV.y + fallAcc, fallVYMin);
		//移動
		world.translation.x += fallV.x;
		world.translation.y += fallV.y;
		world.translation.z += fallV.z;
	}
	//ジャンプ操作
	else if(input->Trigger(DIK_SPACE)){
		IsGround = false;
		const float jumpVYFist = 0.2f;	//ジャンプ時上向き初速
		fallV = {0,jumpVYFist, 0};
	}

	//球コライダー取得
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);


	//クエリ―コサインの関数オブジェクト
	PlayerQueryCallback callback(sphereCollider);
	//球と地形の交差を全検査
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);
	//交差による排斥分動かす
	world.translation.x += callback.move.m128_f32[0];
	world.translation.y += callback.move.m128_f32[1];
	world.translation.z += callback.move.m128_f32[2];


	//行列、カメラ更新
	BaseObjObject::Update(this->camera);
	//コライダー更新
	collider->Update();


	//球の上端から球の下端までの例キャスト用レイを準備
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.m128_f32[1] += sphereCollider->GetRadius();
	ray.dir = {0,-1,0,0};
	RaycastHit raycastHit;

	//接地状態
	if(IsGround){
		//スムーズに坂を下る為の吸着距離
		const float adsDistance = 0.2f;
		//接地を維持
		if(CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)){
			IsGround = true;
			world.translation.y -= (raycastHit.distance - sphereCollider->GetRadius()*2.0f);
			//行列の更新など
			BaseObjObject::Update(this->camera);
		}
		//地面がないので落下
		else{
			IsGround = false;
			fallV = {};
		}
	}
	//落下状態
	else if(fallV.y <= 0.0f){
		if(CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius()*2.0f)){
			//着地
			IsGround = true;
			world.translation.y -= (raycastHit.distance - sphereCollider->GetRadius()*2.0f);
			//行列の更新など
			BaseObjObject::Update(this->camera);
		}
	}
}

void Player::Draw3D()
{
	BaseObjObject::Draw();
}


void Player::Finalize()
{
	BaseObjObject::Finalize();
}

void Player::OnCollision(const CollisionInfo &info)
{
	//衝突点にパーティクル発生
	for(int i = 0; i < 1; i++){
		Vector3 pos{};
		pos.x = info.inter.m128_f32[0];
		pos.y = info.inter.m128_f32[1];
		pos.z = info.inter.m128_f32[2];
		
		const float md_vel = 0.1f;
		Vector3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel/2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel/2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel/2.0f;

		Vector3 acc{};
		acc.x = 0.f;
		acc.y = 0.f;
		acc.z = 0.f;

		ParticleManager::GetInstance()->Add(10, pos, vel, acc, 0.0f, 1.0f, 2);
	}
}

