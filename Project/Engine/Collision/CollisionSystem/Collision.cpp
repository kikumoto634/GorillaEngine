#include "Collision.h"

using namespace DirectX;

bool Collision::CheckSphere2Sphere(const Sphere &sphere1, const Sphere &sphere2, DirectX::XMVECTOR *inter, DirectX::XMVECTOR* reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = XMVector3LengthSq(sphere1.center - sphere2.center).m128_f32[0];

	float radius = sphere1.radius + sphere2.radius;
	radius *= radius;


	if(dist > radius)	{return false;}

	if(inter){
		//Aの半径が0の時、座標はBの中心、Bの半径が0の時座標はAの中心となる
		float t = sphere2.radius / (sphere1.radius + sphere2.radius);
		*inter = XMVectorLerp(sphere1.center, sphere2.center, t);
	}
	//押し出すベクトルを計算
	if(reject){
		float rejectLen = sphere1.radius + sphere2.radius - sqrtf(dist);
		*reject = XMVector3Normalize(sphere1.center - sphere2.center);
		*reject *= rejectLen;
	}

	return true;
}

bool Collision::CheckSphere2Plane(const Sphere &sphere, const Plane &plane, DirectX::XMVECTOR *inter)
{
	//座標系の原点から球の中心座標への距離
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	//平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV.m128_f32[0] - plane.distance;
	//距離の絶対値が半径より大きければ当たっていない
	if(fabsf(dist) > sphere.radius)
	{
		return false;
	}
	//議事交点を計算
	if(inter){
		//平面上の最近接点を、疑似交点とする
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR &point, const Triangle &triangle, DirectX::XMVECTOR *closest)
{
	//pointがp0の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if(d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f)
	{
		//p0は最近
		*closest = triangle.p0;
		return ;
	}

	//pointがp1の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p1_pt = point - triangle.p1;

	XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if(d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0])
	{
		//p1が最近
		*closest = triangle.p1;
		return ;
	}

	//pointがp0_p1の編領域のな簡易あるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];
	
	if(vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f)
	{
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.p0 + v * p0_p1;
		return ;
	}

	//pointがp2の外側の頂点領域の中にあるかどうかチェック
	XMVECTOR p2_pt = point - triangle.p2;

	XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);
	
	if(d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0])
	{
		*closest = triangle.p2;
		return ;
	}

	//pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];
	
	if(vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f)
	{
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.p0 + w * p0_p2;
		return ;
	}

	//pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2に対する射影を返す
	float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];

	if(va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f)
	{
		float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return ;
	}


	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere &sphere, const Triangle &triagnle, DirectX::XMVECTOR *inter, DirectX::XMVECTOR* reject)
{
	XMVECTOR p;

	//球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triagnle, &p);
	//点pと球の中心の差分ベクトル
	XMVECTOR v = p - sphere.center;
	//距離の二乗を求める
	//(同じベクトル同士の内積は三平方の定理のルート内部の式と一致する)
	v = XMVector3Dot(v, v);
	//球と三角形の距離は半径イカなら当たっていない
	if(v.m128_f32[0] > sphere.radius * sphere.radius){
		return false;
	}
	//疑似交点を計算
	if(inter){
		//三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	//押し出すベクトルを計算
	if(reject){
		float ds = XMVector3Dot(sphere.center, triagnle.normal).m128_f32[0];
		float dt = XMVector3Dot(triagnle.p0, triagnle.normal).m128_f32[0];
		float rejectLen = dt - ds + sphere.radius;
		*reject = triagnle.normal * rejectLen;
	}

	return true;
}


bool Collision::CheckRay2Plane(const Ray &ray, const Plane &plane, float *distance, DirectX::XMVECTOR *inter)
{
	const float epsilon = 1.0e-5f;	//誤差吸収用の微小な値
	//面法線とレイの方向ベクトルの内積
	float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];
	//裏面には当たらない
	if(d1 > -epsilon) {return false;}
	//始点と原点の距離(平面の法線方向)
	//面法線とレイの始点座標(位置ベクトル)の内積
	float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];
	//始点と平面の距離(平面の法線方向)
	float dist = d2 - plane.distance;
	//始点と平面の距離(レイ方向)
	float t = dist / -d1;
	//交差が始点より後ろにあるので、当たらない
	if(t < 0)	return false;
	//距離を書き込む
	if(distance){*distance = t;}
	//交差を計算
	if(inter)	{*inter = ray.start + t * ray.dir;}
	return true;
}

bool Collision::CheckRay2Triangle(const Ray &ray, const Triangle &triangle, float *distance, DirectX::XMVECTOR *inter)
{
	//三角形が乗っている平面を算出
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];
	//レイと平面が当たっていなければ、当たっていない
	if(!CheckRay2Plane(ray, plane, distance, &interPlane)){return false;}
	//レイと平面が当たっていたので、距離と交点が書き込まれた
	//レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;
	XMVECTOR m;
	//辺p0_p1について
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);
	//辺の外側であれば当たっていないので判定を打ち切る
	if(XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon)	{return false;}
	
	//辺p1_p2について
	XMVECTOR pt_p1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;
	m = XMVector3Cross(pt_p1, p1_p2);
	//辺の外側であれば当たっていないので判定を打ち切る
	if(XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {return false;}

	//辺p2_p0について
	XMVECTOR pt_p2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;
	m = XMVector3Cross(pt_p2, p2_p0);
	//辺の外側であれば当たっていないので判定を打ち切る
	if(XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) {return false;}

	//内側なので、当たっている
	if(inter){
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray &ray, const Sphere &sphere, float *distance, DirectX::XMVECTOR *inter)
{
	XMVECTOR m = ray.start - sphere.center;
	float b = XMVector3Dot(m, ray.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;
	//rayの始点がsphereのその側にあり(c > 0)、rayがsphereから離れていく方向を指している場合(b > 0)、当たらない
	if(c > 0.0f && b > 0.0f) {return false;}

	float discr = b * b - c;
	//負の判別式はレイが球を外れていることに一致
	if(discr < 0.0f) {return false;}

	//レイは球と交差している
	//交差する最小の値を計算
	float t = -b - sqrtf(discr);
	//tが負である場合、レイは球の内側から開始しているのでtをゼロにクランプ
	if(t < 0.0f) t = 0.0f;
	if(distance) {*distance = t;}

	if(inter) {*inter = ray.start + t * ray.dir;}

	return true;
}
