#include <wrl.h>
#include "../Engine/base/Application.h"

using namespace Microsoft::WRL;

int WINAPI WinMain(HINSTANCE, HINSTANCE,LPSTR,int)
{
	Application* app = Application::GetInstance();

	app->Initialize();
	app->Run();
	app->Finalize();

	return 0;
}