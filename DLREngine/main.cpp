#include <Windows.h>
#include <Windowsx.h>

#include <thread>
#include <vector>
#include <memory>

#include "ray.h"
#include "sphere.h"
#include "mat4.h"

#include "windows/MainWindow.h"
#include "Scene.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	{
		Scene scene;
		
		MainWindow window;
		window.Create(0, 0, { 0, 0, 200, 100 }, L"MainWindow", WS_OVERLAPPEDWINDOW, NULL, NULL, hInstance, NULL);
		window.Show(nShowCmd);

		MSG msg;

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
			scene.ProcessInput();
			scene.Render(window);

			std::this_thread::yield();
		}
	}

	getchar();
}
