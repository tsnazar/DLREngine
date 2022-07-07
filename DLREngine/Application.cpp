#include "Application.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "KeyEvents.h"

namespace
{
	const float FRAME_DURATION = 1.f / 60.f;
}

namespace engine
{
	Application* Application::s_Instance = nullptr;

	Application::Application(HINSTANCE hInstance, int nShowCmd, const std::string& name, uint32_t width, uint32_t height)
	{
		if (s_Instance != nullptr)
			ALWAYS_ASSERT(false);
		
		s_Instance = this;

		std::wstring stemp = std::wstring(name.begin(), name.end());

		m_Window = std::unique_ptr<MainWindow>(new MainWindow());
		//init window
		m_Window->Create(0, 0, { 0, 0, (long)width, (long)height }, stemp.c_str(), WS_OVERLAPPEDWINDOW, NULL, NULL, hInstance, NULL);
		m_Window->Show(nShowCmd);

		std::function<void(engine::Event&)> f = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		m_Window->BindEventCallback(f);

		m_Scene = std::unique_ptr<Scene>(new Scene());
		//init scene
		m_Scene->GetShader().LoadFromFile(engine::VertexType::PosCol, "shaders/shader.shader");

		engine::VertexPosCol vertex_data_array[] = {
			{DirectX::XMFLOAT3{0.0f,  0.5f,  0.0f}, DirectX::XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}}, // point at top
			{DirectX::XMFLOAT3{0.5f, -0.5f,  0.0f }, DirectX::XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}}, // point at bottom-right
			{DirectX::XMFLOAT3{-0.5f, -0.5f,  0.0f}, DirectX::XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}}, // point at bottom-left
		};

		m_Scene->GetBuffer().Create(engine::VertexType::PosCol, vertex_data_array, 3);
	}

	void Application::Run()
	{
		float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

		auto prevFrame = std::chrono::steady_clock::now();
		bool run = true;
		while (run)
		{
			run = m_Window->PoolEvents();

			auto currentFrame = std::chrono::steady_clock::now();
			auto delta = std::chrono::duration<float>(currentFrame - prevFrame).count();

			if (delta >= FRAME_DURATION)
			{
				prevFrame = currentFrame;
				m_Window->ClearColor(color);
				m_Scene->Render(*m_Window);
				m_Window->Flush();
				std::cout << delta << std::endl;
			}

			std::this_thread::yield();
		}
		
	}
	
	void Application::OnEvent(Event& e)
	{
		auto f = [&](Event& e) { std::cout << e << std::endl; return true; };

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(f);
		dispatcher.Dispatch<KeyReleasedEvent>(f);
	}
}