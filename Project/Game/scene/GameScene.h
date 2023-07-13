#pragma once
#include "BaseBattleScene.h"

#include "BlueSlime.h"
#include "Skelton.h"
#include "Coins.h"

class GameScene : public BaseBattleScene
{
public:
	struct FloorInfo{
		int needCoin;

		//エネミー生成数
		int slimne;
		int skelton;
	};

//定数
private:
	//エリアごとの数(コイン/スライム/スケルトン)
	FloorInfo one	= {0,  0,0};
	FloorInfo two	= {5,  8, 7};
	FloorInfo three = {7,  5,10};
	FloorInfo four	= {10, 2,13};

	//一回で階層上がる値
	const int DepthIncreValue = 1;

	//ボスエリア||クリアに必要な階層
	const int DenpthMaxNum = 5;

	//階層テキストの情報
	const Vector2 TextPos = {1100,700};
	const Vector2 TextSize = {100,25};
	const Vector2 TextAnc = {0.5f,0.5f};

	const Vector2 ValuePos = {1170,700};
	const Vector2 ValueSize= {15,25};
	const Vector2 ValueAnc = {0.5f,0.5f};

public:
	~GameScene();

	GameScene(DirectXCommon* dxCommon, Window* window, int saveHP = 5, int floorValue = 1);

private:
//シーン遷移
	void NextSceneChange() override;

	//初期化
	void AddCommonInitialize() override;
	void AddObject3DInitialize() override;
	void AddObject2DInitialize() override;

	//更新
	void AddCommonUpdate() override;
	void AddObject3DUpdate() override;
	void AddObject2DUpdate() override;
	void AddBeatEndUpdate() override;

	//描画
	void AddObject3DDraw() override;
	void AddParticleDraw() override;
	void AddFrontUIDraw() override;
	void AddBackUIDraw() override;

	//後処理
	void AddObjectFinalize() override;
	void AddCommonFinalize() override;


private:
	void ActorCreateInitialize();

private:
	//エネミー
	int slimePopNumMax_ = 5;
	int skeletonPopNumMax_ = 5;
	list<unique_ptr<BaseEnemy>> enemys_;

	//コイン
	int coinPopNumMax_ = 10;
	queue<Vector3> coinDropPos_ = {};
	//オブジェクト
	list<unique_ptr<BaseObjObject>> obj_;


	//数字の画像番号
	int numberTextBase_ = number0_tex.number;
	
	//画像
	list<unique_ptr<BaseSprites>> sp_;

	//トラップ
	int trapNum = 10;
	vector<unique_ptr<Botton>> button;
};

