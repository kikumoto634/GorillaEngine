#pragma once
#include "MapNode.h"
#include <vector>
#include "BaseEnemy.h"

//前方宣言
class SkeltonStateManager;

class Skelton : public BaseEnemy
{
public:
	//フレンド関数
	friend class IdelSkeltonState;
	friend class TrackSkeltonState;
	friend class DeadSkeltonState;
	friend class PopSkeltonState;


public:

	void AddInitialize() override;
	void AddUpdate() override;
	void AddDraw() override;
	void AddParticleDraw() override;
	void AddFinalize() override;

	void AddContactUpdate() override;

	//Getter
	inline Vector3 GetPopPosition() override {return popPosition_;}

private:

	//経路探索(A*アルゴリズム)
	vector<MapNode*> PathSearch(
		vector<vector<int>>& grid, 
		int start_x, int start_y, 
		int end_x, int end_y);
	//ヒューリスティック推定値計算
	int Heuristic(int x1,int y1, int x2,int y2){
		return abs(x1-x2) + abs(y1-y2);
	}


private:
	//状態
	SkeltonStateManager* state_ = nullptr;
	//死亡位置
	Vector3 popPosition_ = {};
};