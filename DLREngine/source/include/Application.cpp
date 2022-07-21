#include "Application.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "KeyEvents.h"
#include "TextureManager.h"
#include "ShaderManager.h"

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

		ShaderManager::Get().LoadShader("shader", VertexType::PosTex, "shaders/shader.hlsl");
		ShaderManager::Get().LoadShader("skybox", VertexType::Undefined, "shaders/sky.hlsl");
		TextureManager::Get().LoadTexture2D("container", "assets/container2.dds");
		TextureManager::Get().LoadCubemap("skybox", "assets/cubemap.dds");

		VertexPosTex vertexData[] = {
			//back
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}},
			//front
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.0f, 1.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}}, { DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}},
			//right
			{ DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}},
			//left
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}},
			//bottom
			{ DirectX::XMFLOAT3{-0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{0.5f, -0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, -0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 0.f}},
			//top
			{ DirectX::XMFLOAT3{-0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{0.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}},
			{ DirectX::XMFLOAT3{0.5f, 0.5f, -0.5f}, DirectX::XMFLOAT2{1.f, 1.f}}, { DirectX::XMFLOAT3{-0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{0.f, 0.f}}, { DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT2{1.f, 0.f}},
		};

		m_Scene->GetBuffer().Create<VertexPosTex>(D3D11_USAGE_IMMUTABLE, vertexData, 36);

		m_Scene->GetSky().SetSky("skybox", "shaders/sky.hlsl", "assets/cubemap.dds");
		
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
		EventDispatcher dispatcher(e);
		
		dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& e)
			{
				switch (e.GetKeyCode())
				{
				case Key::ONE:
					Globals::Get().SetCurrentSampler(1);
					break;
				case Key::TWO:
					Globals::Get().SetCurrentSampler(2);
					break;
				case Key::THREE:
					Globals::Get().SetCurrentSampler(3);
					break;
				case Key::FOUR:
					Globals::Get().SetCurrentSampler(4);
					break;
				}
				return true;
			});

		m_CameraController->OnEvent(e);

	}
	
	void Application::OnUpdate(float delta)
	{
		using namespace DirectX;

		m_CameraController->OnUpdate(delta);

		PerFrame& p = Globals::Get().GetPerFrameObj();

		Camera& camera = m_CameraController->GetCamera();
		
		XMStoreFloat4x4(&p.viewProj, XMMatrixTranspose(camera.GetViewProj()));

		XMVECTOR TL, TR, BL, xDir, yDir;
		TL = camera.Unproject(XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f)) - camera.Position();
		TR = camera.Unproject(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)) - camera.Position();
		BL = camera.Unproject(XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f)) - camera.Position();
		xDir = TR - TL;
		yDir = BL - TL;
		XMStoreFloat4(&(p.frustumCorners[0]), TL);
		XMStoreFloat4(&(p.frustumCorners[1]), xDir);
		XMStoreFloat4(&(p.frustumCorners[2]), yDir);

	}
	
	void Application::Render()
	{
		float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

		m_Window->ClearColor(color);
		m_Window->BindRenderTarget();
		Globals::Get().Bind();

		m_Scene->Render(*m_Window, m_CameraController->GetCamera());
		m_Window->Flush();
	}
}