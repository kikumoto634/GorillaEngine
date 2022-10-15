#pragma once
#include "Sprite.h"
#include "GeometryObject.h"
#include "GeometryObjectManager.h"
#include "FbxModelObject.h"
#include "WorldTransform.h"

//スプライト削除
void SpriteDelete(Sprite* sp){
	delete sp;
	sp = nullptr;
}

//幾何学オブジェクト削除
void GeometryDelete(GeometryObject* obj, WorldTransform& worldtransform){
	delete obj;
	obj = nullptr;
	worldtransform = {};
}

//FBXオブジェクト削除
void FBXDelete(FbxModelObject* obj, FbxModelManager* model, WorldTransform& worldtransform){
	delete obj;
	delete model;
	obj = nullptr;
	model = nullptr;
	worldtransform = {};
}

template<class T> void SafeDelete(T* p){
	delete p;
	p = nullptr;
}