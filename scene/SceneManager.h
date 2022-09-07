#pragma once
#include "GameSceneBase.h"
#include <map>
#include <string>

class SceneManager
{
public:
	using string = std::string;

public:
	SceneManager();
	~SceneManager();

	void Update();
	void Draw();

	//シーン追加
	void Add(const string& name, GameSceneBase* scene);
	//チェンジ
	void Change(const string& name);
	//クリア
	void Clear();

	//現在
	string GetNowScene();
	string GetNextScene();
	bool GetIsAlive();

private:
	//全名前と全シーンを所持
	std::map<std::string, GameSceneBase*> scenes;

	//現在
	GameSceneBase* nowScene;

	bool IsAllEnd;
};

