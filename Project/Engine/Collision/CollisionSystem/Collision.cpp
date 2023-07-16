#include "Collision.h"
#include <cmath>

bool Collision::CheckSphere2Sphere(const Sphere &sphere1, const Sphere &sphere2, Vector4 *inter,Vector4* reject)
{
	// 中心点の距離の２乗 <= 半径の和の２乗　なら交差
	float dist = (sphere1.center - sphere2.center).x;
	dist *= dist;

	float radius = sphere1.radius + sphere2.radius;
	radius *= radius;


	if(dist > radius)	{return false;}

	if(inter){
		//Aの半径が0の時、座標はBの中心、Bの半径が0の時座標はAの中心となる
		float t = sphere2.radius / (sphere1.radius + sphere2.radius);
		Vector4 temp = temp.Lerp(sphere1.center, sphere2.center, t);
		*inter = temp;
	}
	//押し出すベクトルを計算
	if(reject){
		float rejectLen = sphere1.radius + sphere2.radius - sqrtf(dist);
		Vector4 temp = sphere1.center - sphere2.center;
		*reject = temp.Normalize();
		*reject *= rejectLen;
	}

	return true;
}

bool Collision::CheckSphere2Plane(const Sphere &sphere, const Plane &plane, Vector4 *inter)
{
	//座標系の原点から球の中心座標への距離
	Vector4 distV = sphere.center*plane.normal;
	//平面の原点距離を減算することで、平面と球の中心との距離が出る
	float dist = distV.x - plane.distance;
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

void Collision::ClosestPtPoint2Triangle(const Vector4 &point, const Triangle &triangle, Vector4 *closest)
{
	//pointがp0の外側の頂点領域の中にあるかどうかチェック
	Vector4 p0_p1 = triangle.p1 - triangle.p0;
	Vector4 p0_p2 = triangle.p2 - triangle.p0;
	Vector4 p0_pt = point - triangle.p0;

	Vector4 d1 = (p0_p1 * p0_pt);
	Vector4 d2 = (p0_p2 * p0_pt);

	if(d1.x <= 0.0f && d2.x <= 0.0f)
	{
		//p0は最近
		*closest = triangle.p0;
		return ;
	}

	//pointがp1の外側の頂点領域の中にあるかどうかチェック
	Vector4 p1_pt = point - triangle.p1;

	Vector4 d3 = (p0_p1 * p1_pt);
	Vector4 d4 = (p0_p2 * p1_pt);

	if(d3.x >= 0.0f && d4.x <= d3.x)
	{
		//p1が最近
		*closest = triangle.p1;
		return ;
	}

	//pointがp0_p1の編領域のな簡易あるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1.x * d4.x - d3.x * d2.x;
	
	if(vc <= 0.0f && d1.x >= 0.0f && d3.x <= 0.0f)
	{
		float v = d1.x / (d1.x - d3.x);
		*closest = triangle.p0 + v * p0_p1;
		return ;
	}

	//pointがp2の外側の頂点領域の中にあるかどうかチェック
	Vector4 p2_pt = point - triangle.p2;

	Vector4 d5 = (p0_p1* p2_pt);
	Vector4 d6 = (p0_p2* p2_pt);
	
	if(d6.x >= 0.0f && d5.x <= d6.x)
	{
		*closest = triangle.p2;
		return ;
	}

	//pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5.x * d2.x - d1.x * d6.x;
	
	if(vb <= 0.0f && d2.x >= 0.0f && d6.x <= 0.0f)
	{
		float w = d2.x / (d2.x - d6.x);
		*closest = triangle.p0 + w * p0_p2;
		return ;
	}

	//pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2に対する射影を返す
	float va = d3.x * d6.x - d5.x * d4.x;

	if(va <= 0.0f && (d4.x - d3.x) >= 0.0f && (d5.x - d6.x) >= 0.0f)
	{
		float w = (d4.x - d3.x) / ((d4.x - d3.x) + (d5.x - d6.x));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return ;
	}


	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere &sphere, const Triangle &triagnle, Vector4 *inter, Vector4* reject)
{
	Vector4 p;

	//球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(sphere.center, triagnle, &p);
	//点pと球の中心の差分ベクトル
	Vector4 v = p - sphere.center;
	//距離の二乗を求める
	//(同じベクトル同士の内積は三平方の定理のルート内部の式と一致する)
	v = (v* v);
	//球と三角形の距離は半径イカなら当たっていない
	if(v.x > sphere.radius * sphere.radius){
		return false;
	}
	//疑似交点を計算
	if(inter){
		//三角形上の最近接点pを疑似交点とする
		*inter = p;
	}
	//押し出すベクトルを計算
	if(reject){
		float ds = (sphere.center* triagnle.normal).x;
		float dt = (triagnle.p0* triagnle.normal).x;
		float rejectLen = dt - ds + sphere.radius;
		*reject = triagnle.normal * rejectLen;
	}

	return true;
}


bool Collision::CheckRay2Plane(const Ray &ray, const Plane &plane, float *distance, Vector4 *inter)
{
	const float epsilon = 1.0e-5f;	//誤差吸収用の微小な値
	//面法線とレイの方向ベクトルの内積
	float d1 = (plane.normal* ray.dir).x;
	//裏面には当たらない
	if(d1 > -epsilon) {return false;}
	//始点と原点の距離(平面の法線方向)
	//面法線とレイの始点座標(位置ベクトル)の内積
	float d2 = (plane.normal* ray.start).x;
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

bool Collision::CheckRay2Triangle(const Ray &ray, const Triangle &triangle, float *distance, Vector4 *inter)
{
	//三角形が乗っている平面を算出
	Plane plane;
	Vector4 interPlane;
	plane.normal = triangle.normal;
	plane.distance = (triangle.normal* triangle.p0).x;
	//レイと平面が当たっていなければ、当たっていない
	if(!CheckRay2Plane(ray, plane, distance, &interPlane)){return false;}
	//レイと平面が当たっていたので、距離と交点が書き込まれた
	//レイと平面の交点が三角形の内側にあるか判定
	const float epsilon = 1.0e-5f;
	Vector4 m;
	//辺p0_p1について
	Vector4 pt_p0 = triangle.p0 - interPlane;
	Vector4 p0_p1 = triangle.p1 - triangle.p0;
	m = pt_p0.CorssVector3(p0_p1);
	//辺の外側であれば当たっていないので判定を打ち切る
	if((m* triangle.normal).x < -epsilon)	{return false;}
	
	//辺p1_p2について
	Vector4 pt_p1 = triangle.p1 - interPlane;
	Vector4 p1_p2 = triangle.p2 - triangle.p1;
	m = pt_p1.CorssVector3(p1_p2);
	//辺の外側であれば当たっていないので判定を打ち切る
	if((m* triangle.normal).x < -epsilon) {return false;}

	//辺p2_p0について
	Vector4 pt_p2 = triangle.p2 - interPlane;
	Vector4 p2_p0 = triangle.p0 - triangle.p2;
	m = pt_p2.CorssVector3(p2_p0);
	//辺の外側であれば当たっていないので判定を打ち切る
	if((m* triangle.normal).x < -epsilon) {return false;}

	//内側なので、当たっている
	if(inter){
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray &ray, const Sphere &sphere, float *distance,Vector4 *inter)
{
	Vector4 m = ray.start - sphere.center;
	float b = (m* ray.dir).x;
	float c = (m* m).x - sphere.radius * sphere.radius;
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
