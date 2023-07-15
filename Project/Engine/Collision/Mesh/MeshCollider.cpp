#include "MeshCollider.h"
#include "Collision.h"

using namespace DirectX;

void MeshCollider::ConstructTriangles(ObjModelManager *model)
{
	//三角形リストをクリア
	triangle.clear();
	//モデルの持つメッシュリストを取得
	const std::vector<ObjModelMesh*>& meshes = model->GetMeshes();
	//現在のメッシュの開始三角形番号を入れておく変数(0で初期化)
	int start = 0;
	//全メッシュについて順に処理する
	std::vector<ObjModelMesh*>::const_iterator it = meshes.cbegin();
	for(; it != meshes.cend(); ++it){
		//インデックスは、三角形の数*3個あるので、そこからメッシュ内の三角形の数を逆算する
		size_t triangleNum = (*it)->GetIndices().size()/3;
		triangle.resize(triangle.size() + triangleNum);
		//全三角形について順に処理
		for(int i = 0; i < triangleNum; i++){
			//今から計算する三角形の参照
			Triangle& tri = triangle[start+i];
			int idx0 = (*it)->GetIndices()[i*3+0];
			int idx1 = (*it)->GetIndices()[i*3+1];
			int idx2 = (*it)->GetIndices()[i*3+2];
			//三角形の3頂点の座標を代入
			tri.p0 = {(*it)->GetVertices()[idx0].pos.x, (*it)->GetVertices()[idx0].pos.y, (*it)->GetVertices()[idx0].pos.z, 1};
			tri.p1 = {(*it)->GetVertices()[idx1].pos.x, (*it)->GetVertices()[idx1].pos.y, (*it)->GetVertices()[idx1].pos.z, 1};
			tri.p2 = {(*it)->GetVertices()[idx2].pos.x, (*it)->GetVertices()[idx2].pos.y, (*it)->GetVertices()[idx2].pos.z, 1};
			//3頂点から法線を計算
			tri.ComputeNormal();
		}

		//次のメッシュは今まで三角形番号の次から使う
		start += (int)triangleNum;
	}
}

void MeshCollider::Update()
{
	//ワールド行列の逆行列を計算
	invMatWorld = invMatWorld.Inverse(GetObjObject()->GetmatWorld().matWorld);
}

bool MeshCollider::CheckCollisionSphere(const Sphere &sphere, Vector4 *inter, Vector4* reject)
{
	//オブジェクトのローカル座標系での球を得る(半径はXスケールを参照)
	Sphere localSphere;
	localSphere.center = localSphere.center.TransformVector3(sphere.center, invMatWorld);

	Vector4 tempV = {invMatWorld.m[0][0], invMatWorld.m[0][1], invMatWorld.m[0][2], invMatWorld.m[0][3]};
	localSphere.radius *= tempV.NormalizeVector3().x;

	//ローカル座標系での交差をチェック
	std::vector<Triangle>::const_iterator it = triangle.cbegin();

	for(; it != triangle.cend(); ++it){
		const Triangle& triangle = *it;

		//球と三角形との当たり判定
		if(Collision::CheckSphere2Triangle(localSphere, triangle, inter, reject)){
			if(inter){
				const Matrix4x4& matWorld = GetObjObject()->GetmatWorld().matWorld;
				//ワールド座標系での交点を得る
				*inter = inter->TransformVector3(*inter, matWorld);
			}
			if(reject){
				const Matrix4x4& matWorld = GetObjObject()->GetmatWorld().matWorld;
				//ワールド座標系での排斥ベクトルに変換
				reject->TransformNormalizeVector3(*reject, matWorld);
			}
			return true;
		}
	}

	return false;
}

bool MeshCollider::CheckCollisionRay(const Ray &ray, float *distance, Vector4 *inter)
{
	//オブジェクトのローカル座標系でのレイを得る
	Ray localRay;
	localRay.start = localRay.start.TransformVector3(ray.start, invMatWorld);
	localRay.dir.TransformNormalizeVector3(ray.dir, invMatWorld);

	//ローカル座標系で交差をチェック
	std::vector<Triangle>::const_iterator it = triangle.cbegin();

	for(; it != triangle.cend(); ++it){
		const Triangle& triangle = *it;

		Vector4 tempInter;
		//レイと三角形の当たり判定
		if(Collision::CheckRay2Triangle(localRay, triangle, nullptr, &tempInter)){
			const Matrix4x4& matWorld = GetObjObject()->GetmatWorld().matWorld;

			//ワールド座標系での交点を得る
			tempInter = tempInter.TransformVector3(tempInter, matWorld);

			if(distance){
				//交点とレイ始点の距離を計算
				Vector4 sub = tempInter - ray.start;
				*distance = (sub*ray.dir).x;
			}
			if(inter){
				*inter = tempInter;
			}
			return true;
		}
	}

	return false;
}
