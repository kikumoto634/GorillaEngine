﻿#pragma once

class BaseObjObject;
class BaseCollider;

/// <summary>
///	クエリーいよる情報を得る為の構造体
/// </summary>
struct QueryHit
{
	//衝突相手のオブジェクト
	BaseObjObject* object = nullptr;
	//衝突相手のコライダー
	BaseCollider* collider = nullptr;
	//衝突点
	Vector4 inter;
	//排斥ベクトル
	Vector4 reject;
};


/// <summary>
/// クエリ―で交差を検出した時の動作を想定するクラス
/// </summary>
class QueryCallback
{
public:
	QueryCallback() = default;
	virtual ~QueryCallback() = default;

	/// <summary>
	/// 交差時コールバック
	/// </summary>
	/// <param name="info">交差情報</param>
	/// <returns>クエリ―を続けるならtrue, 打ち切るならfalseを返す</returns>
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};