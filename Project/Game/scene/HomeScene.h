#pragma once
#include "BaseBattleScene.h"

#include "BlueSlime.h"
#include "Skelton.h"
#include "Coins.h"
#include "Botton.h"

#include "TutorialSp.h"

class HomeScene : public BaseBattleScene
{
private:
	//テキスト情報
	const Vector3 TrainingTextPos = {-8,-3,-1};
	const Vector2 TrainingTextSize = {200, 40};
	const Vector2 TraningTextAnc = {0.5f,0.5f};

	const Vector3 ExitTextPos = {-0,-3,-7};
	const Vector2 ExitTextSize= {200, 40};
	const Vector2 TrainingTextAnc = {0.5f,0.5f};

	//必要コイン
	const int NeedCoinNum = 0;

	const int SlimePopNumMax = 1;

public:
	~HomeScene();

	HomeScene(DirectXCommon* dxCommon, Window* window, int saveHP = 5);

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
	//ダンジョンテキスト
	unique_ptr<TutorialSp> exitText_;

	unique_ptr<TutorialSp> trainingText_;

	list<unique_ptr<BaseEnemy>> slime_;

	unique_ptr<Botton> bottonHigh_;
	unique_ptr<Botton> bottonLow_;
};

