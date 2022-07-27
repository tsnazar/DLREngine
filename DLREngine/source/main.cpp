#include <Windows.h>
#include <Windowsx.h>

#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

#include "windows/MainWindow.h"
#include "Globals.h"

#include "Application.h"
#include "Engine.h"

void initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	initConsole();

	engine::Engine::Init();
	engine::Application* app = new engine::Application(hInstance, nShowCmd);
	
	app->Run();
	
	delete app;
	engine::Engine::Fini();

	/*Assimp::Importer importer;
	uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	const aiScene* scene = importer.ReadFile("assets/Samurai/Samurai.fbx", flags);

	std::cout << scene->mNumMeshes << std::endl;*/

	return 0;
}
