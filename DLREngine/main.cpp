#include <Windows.h>
#include <Windowsx.h>

#include <thread>
#include <vector>
#include <memory>
#include <chrono>

#include "ray.h"
#include "sphere.h"
#include "mat4.h"

#include "windows/MainWindow.h"
#include "Scene.h"

void initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	initConsole();

	Scene scene;
		
	MainWindow window;
	window.Create(0, 0, { 0, 0, 200, 100 }, L"MainWindow", WS_OVERLAPPEDWINDOW, NULL, NULL, hInstance, NULL);
	window.Show(nShowCmd);

	MSG msg;

	auto prevFrame = std::chrono::steady_clock::now();
	bool run = true;
	while (run)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				run = false;
				break;
			}
		}
		auto currentFrame = std::chrono::steady_clock::now();
		auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentFrame - prevFrame).count();
			
		if (delta >= 16)
		{
			prevFrame = currentFrame;
			scene.ProcessInput(delta/1000.f);
			scene.Render(window);
		}

		std::this_thread::yield();
	}
}
