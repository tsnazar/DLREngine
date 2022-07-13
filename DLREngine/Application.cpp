#include "Application.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "KeyEvents.h"
#include "ResourceManager.h"

namespace
{
	const float FOV = 1.0472f; // 60 degrees
	const float ZNEAR = 100.f;
	const float ZFAR = 0.1f;
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

		//init window
		m_Window = std::unique_ptr<MainWindow>(new MainWindow());
		m_Window->Create(0, 0, { 0, 0, (long)width, (long)height }, stemp.c_str(), WS_OVERLAPPEDWINDOW, NULL, NULL, hInstance, NULL);
		m_Window->Show(nShowCmd);

		std::function<void(Event&)> f = std::bind(&Application::OnEvent, this, std::placeholders::_1);
		m_Window->BindEventCallback(f);

		//init scene
		m_Scene = std::unique_ptr<Scene>(new Scene());

		ResourceManager::Get().LoadShader("shader", VertexType::PosTex, "shaders/shader.hlsl");
		ResourceManager::Get().LoadShader("skybox", VertexType::Undefined, "shaders/sky.hlsl");
		ResourceManager::Get().LoadTexture2D("container", "./textures/container2.dds");
		ResourceManager::Get().LoadCubemap("skybox", "./textures/cubemap.dds");

		VertexPosTex vertexData[] = {
			//back
			{ DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 0.f}},
			//front
			{ DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{0.0f, 1.0f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 0.f}}, { DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 0.f}},
			//right
			{ DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 0.f}},
			//left
			{ DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 0.f}},
			//bottom
			{ DirectX::XMFLOAT3{-1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{1.0f, -1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, -1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 0.f}},
			//top
			{ DirectX::XMFLOAT3{-1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{1.0f, 1.0f, -1.0f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}, DirectX::XMFLOAT2{1.f, 0.f}},
		};

		m_Scene->GetBuffer().Create<VertexPosTex>(D3D11_USAGE_IMMUTABLE, vertexData, 36);

		m_Scene->GetConstantBuffer().Create<Scene::FrustumCorners>(D3D11_USAGE_DYNAMIC, nullptr, 0);
		
		//init camera
		m_CameraController = std::unique_ptr<CameraController>(new CameraController(FOV, (float)width/(float)height, ZNEAR, ZFAR));
		m_CameraController->GetCamera().SetWorldOffset({0.f, 0.f, -5.f});
	}

	Application::~Application()
	{
		s_Instance = nullptr;
	}

	void Application::Run()
	{
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
				OnUpdate(delta);
				Render();
				std::cout << delta << std::endl;
			}

			std::this_thread::yield();
		}
	}
	
	void Application::OnEvent(Event& e)
	{
		m_CameraController->OnEvent(e);
	}
	
	void Application::OnUpdate(float delta)
	{
		PerFrame& p = Globals::Get().GetPerFrameObj();

		m_CameraController->OnUpdate(delta);
		
		DirectX::XMStoreFloat4x4(&p.viewProj, DirectX::XMMatrixTranspose(m_CameraController->GetCamera().GetViewProj()));

		Globals::Get().UpdateConstants();
	}
	
	void Application::Render()
	{
		float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

		m_Window->ClearColor(color);

		m_Scene->Render(*m_Window, m_CameraController->GetCamera());
		m_Window->Flush();
	}
}