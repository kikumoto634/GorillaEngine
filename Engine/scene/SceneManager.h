#pragma once
#include "../../scene/BaseScene.h"

class SceneManager
{
public:
	~SceneManager();

	static SceneManager* GetInstance();
	static void Delete();

	void SetNextScene(BaseScene* lnextScene)	{nextScene = lnextScene;}

	void Update();
	void Draw();

private:
	//シングルトン
	static SceneManager* instance;

	BaseScene* scene = nullptr;
	//次シーン
	BaseScene* nextScene = nullptr;
};

