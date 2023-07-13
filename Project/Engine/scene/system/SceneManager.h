#pragma once
#include "BaseScene.h"
#include "DirectXCommon.h"

class SceneManager
{
public:
	~SceneManager();

	static SceneManager* GetInstance();
	static void Delete();

	void SetNextScene(BaseScene* lnextScene)	{nextScene = lnextScene;}

	void Update();
	void Draw();
	void DrawBack();
	void DrawNear();

private:
	//シングルトン
	static SceneManager* instance;

	BaseScene* scene = nullptr;
	//次シーン
	BaseScene* nextScene = nullptr;
};

