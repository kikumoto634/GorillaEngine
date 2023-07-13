#pragma once
#include <sstream>
#include <string>

#include "Planes.h"
#include "Walls.h"
#include "IndestructibleWall.h"

#include "GameManager.h"

class AreaManager
{
private:
	template <class T> using unique_ptr = std::unique_ptr<T>;
	template <class T> using vector = std::vector<T>;

public:
	static const int DIV_NUM = 31;
	static const float DIV_NUM_FLOAT;
	static const int DIV_NUM_HALF = 15;
	static const float DIV_NUM_HALF_FLOAT;
	static const float Block_Size;

private:
	//壁破壊パーティクルのリセット時間
	const int DigAppearanceFrame = 50;

private:
	//領域
struct Area{
	int X;
	int Y;

	int Width;
	int Height;
};

//部屋
struct Room{
	int X;
	int Y;

	int Width;
	int Height;
};

public:
	void RandamAreaInitialize();
	void CSVAreaInitialize(std::string name);

	void CommonInitialize();

	void RandamAreaUpdate(Camera* camera, Vector3 PlayerPos);
	void CSVAreaUpdate(Camera* camera, Vector3 PlayerPos);

	void CommonUpdate();

	void BeatEndUpdate(GameManager* gameManager);

	void RandamAreaDraw();
	void CSVAreaDraw();

	void ParticleDraw();

	void RandamAreaFinalize();
	void CSVAreaFinalize();

	void CommonFinalize();

	//ブロック破壊
	void BreakBlock(Vector2 pos);

	//ランダム用
	Vector3 GetExitPosition()	{return exitPosition_;}
	Vector3 GetPlayerPosition()	{return playerPopPosition_;}
	Vector3 GetObjectPopPosition();
	vector<Vector3> GetTrapPopPosition()	{return trapPopPosition_;}
	
	//CSV用
	Vector3 GetCSVObjectPopPosition(int index);
	bool GetCSVObjectPopActive(int index, bool IsFlag = true);
	int GetCSVObjectSize()	{return (int)objectPos_.size();}

	//Getter
	inline std::vector<std::vector<int>> GetMapInfo()	{return mapInfo_;}

private:
	//地面
	void AreaPlaneInitialize(bool IsLighting = false);
	void PlaneUpdate();
	void PlaneBeatEndUpdate();
	void PlaneDraw();
	void PlaneFinalize();

	//壁
	void AreaWallsInitialize(bool IsLigthing = false);
	void WallUpdate();
	void WallBeatEndUpdate();
	void WallDraw();
	void WallFinalize();

	//破壊不可
	void RandamAreaIndestructibleWallInitialize();
	void CSVAreaIndestructibleWallInitialize();
	void IndestructibleWallUpdate();
	void IndestructibleWallDraw();
	void IndestructibleWallFinalize();

	//ダンジョン生成
	void CreateMap();
	Room DevideRoom(Area area, bool hr = false);
	Room CreateRoom(Area area);
	void ConnectRoom(Room parent, Room childRoom, int divline, bool hr);

	//CSV読み込み
	void CSVMapDataLoad(std::string fullPath);

	//座標
	void ObjectRandomPop();

	//壁破壊パーティクル発生
	void DigParticlePop();

	//オブジェクト発生
	void PlayerPop();
	void ExitPop();
	void TrapPop();
	

private:
	//借りもの
	Camera* camera_ = nullptr;
	GameManager* gameManager_ = nullptr;
	Vector3 playerPos_={};

	//Plane
	ObjModelManager* planeModel_ = nullptr;
	vector<vector<unique_ptr<Planes>>> plane_;
	bool isComboColorChange_ = false;

	//Wall
	ObjModelManager* wallModel_ = nullptr;
	ObjModelManager* wallColliderModel_ = nullptr;
	bool isAlive_ = false;
	vector<vector<unique_ptr<Walls>>> wall_;

	//破壊不可能壁
	ObjModelManager* indestructibleWallModel_ = nullptr;
	ObjModelManager* indestructibleWallColliderModel_ = nullptr;
	vector<vector<unique_ptr<IndestructibleWall>>> indestructibleWalls_;

	//部屋情報
	/*
	* 0:無
	* 1:道
	* 2:不可壁
	* 3:可能壁
	*/
	vector<vector<int>> mapInfo_;

	//ランダム生成部屋
	vector<Room> rooms_;


	//CSVコマンド
	std::stringstream csvCommands_;
	vector<Vector3> objectPos_;
	vector<bool> objectPopActive_;


	//壁破壊パーティクル
	bool isDig_ = false;
	bool isDigApp_ = false;
	int paricleApperanceFrame_ = 0;
	Vector3 digParticlePos_ = {};
	unique_ptr<ParticleObject> digParticle_;
	bool isDigSound_ = false;

	//座標
	//出口
	Vector3 exitPosition_ = {0,-5,0};

	//プレイヤー
	Vector3 playerPopPosition_ = {0,-3,0};

	//生成オブジェクト
	Vector3 objectPopPosition_ = {0,0,0};

	//トラップ
	vector<Vector3> trapPopPosition_ = {};
};

