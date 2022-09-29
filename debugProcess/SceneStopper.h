#pragma once
class SceneStopper
{
public:
	static SceneStopper* GetInstance();

	bool GetIsSceneStop()	{return IsSceneStop;}
	void SetIsSceneStop(bool IsFlag)	{IsSceneStop = IsFlag;}

private:
	bool IsSceneStop = false;
};

