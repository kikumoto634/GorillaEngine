#include "SceneManager.h"

SceneManager* SceneManager::instance = nullptr;

SceneManager::~SceneManager()
{
	scene->Finalize();
	delete scene;
}

SceneManager *SceneManager::GetInstance()
{
	if(!instance)
	{
		instance = new SceneManager();
	}
	return instance;
}

void SceneManager::Delete()
{
	if(instance){
		delete instance;
		instance = nullptr;
	}
}

void SceneManager::Update()
{
	if(nextScene){

#ifdef _DEBUG
		DebugText* text = nextScene->GetDebugText();
		imguiManager* imgui = nextScene->GetImGui();
#endif // _DEBUG
		
		if(scene){

#ifdef _DEBUG
			text = scene->GetDebugText();
			imgui = scene->GetImGui();
#endif // _DEBUG

			scene->Finalize();
			delete scene;
		}

		//�V�[���؂�ւ�
		scene = nextScene;

#ifdef _DEBUG
		scene->SetDebugText(std::move(text));
		scene->SetImGui(std::move(imgui));
#endif // _DEBUG

		nextScene = nullptr;

		//�V�[���}�l�[�W���[�Z�b�g
		scene->SetSceneManager(this);

		//������
		scene->Application();
		scene->Initialize();
	}

	scene->Update();
}

void SceneManager::Draw()
{
	scene->Draw();
}