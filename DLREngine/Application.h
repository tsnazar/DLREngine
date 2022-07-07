#pragma once

#include "windows/MainWindow.h"
#include "Scene.h"
#include "Event.h"

namespace engine
{
	class Application
	{
	public:
		Application(HINSTANCE hInstance, int nShowCmd, const std::string& name = "MainApp", uint32_t width = 1280, uint32_t height = 720);
		virtual ~Application() = default;

		void Run();
		void OnEvent(Event& e);

		inline MainWindow& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		std::unique_ptr<MainWindow> m_Window;
		std::unique_ptr<Scene> m_Scene;
		bool m_Running = true;
	private:
		static Application* s_Instance;
	};
}