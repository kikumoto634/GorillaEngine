#include "AreaManager.h"

#include <fstream>
#include <cassert>
#include <algorithm>

#include "AudioUtility.h"

using namespace std;

const float AreaManager::DIV_NUM_FLOAT = (float)DIV_NUM;
const float AreaManager::DIV_NUM_HALF_FLOAT = (float)DIV_NUM_HALF;
const float AreaManager::Block_Size = 2.f;



void AreaManager::RandamAreaInitialize()
{
	CreateMap();
	ObjectRandomPop();

	CommonInitialize();

	AreaWallsInitialize(true);
	AreaPlaneInitialize(true);
	RandamAreaIndestructibleWallInitialize();
}

void AreaManager::CSVAreaInitialize(string name)
{
	CSVMapDataLoad(name);

	CommonInitialize();

	AreaPlaneInitialize();
	AreaWallsInitialize();
	CSVAreaIndestructibleWallInitialize();
}

void AreaManager::CommonInitialize()
{
	if(!planeModel_){
		planeModel_ = new ObjModelManager();
		planeModel_->CreateModel("GroundBlock");
	}

	if(!wallModel_){
		wallModel_ = new ObjModelManager();
		wallModel_->CreateModel("GroundBlock2");
	}
	if(!wallColliderModel_){
		wallColliderModel_ = new ObjModelManager();
		wallColliderModel_->CreateModel("GroundBlock2_Collider");
	}

	if(!indestructibleWallModel_){
		indestructibleWallModel_ = new ObjModelManager();
		indestructibleWallModel_->CreateModel("GroundBlock3");
	}
	if(!indestructibleWallColliderModel_){
		indestructibleWallColliderModel_ = new ObjModelManager();
		indestructibleWallColliderModel_->CreateModel("GroundBlock2_Collider");
	}

	digParticle_ = make_unique<ParticleObject>();
	digParticle_->Initialize();
}

void AreaManager::RandamAreaUpdate(Camera *camera, Vector3 PlayerPos)
{
	assert(camera);

	this->camera_ = camera;
	this->playerPos_ = PlayerPos;

	isDigSound_ = false;

	WallUpdate();
	IndestructibleWallUpdate();
	PlaneUpdate();

	CommonUpdate();
}

void AreaManager::CSVAreaUpdate(Camera* camera, Vector3 PlayerPos)
{
	assert(camera);

	isDigSound_ = false;

	this->camera_ = camera;
	this->playerPos_ = PlayerPos;

	WallUpdate();
	IndestructibleWallUpdate();
	PlaneUpdate();

	CommonUpdate();
}

void AreaManager::CommonUpdate()
{
	if(isDig_ && !isAlive_){
		DigParticlePop();
		isDig_ = false;
	}
	else if(isDigApp_){
		if(paricleApperanceFrame_ >= DigAppearanceFrame){
			isDigApp_ = false;
			paricleApperanceFrame_ = 0;
		}
		paricleApperanceFrame_++;
		digParticle_->Update(this->camera_);
	}
}

void AreaManager::BeatEndUpdate(GameManager* gameManager)
{
	assert(gameManager);

	this->gameManager_ = gameManager;

	PlaneBeatEndUpdate();
}

void AreaManager::RandamAreaDraw()
{
	WallDraw();
	IndestructibleWallDraw();
	PlaneDraw();
}

void AreaManager::CSVAreaDraw()
{
	WallDraw();
	IndestructibleWallDraw();
	PlaneDraw();
}

void AreaManager::ParticleDraw()
{
	if(isDigApp_){
		digParticle_->Draw();
	}
}

void AreaManager::RandamAreaFinalize()
{
	CommonFinalize();

	WallFinalize();
	IndestructibleWallFinalize();
	PlaneFinalize();
}

void AreaManager::CSVAreaFinalize()
{
	playerPos_ = {};

	CommonFinalize();

	WallFinalize();
	IndestructibleWallFinalize();
	PlaneFinalize();
}

void AreaManager::CommonFinalize()
{
	digParticle_->Finalize();
}

void AreaManager::BreakBlock(Vector2 pos)
{
	int blockNumX = +int(pos.x/Block_Size) + DIV_NUM_HALF;
	int blockNumZ = -int(pos.y/Block_Size) + DIV_NUM_HALF;

	if(blockNumX < 0 || blockNumZ < 0) return;
	//if(mapInfo[blockNumZ][blockNumX] != 1) return;
	mapInfo_[blockNumZ][blockNumX] = 1;
}


#pragma region 地面
void AreaManager::AreaPlaneInitialize(bool IsLighting)
{
	float startPos = -DIV_NUM_HALF_FLOAT;
	Vector2 pos = {};

	for(int i = 0; i < DIV_NUM; i++){
		plane_.resize(i+1);
		for(int j = 0; j < DIV_NUM; j++){
			if(mapInfo_[i][j] == 0){
				continue;
			}
			unique_ptr<Planes> obj = make_unique<Planes>();
			obj->Initialize(planeModel_);
			pos = {startPos + j, startPos + i};
			pos *= Block_Size;
			obj->Pop({pos.x,-5,-pos.y});

			if(!IsLighting) {
				plane_[i].push_back(move(obj));
				continue;
			}
			obj->CaveLightOn();
			plane_[i].push_back(move(obj));
		}
	}
}
void AreaManager::PlaneUpdate()
{
	//地面
	for(size_t i = 0; i < plane_.size(); i++){
		for(auto it = plane_[i].begin(); it != plane_[i].end(); ++it){
			(*it)->SetPlayerPos(playerPos_);
			(*it)->Update(camera_);
		}
	}
}
void AreaManager::PlaneBeatEndUpdate()
{
	isComboColorChange_ = !isComboColorChange_;
	bool IsChange = false;

	for(size_t i = 0; i < plane_.size(); i++){
		for(auto it = plane_[i].begin(); it != plane_[i].end(); ++it){
			//コンボ数に応じて色変化
			if(gameManager_->GetComboNum() >= gameManager_->GetPlaneColorChangeCombo()){
				(*it)->PlaneColorChange(IsChange, isComboColorChange_);
				IsChange = !IsChange;
			}
			else if(gameManager_->GetComboNum() < 10){
				(*it)->PlaneColorReset();
			}

			(*it)->IsBeatEndOn();
		}
	}
}
void AreaManager::PlaneDraw()
{
	for(size_t i = 0; i < plane_.size(); i++){
		for(auto it = plane_[i].begin(); it != plane_[i].end(); ++it){
			(*it)->Draw();
		}
	}
}
void AreaManager::PlaneFinalize()
{
	delete planeModel_;
	for(size_t i = 0; i < plane_.size(); i++){
		for(auto it = plane_[i].begin(); it != plane_[i].end(); ++it){
			(*it)->Finalize();
		}
	}
}
#pragma endregion

#pragma region 壁
void AreaManager::AreaWallsInitialize(bool IsLigthing)
{
	float startPos = -DIV_NUM_HALF_FLOAT;
	Vector2 pos = {};

	for(int i = 0; i < DIV_NUM; i++){
		wall_.resize(i+1);
		for(int j = 0; j < DIV_NUM; j++){
			
			if(mapInfo_[i][j] != 3){
				continue;
			}

			unique_ptr<Walls> obj = make_unique<Walls>();
			obj->Initialize(wallModel_,wallColliderModel_);

			pos = {startPos + j, startPos + i};
			pos*= Block_Size;
			obj->Pop({pos.x,-3,-pos.y});

			if(!IsLigthing) {
				wall_[i].push_back(move(obj));
				continue;
			}
			obj->CaveLightOn();
			wall_[i].push_back(move(obj));
		}
	}
}
void AreaManager::WallUpdate()
{
	//地面
	for(size_t i = 0; i < wall_.size(); i++){
		for(auto it = wall_[i].begin(); it != wall_[i].end(); ++it){
			(*it)->SetPlayerPos(playerPos_);
			if((*it)->GetIsDIg()){
				isDigSound_ = true;
				isDig_ =true;
				digParticlePos_ = (*it)->GetDigPosition();
				gameManager_->AudioPlay(dig_audio.number, dig_audio.volume);
			}
			(*it)->Update(camera_);
		}
	}
}
void AreaManager::WallBeatEndUpdate()
{
}
void AreaManager::WallDraw()
{
	for(size_t i = 0; i < wall_.size(); i++){
		for(auto it = wall_[i].begin(); it != wall_[i].end(); ++it){
			(*it)->Draw();
		}
	}
}
void AreaManager::WallFinalize()
{

	delete wallModel_;
	wallModel_ = nullptr;
	delete wallColliderModel_;
	wallColliderModel_ = nullptr;

	for(size_t i = 0; i < wall_.size(); i++){
		for(auto it = wall_[i].begin(); it != wall_[i].end(); ++it){
			(*it)->Finalize();
		}
	}
}
#pragma endregion

#pragma region 破壊不可壁
void AreaManager::RandamAreaIndestructibleWallInitialize()
{
	float startPos = -DIV_NUM_HALF_FLOAT;
	Vector2 pos = {};

	for(int i = 0; i < 4; i++){
		indestructibleWalls_.resize(i + 1);
		for(int j = 0; j < DIV_NUM; j++){
			unique_ptr<IndestructibleWall> obj = make_unique<IndestructibleWall>();
			obj->Initialize(indestructibleWallModel_, indestructibleWallColliderModel_);

			if(i == 0){
				pos = {startPos + j, startPos - 1};
				pos*=Block_Size;
			}
			else if(i == 1){
				pos = {startPos + j, DIV_NUM_HALF_FLOAT+1};
				pos*=Block_Size;
			}
			else if(i == 2){
				pos = {startPos - 1, startPos + j};
				pos*=Block_Size;
			}
			else if(i == 3){
				pos = {DIV_NUM_HALF_FLOAT+1, startPos + j};
				pos*=Block_Size;
			}
			obj->Pop({pos.x, -3,pos.y});
			obj->CaveLightOn();

			indestructibleWalls_[i].push_back(move(obj));
		}
	}
}
void AreaManager::CSVAreaIndestructibleWallInitialize()
{
	float startPos = -DIV_NUM_HALF_FLOAT;
	Vector2 pos = {};

	for(int i = 0; i < DIV_NUM; i++){
		indestructibleWalls_.resize(i+1);
		for(int j = 0; j < DIV_NUM; j++){
			if(mapInfo_[i][j] != 2){
				continue;
			}

			unique_ptr<IndestructibleWall> obj = make_unique<IndestructibleWall>();
			obj->Initialize(indestructibleWallModel_, indestructibleWallColliderModel_);
			pos = {startPos + j ,startPos + i};
			pos *= Block_Size;
			obj->Pop({pos.x,-3,-pos.y});

			indestructibleWalls_[i].push_back(move(obj));
		}
	}
}
void AreaManager::IndestructibleWallUpdate()
{
	//地面
	for(size_t i = 0; i < indestructibleWalls_.size(); i++){
		for(auto it = indestructibleWalls_[i].begin(); it != indestructibleWalls_[i].end(); ++it){
			(*it)->SetPlayerPos(playerPos_);
			if((*it)->GetIsReflect()){
				gameManager_->AudioPlay(reflected_audio.number,reflected_audio.volume);
			}
			(*it)->Update(camera_);
		}
	}
}
void AreaManager::IndestructibleWallDraw()
{
	for(size_t i = 0; i < indestructibleWalls_.size(); i++){
		for(auto it = indestructibleWalls_[i].begin(); it != indestructibleWalls_[i].end(); ++it){
			(*it)->Draw();
		}
	}
}
void AreaManager::IndestructibleWallFinalize()
{
	delete indestructibleWallModel_;
	indestructibleWallModel_ = nullptr;

	delete indestructibleWallColliderModel_;
	indestructibleWallColliderModel_ = nullptr;

	for(size_t i = 0; i < indestructibleWalls_.size(); i++){
		for(auto it = indestructibleWalls_[i].begin(); it != indestructibleWalls_[i].end(); ++it){
			(*it)->Finalize();
		}
	}
}
#pragma endregion

#pragma region ダンジョン自動生成
void AreaManager::CreateMap()
{
	for(int y = 0; y < DIV_NUM; y++){
		mapInfo_.resize(y+1);
		for(int x = 0; x < DIV_NUM; x++){
			mapInfo_[y].push_back(3);
		}
	}

	DevideRoom({0,0,DIV_NUM,DIV_NUM}, false);
}
AreaManager::Room AreaManager::DevideRoom(Area area, bool hr)
{
	if(area.Width < 8 || area.Height < 8) {
		return CreateRoom(area);
	}

	if(!hr){
		//領域の高さ 1/3~2/3の範囲でランダム
		//24 : 1/3 = 8, 2/3 = 16
		//分割ライン水平
		int divline = static_cast<int>(floor(area.Height/3 + rand()%(area.Height/3)));

		//親(上)
		Area parent = {area.X,area.Y, area.Width,divline};
		//子(下)
		Area child = {area.X,area.Y+divline, area.Width,area.Height-divline};

		//親領域に部屋作成
		Room parentRoom = CreateRoom(parent);
		//子領域に部屋作成
		Room childRoom = DevideRoom(child, !hr);

		//通路開拓
		ConnectRoom(parentRoom, childRoom, divline, hr);

		return parentRoom;
	}
	else if(hr){
		//領域の高さ 1/3~2/3の範囲でランダム
		//48 : 1/3 = 16, 2/3 = 32
		//分割ライン水平
		//int divline = rand()%(area.Width/3)+ area.Width/3;
		int divline = static_cast<int>(floor(area.Width/3 + rand()%(area.Width/3)));

		//親(上)
		Area parent = {area.X,area.Y, divline,area.Height};
		//子(下)
		Area child = {area.X+divline,area.Y, area.Width-divline,area.Height};

		//親領域に部屋作成
		Room parentRoom = CreateRoom(parent);
		//子領域に部屋作成
		Room childRoom = DevideRoom(child, !hr);

		//通路開拓
		ConnectRoom(parentRoom, childRoom, divline, hr);

		return parentRoom;
	}
	return CreateRoom(area);
}
AreaManager::Room AreaManager::CreateRoom(Area area)
{
//範囲内でランダムに4点を取得
	int X1 = rand()%(area.Width) + area.X;
	int X2 = rand()%(area.Width) + area.X;
	int Y1 = rand()%(area.Height) + area.Y;
	int Y2 = rand()%(area.Height) + area.Y;

	//Xの始点、小さいほう
	int X = min(X1,X2);
	//差分が幅
	int W = abs(X1 - X2) + 2;
	//Yの始点、小さいほう
	int Y = min(Y1,Y2);
	//差分が高さ
	int H = abs(Y1 - Y2) + 2;

	Room room = {X,Y,W,H};
	rooms_.push_back(room);

	//マップに反映
	for(int y = 0; y < H-1; y++){
		for(int x = 0; x < W-1; x++){
			mapInfo_[Y+y][X+x] = 1;
		}
	}

	return room;
}
void AreaManager::ConnectRoom(Room parent, Room childRoom, int divline, bool hr)
{
	if(hr){
		//親部屋ないからランダムに一点を選択
		int X1 = parent.X + rand()%(parent.Width-1);
		//子部屋
		int X2 = childRoom.X + rand()%(childRoom.Width-1);
		//小さい
		int minX = min(X1,X2);
		//大きい
		int maxX = max(X1,X2);

		//マップに分割ライン上の通路作成
		for(int i = 0; minX + i < maxX; i++){
			mapInfo_[divline][minX + i] = 1;
		}
		//分割ラインから親部屋への通路
		for(int i = 1; mapInfo_[divline-i][X1] == 3 && divline - i != 0; i++){
			mapInfo_[divline-i][X1] = 1;
		}
		//子部屋へ
		for(int i = 1; mapInfo_[divline+i][X2] == 3 && divline + i != DIV_NUM-1; i++){
			mapInfo_[divline+i][X2] = 1;
		}
	}
	else if(!hr){
		//親部屋ないからランダムに一点を選択
		int Y1 = parent.Y + rand()%(parent.Height-1);
		//子部屋
		int Y2 = childRoom.Y + rand()%(childRoom.Height-1);
		//小さい
		int minY = min(Y1,Y2);
		//大きい
		int maxY = max(Y1,Y2);

		//マップに分割ライン上の通路作成
		for(int i = 0; minY + i < maxY; i++){
			mapInfo_[minY + i][divline] = 1;
		}
		//分割ラインから親部屋への通路
		for(int i = 1; mapInfo_[Y1][divline-i] == 3 && divline - i != 0; i++){
			mapInfo_[Y1][divline-i] = 1;
		}
		//子部屋へ
		for(int i = 1; mapInfo_[Y2][divline+i] == 3 && divline + i != DIV_NUM-1; i++){
			mapInfo_[Y2][divline+i] = 1;
		}
	}
}
#pragma endregion

#pragma region CSVデータ ダンジョン生成
void AreaManager::CSVMapDataLoad(string fullPath)
{
	string directoryPath = "Resources/CSVData/";
	string csv = ".csv";
	//file開く
	ifstream file;
	string Path = directoryPath + fullPath + csv;
	file.open(Path);
	assert(file.is_open());

	//file培養を文字列ストリームにコピー
	csvCommands_ << file.rdbuf();
	file.close();

	//一行分の文字列を入れる変数
	string line;
	int y = 0;
	int x = 0;

	float start = -DIV_NUM_HALF_FLOAT;
	Vector2 pos = {};

	//コマンドループ
	while(getline(csvCommands_, line)){
		//一行分のっ文字列をストリームに変換して解析しやすく
		istringstream line_stream(line);

		string word;
		string area;

		//区切りで行の銭湯文字列取得
		getline(line_stream, word, ',');

		x = 0;
		mapInfo_.resize(y+1);
		for(int i = 0; i < DIV_NUM*DIV_NUM; i++){

			if(word.find("0") == 0){
				mapInfo_[y].push_back(0);
				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("1") == 0){
				mapInfo_[y].push_back(1);
				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("2") == 0){
				mapInfo_[y].push_back(2);
				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("3") == 0){
				mapInfo_[y].push_back(3);
				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("4") == 0){
				mapInfo_[y].push_back(1);

				pos = {start + x, start + y};
				pos *= Block_Size;
				playerPopPosition_ = {pos.x, -3, -pos.y};

				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("5") == 0){
				mapInfo_[y].push_back(1);

				pos = {start + x, start + y};
				pos *= Block_Size;
				objectPos_.push_back({pos.x,0,-pos.y});
				objectPopActive_.push_back(true);

				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("6") == 0){
				mapInfo_[y].push_back(1);

				pos = {start + x, start + y};
				pos *= Block_Size;
				exitPosition_ = {pos.x,-5,-pos.y};

				getline(line_stream, word, ',');
				x++;
			}
			else if(word.find("//") == 0){
				y++;
				break;
			}
		}
	}
}

Vector3 AreaManager::GetCSVObjectPopPosition(int index)
{
	return objectPos_[index];
}
bool AreaManager::GetCSVObjectPopActive(int index, bool IsFlag)
{
	if(objectPopActive_[index]){
		objectPopActive_[index] = IsFlag;
		return true;
	}

	return false;
}
#pragma endregion

#pragma region ランダム生成
void AreaManager::ObjectRandomPop()
{
	PlayerPop();
	ExitPop();
	TrapPop();
}

Vector3 AreaManager::GetObjectPopPosition()
{
	int roomSize = (int)rooms_.size();
	Vector2 areaPos;
	Vector2 areaWH;

	float start = -DIV_NUM_HALF_FLOAT;
	int RoomsNum = rand()%roomSize;

	areaPos = {(start+rooms_[RoomsNum].X),(start+rooms_[RoomsNum].Y)};
	areaWH = {float(rand()%(rooms_[RoomsNum].Width-1)),float(rand()%(rooms_[RoomsNum].Height-1))};
	areaPos*= Block_Size;
	areaWH *= Block_Size;
	objectPopPosition_ = {areaPos.x+areaWH.x,0.f,-(areaPos.y+areaWH.y)};

	return objectPopPosition_;
}
#pragma endregion

void AreaManager::DigParticlePop()
{
	isDigApp_ = true;
	for (int i = 0; i < 3; i++) {
		const float rnd_vel = 0.08f;
		Vector3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = 0.06f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		Vector3 acc{};
		acc.y = -0.005f;

		digParticle_->ParticleSet(DigAppearanceFrame,digParticlePos_,vel,acc,0.4f,0.0f,1,{0.5f,0.3f,0.2f,1.f});
		digParticle_->ParticleAppearance();
	}
}


void AreaManager::PlayerPop()
{
	int roomSize = (int)rooms_.size();
	Vector2 areaPos;
	Vector2 areaWH;

	float start = -DIV_NUM_HALF_FLOAT;

	int playerRoomsNumber = rand()%roomSize;
	areaPos = {float(start+rooms_[playerRoomsNumber].X),float(start+rooms_[playerRoomsNumber].Y)};
	areaWH = {float(rand()%(rooms_[playerRoomsNumber].Width-1)),float(rand()%(rooms_[playerRoomsNumber].Height-1))};
	areaPos*= Block_Size;
	areaWH *= Block_Size;
	playerPopPosition_ = {areaPos.x+areaWH.x,-3.f,-(areaPos.y+areaWH.y)};
}

void AreaManager::ExitPop()
{
	int roomSize = (int)rooms_.size();
	Vector2 areaPos;
	Vector2 areaWH;

	float start = -DIV_NUM_HALF_FLOAT;

	//exit
	int exitRoomsNumber = rand()%roomSize;
	areaPos = {float(start+rooms_[exitRoomsNumber].X), float(start+rooms_[exitRoomsNumber].Y)};
	areaWH = {float(rand()%(rooms_[exitRoomsNumber].Width-1)),float(rand()%(rooms_[exitRoomsNumber].Height-1))};
	areaPos*= Block_Size;
	areaWH *= Block_Size;
	exitPosition_ = {areaPos.x+areaWH.x,-5.f,-(areaPos.y+areaWH.y)};
}

void AreaManager::TrapPop()
{
	int roomSize = (int)rooms_.size();
	Vector2 areaPos;
	Vector2 areaWH;

	float start = -DIV_NUM_HALF_FLOAT;

	for(int i = 0; i < 10; i++){
		int trapRoomsNumber = i;
		if(trapRoomsNumber >= roomSize) trapRoomsNumber = 0;

		areaPos = {float(start+rooms_[trapRoomsNumber].X),float(start+rooms_[trapRoomsNumber].Y)};
		areaWH = {float(rand()%(rooms_[trapRoomsNumber].Width-1)),float(rand()%(rooms_[trapRoomsNumber].Height-1))};
		areaPos*= Block_Size;
		areaWH *= Block_Size;

		Vector3 lpos = {(areaPos.x+areaWH.x), 0 ,-(areaPos.y+areaWH.y)};
		trapPopPosition_.push_back(lpos);
	}
}
