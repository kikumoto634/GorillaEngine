#pragma once
#include "BaseBattleScene.h"

#include "Boss1.h"
#include "Skelton.h"
#include "TutorialSp.h"

#include <vector>

class Boss1Area : public BaseBattleScene
{
public:
	~Boss1Area();

	Boss1Area(DirectXCommon* dxCommon, Window* window, int saveHP = 5);

private:
	template <class T> using vector = std::vector<T>;

public:
	enum class EventState{
		Approach,
		Roar,
		Return
	};

private:
	//CutInUI
	const int CutInSpNum = 4;
	const Vector2 CutInSpAnc = {0.5f,0.5f};

	//真ん中
	const int CutInSpCenterNumber = 0;
	const Vector2 CutInSpCenterBeginPos = {1920,360};
	const Vector2 CutInSpCenterEndPos   = {640,360};
	const Vector2 CutInSpCenterSize     = {1280,360};
	//上
	const int CutInSpUpNumber = 1;
	const Vector2 CutInSpUpBeginPos = {-400,120};
	const Vector2 CutInSpUpEndPos   = {380,120};
	const Vector2 CutInSpUpDownSize = {800,180};
	//下
	const int CutInSpDownNumber = 2;
	const Vector2 CutInSpDownBeginPos = {1680,600};
	const Vector2 CutInSpDownEndPos   = {880,600};
	//名前
	const int CutInSpNameNumber = 3;
	const Vector2 CutInSpNameBeginPos = {1605,610};
	const Vector2 CutInSpNameEndPos = {950,610};
	const Vector2 CutInSpNameSize = {650,150};

	//移動時間
	const float CutInMoveSecondMax = 0.25f;


	//壁
	const int BlocksNum = 3;
	const float BlockSize = AreaManager::Block_Size;

	//入口
	const Vector3 EnterWallBasePos = {-2,-3,-2};
	const float EnterCloseBorderPosZ = 0.f;

	//出口
	const Vector3 ExitWallBasePos = {-2,-3,24};

	//リズム変更
	const float ChangeTime = 1.5f;

private:
//シーン遷移
	void NextSceneChange() override;
	void SceneChange() override;

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
	//カットイン
	void CutInInitialize();
	void cutInInput();
	void CutInUpdate();
	void CutInDraw();
	void CutInFinalize();

	//入口
	void EnterInitialize();
	void EnterUpdate();
	void EnterDraw();
	void EnterFinalize();

	//出口
	void ExitInitialize();
	void ExitUpdate();
	void ExitDraw();
	void ExitFinalize();

	void ExitOpen();


	//ボス
	void BossInitialize();
	void BossUpdate();
	void BossBeatEnd();
	void BossDraw();
	void BossParticleDraw();
	void BossFinalize();

	//スケルトン
	void SkeltonCreate();
	void SkeltonInitialize();
	void SkeltonUpdate();
	void SkeltonBeatEnd();
	void SkeltonDraw();
	void SkeltonParticleDraw();
	void SkeltonFinalize();

	//イベント処理
	void EventStart();
	void EventUpdate();


private:
	//CutInUIフラグ
	//生存
	bool isCutInAlive_ = true;
	//登場
	bool isBossAppUIFlag_ = true;
	//移動
	bool isCutInUIMove_ = false;
	//STL
	vector<unique_ptr<BaseSprites>> cutInSp_;
	vector<Vector2> cutInSpPos_;
	//時間
	float cutInMoveFrame_ = 0.f;

	//壁
	//モデル
	ObjModelManager* indestructibleWallModel_ = nullptr;
	ObjModelManager* indestructibleWallColliderModel_ = nullptr;
	//入口
	bool isEnterBlocksAlive_ = false;
	list<unique_ptr<IndestructibleWall>> enterWall_;
	//出口
	bool isExitBlocksAlive_ = true;
	list<unique_ptr<IndestructibleWall>> exitWall_;

	//ボス
	bool isBossAlive_ = false;
	unique_ptr<Boss1> boss_;

	//スケルトン
	list<unique_ptr<BaseEnemy>> enemys_;

	//イベント開始
	EventState event_ = EventState::Approach;
	bool isEventActive_ = false;

	const Vector3 TargetEnd = {0.f,-3.f,10.f};
	const Vector3 EyeEnd = {0.f,3.f,-8.f};

	Vector3 targetSaveValue;
	Vector3 eyeSaveValue;

	Vector3 targetValue;
	Vector3 eyeValue;

	float eventSecond = 0;
	const float EventApproachSecond = 3.f;
	const float EventRoarSecond = 1.5f;

	//咆哮
	bool isRoar_ = true;

	//BGM再生
	bool isEventBGM_ = false;
	float changeTime = 0.f;

	//コイン
	int coinPopNumMax_ = 10;
	queue<Vector3> coinDropPos_ = {};
	//オブジェクト
	list<unique_ptr<BaseObjObject>> obj_;
};
