#include "Application.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "KeyEvents.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "MathUtils.h"
#include "MeshSystem.h"

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
		Globals::Get().CreateDepthBuffer(m_Window->GetClientWidth(), m_Window->GetClientHeight());

		//init scene
		m_Renderer = std::unique_ptr<Renderer>(new Renderer());

		std::vector<D3D11_INPUT_ELEMENT_DESC> simple = {
			D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTex, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> instance = {
			D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTexNorTan, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, nor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, tan), D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[0]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[1]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[2]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[3]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};

		ShaderManager::Get().LoadShader("shader",  "shaders/shader.hlsl", &simple);
		ShaderManager::Get().LoadShader("instance",  "shaders/instance.hlsl", &instance);
		ShaderManager::Get().LoadShader("skybox", "shaders/sky.hlsl", nullptr);
		TextureManager::Get().LoadCubemap("skybox", "assets/cubemap.dds");

		Model* pCube = &ModelManager::Get().CreateModel("Cube");
		pCube->InitUnitCube();
		std::vector<OpaqueInstances::Material> cubeContainerTexture = { {&TextureManager::Get().LoadTexture2D("container", "assets/container2.dds")} };
		std::vector<OpaqueInstances::Material> cubeWallTexture = { {&TextureManager::Get().LoadTexture2D("wall", "assets/stonewall.dds")} };

		Model* pSamurai = &ModelManager::Get().LoadModel("Samurai", "assets/Samurai/Samurai.fbx");
		std::vector<OpaqueInstances::Material> samuraiTextures =
		{
			{&TextureManager::Get().LoadTexture2D("Sword", "assets/Samurai/dds/Sword_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Head", "assets/Samurai/dds/Head_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Eye", "assets/Samurai/dds/Eye_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Helmet", "assets/Samurai/dds/Helmet_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Torso", "assets/Samurai/dds/Torso_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Legs", "assets/Samurai/dds/Legs_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Hand", "assets/Samurai/dds/Hand_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Torso", "assets/Samurai/dds/Torso_BaseColor.dds")},
		};

		Model* pHorse = &ModelManager::Get().LoadModel("Horse", "assets/KnightHorse/KnightHorse.fbx");
		std::vector<OpaqueInstances::Material> horseTextures =
		{
			{&TextureManager::Get().LoadTexture2D("Armor", "assets/KnightHorse/dds/Armor_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Horse", "assets/KnightHorse/dds/Horse_BaseColor.dds")},
			{&TextureManager::Get().LoadTexture2D("Tail", "assets/KnightHorse/dds/Tail_BaseColor.dds")},
		};

		OpaqueInstances::Instance transform;

		DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(DirectX::XMVectorReplicate(10.0f), 
			DirectX::XMVectorZero(), DirectX::XMQuaternionIdentity(), DirectX::XMVectorSet(0.0f, -5.0f, 0.0f, 0.0f));
		mat = DirectX::XMMatrixTranspose(mat);
		LoadMatrixInArray(mat, transform.matrix);
		MeshSystem::Get().GetOpaqueInstances().AddInstance(pCube, cubeWallTexture, transform);

		mat = DirectX::XMMatrixTranslation(0.0f, 0.5f, -4.0f);
		mat = DirectX::XMMatrixTranspose(mat);
		LoadMatrixInArray(mat, transform.matrix);
		MeshSystem::Get().GetOpaqueInstances().AddInstance(pCube, cubeContainerTexture, transform);

		for (uint32_t i = 0; i < 3; ++i)
		{
			mat = DirectX::XMMatrixTranslation(-4.0f + i * 3.0f, 0.0f, 0.0f);
			mat = DirectX::XMMatrixTranspose(mat);
			LoadMatrixInArray(mat, transform.matrix);

			MeshSystem::Get().GetOpaqueInstances().AddInstance(pSamurai, samuraiTextures, transform);

			mat = DirectX::XMMatrixTranslation(-3.0f + i * 3.0f, 0.0f, 0.0f);
			mat = DirectX::XMMatrixTranspose(mat);
			LoadMatrixInArray(mat, transform.matrix);

			MeshSystem::Get().GetOpaqueInstances().AddInstance(pHorse, horseTextures, transform);
		}

		m_Renderer->GetSky().SetSky("skybox", "shaders/sky.hlsl", "assets/cubemap.dds");
		
		//init camera
		m_CameraController = std::unique_ptr<CameraController>(new CameraController(FOV, (float)width/(float)height, ZNEAR, ZFAR));
		m_CameraController->GetCamera().SetWorldOffset({0.f, 1.f, -10.0f});
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

		dispatcher.Dispatch<WindowResizeEvent>([](WindowResizeEvent& e)
			{
				uint32_t width = e.GetWidth() > 0 ? e.GetWidth() : 8;
				uint32_t height = e.GetHeight() > 0 ? e.GetHeight() : 8;

				Globals::Get().CreateDepthBuffer(width, height);
				
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

		MeshSystem::Get().Update();
	}
	
	void Application::Render()
	{
		const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

		m_Window->ClearColor(color);
		//m_Window->BindRenderTarget();
		Globals::Get().Bind(m_Window->GetRenderTarget());

		m_Renderer->Render(*m_Window, m_CameraController->GetCamera());
		m_Window->Flush();
	}
}