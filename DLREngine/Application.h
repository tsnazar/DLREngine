#pragma once

#include "windows/MainWindow.h"
#include "Scene.h"
#include "Event.h"
#include "CameraController.h"

namespace engine
{
	class Application
	{
	public:
		Application(HINSTANCE hInstance, int nShowCmd, const std::string& name = "MainApp", uint32_t width = 400, uint32_t height = 400);
		virtual ~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

		void Run();
		void OnEvent(Event& e);
		void OnUpdate(float delta);
		void Render();

		inline MainWindow& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		std::unique_ptr<MainWindow> m_Window;
		std::unique_ptr<Scene> m_Scene;
		std::unique_ptr<CameraController> m_CameraController;
		bool m_Running = true;
	private:
		static Application* s_Instance;
	};
}