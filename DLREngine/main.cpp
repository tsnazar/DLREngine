#include <Windows.h>
#include <Windowsx.h>

#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

#include "windows/MainWindow.h"
#include "Globals.h"
#include "ResourceManager.h"

#include "Application.h"


void initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	initConsole();

	engine::Globals* glob = new engine::Globals;
	engine::ResourceManager* manager = new engine::ResourceManager;
	engine::Application* app = new engine::Application(hInstance, nShowCmd);
	
	app->Run();
	
	delete app;
	delete manager;
	delete glob;

	return 0;
}
