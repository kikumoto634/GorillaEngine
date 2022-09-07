#include "SceneManager.h"

using namespace std;

SceneManager::SceneManager()
{
	Clear();
}

SceneManager::~SceneManager()
{
	Clear();
}

void SceneManager::Update()
{
	if(!nowScene){
		return;
	}

	nowScene->Update();
	if(nowScene->GetIsAllEnd())
	{
		IsAllEnd = true;
	}
}

void SceneManager::Draw()
{
	if(!nowScene){
		return;
	}

	nowScene->Draw();
	if(nowScene->GetIsEnd())
	{
		Change(nowScene->GetNextScene());
	}
}

void SceneManager::Add(const string &name, GameSceneBase *scene)
{
	if(!scenes[name]){
		return;
	}
	scenes[name] = scene;
}

void SceneManager::Change(const string &name)
{
	if(!scenes[name]){
		return;
	}

	nowScene = scenes[name];
	nowScene->Initialize();
}

void SceneManager::Clear()
{
	for(auto itr = scenes.begin(); itr != scenes.end();++itr){
		delete (*itr).second;
		(*itr).second = nullptr;
	}
	scenes.clear();
}

string SceneManager::GetNowScene()
{
	return nowScene->GetNowScene();
}

string SceneManager::GetNextScene()
{
	return nowScene->GetNextScene();
}

bool SceneManager::GetIsAlive()
{
	return IsAllEnd;
}
