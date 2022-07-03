#include <Windows.h>
#include <Windowsx.h>

#include <thread>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

#include "geometry/Ray.h"

#include "windows/MainWindow.h"
#include "Controller.h"
#include "Globals.h"


void initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	initConsole();

	engine::Globals glob;
	glob.InitD3D();

	Scene scene;
	Camera camera;
	
	MainWindow window;
	window.Create(0, 0, { 0, 0, 300, 300 }, L"MainWindow", WS_OVERLAPPEDWINDOW, NULL, NULL, hInstance, NULL);
	window.Show(nShowCmd);

	Controller controller(scene, camera, window);
	controller.InitScene();
	controller.InitCamera();

	std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> f = std::bind(&Controller::ProcessEvents, &controller, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	window.BindEventCallback(f);

	return controller.Run();
}
