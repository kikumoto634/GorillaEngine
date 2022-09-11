#pragma once
#include <string>

#include "TextureManager.h"
#include "Sprite.h"

class Window;
class GameSceneBase
{
public:
	using string = std::string;

public:
	virtual ~GameSceneBase();
	virtual void Initialize();
	virtual void Update() = 0;
	virtual void Draw();

	bool GetIsEnd() { return isEnd; }
	bool GetIsAllEnd() { return isAllEnd; }
	const string& GetNowScene() { return nowScene; }
	const string& GetNextScene() { return nextScene; }

protected:
	virtual void ShutDown() { isEnd = true; }
	virtual void ShutDownAll() { isAllEnd = true; }
	
	Window* pWindow;
	bool isEnd;
	bool isAllEnd;
	string nowScene;
	string nextScene;
};

