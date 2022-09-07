#pragma once
#include <string>


class GameSceneBase
{
public:
	using string = std::string;

public:
	virtual ~GameSceneBase() = default;
	virtual void Initialize()= 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool GetIsEnd() { return isEnd; }
	bool GetIsAllEnd() { return isAllEnd; }
	const string& GetNowScene() { return nowScene; }
	const string& GetNextScene() { return nextScene; }

protected:
	virtual void ShutDown() { isEnd = true; }
	virtual void ShutDownAll() { isAllEnd = true; }
	
	//Window* pWindow;
	bool isEnd;
	bool isAllEnd;
	string nowScene;
	string nextScene;
};

