#pragma once
#include "BaseScene.h"

#include "TouchableObject.h"

#include "Player.h"
#include "Exit.h"
#include "Botton.h"

#include "JudgeLocation.h"
#include "RNotes.h"
#include "LNotes.h"

#include "CollisionPrimitive.h"
#include "CollisionManager.h"

#include "RhythmManager.h"
#include "GameManager.h"
#include "AreaManager.h"

#include "AudioUtility.h"
#include "TextureUtility.h"

#include <list>
#include <queue>

class BaseBattleScene : public BaseScene
{
private:
	//カメラ初期回転値
	const Vector3 CameraRotValueIni = {DirectX::XMConvertToRadians(-60.f),0.f,0.f};
	
	//プレイヤー
	//初期回転値
	const Vector3 PlayerRotValueIni = {0.f,DirectX::XMConvertToRadians(180),0.f};
	//初期武器オフセット位置
	const Vector3 WeaponOffsetValueIni = {0.f,0.f,-2.f};

	//出口のスプライト出現位置
	const Vector3 ExitSpLocalPos = {-1,2,0};

	//リザルト
	const Vector2 ResultTextPos = {640, 360};
	const Vector2 PlayResultPos = {640, 180};
	const Vector2 ResultTextAnch = {0.5f,0.5f};
	const Vector2 ResultTextSize = {256,64};
	const Vector2 PlayResultSize = {448,96};
	const Vector2 ResultBackSize = {1280,720};
	const Vector3 ResultColor = {0.f,0.25f,1.0f};
	const Vector3 ResultBackColor = {0.f,0.f,0.0f};

protected:
	//シーン遷移
	const float GameStartFadeSecond = 1.f;
	const float GameEndFadeSecond = 0.5f;

	//ノーツ数
	const int NotesNum= 7;

protected:
	template <class T> using unique_ptr = std::unique_ptr<T>;
	template <class T> using list = std::list<T>;
	template <class T> using vector = std::vector<T>;
	template <class T> using queue = std::queue<T>;

public:
	
	//コンストラクタ
	BaseBattleScene(DirectXCommon* dxCommon, Window* window, int saveHP = 5, int floorVaule = 1);

	~BaseBattleScene();

	/// <summary>
	/// 起動時
	/// </summary>
	void Application() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 3D描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 背景描画
	/// </summary>
	void DrawBack() override;

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawNear() override;


	/// <summary>
	/// 後処理
	/// </summary>
	void Finalize() override;

private:
	//初期化
	void CommonInitialize();
	void Object3DInitialize();
	void Object2DInitialize();

	//更新
	void Object3DUpdate();
	void Object2DUpdate();
	void CommonUpdate();

	//リズム関連
	void RhythmJudgeUpdate();
	void BeatEndUpdate();

	//描画
	void Object3DDraw();
	void ParticleDraw();
	void UIDraw();

	//後処理
	void ObjectFinaize();
	void CommonFinalize();

	//リザルト
	void ResultUpdate();
	void ResultDraw2D();

protected:

	//シーン遷移
	virtual void SceneGameEnd();
	virtual void NextSceneChange() = 0;
	virtual void SceneChange();

	//初期化
	virtual void AddCommonInitialize() = 0;
	virtual void AddObject3DInitialize() = 0;
	virtual void AddObject2DInitialize() = 0;

	//更新
	virtual void AddCommonUpdate() = 0;
	virtual void AddObject3DUpdate() = 0;
	virtual void AddObject2DUpdate() = 0;
	virtual void AddBeatEndUpdate() = 0;

	//描画
	virtual void AddObject3DDraw() = 0;
	virtual void AddParticleDraw() = 0;
	virtual void AddFrontUIDraw() = 0;
	virtual void AddBackUIDraw() = 0;

	//後処理
	virtual void AddObjectFinalize() = 0;
	virtual void AddCommonFinalize() = 0;


protected:
	//共通
	//衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;

	//持ち越し
	//セーブ体力
	int saveHP_ = 0;
	//階層
	int floorValue_ = 1;

	//player
	unique_ptr<Player> player_;
	//リズムカウント
	unique_ptr<RhythmManager> rhythmManager_;
	//ゲームマネージャー
	unique_ptr<GameManager> gameManager_;
	//リズム入力
	bool isRhythmInput_ = false;
	//出口
	unique_ptr<Exit> exit_;
	//必要コイン
	int needCoin_ = 0;

	//エリアマネージャー
	unique_ptr<AreaManager> areaManager_;

	//Prev
	bool isPrevSceneChange_ = true;
	//Next
	bool isNextSceneChange_ = false;

	//GameOver
	bool isGameEnd_ = false;

	//BGM再生フラグ
	bool isBGMStart_ = true;

	//リズム目視確認用SP
	unique_ptr<JudgeLocation> judgeLoca_;

	//ノーツ数
	const int notesNum= 5;
	vector<unique_ptr<Notes>> lNotes;
	vector<unique_ptr<Notes>> rNotes;
	bool isNoteInput_ = false;


	//リザルト
	bool isResult = false;
	//行先
	bool isTitle = false;
	bool isHome = false;
	vector<unique_ptr<BaseSprites>> resultText;
	int select = 0;
	unique_ptr<BaseSprites> backSp;
};