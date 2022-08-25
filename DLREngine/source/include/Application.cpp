#include "Application.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "KeyEvents.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "MathUtils.h"
#include "Ray.h"
#include "MeshSystem.h"

namespace
{
	const float FOV = 1.0472f; // 60 degrees
	const float ZNEAR = 100.f;
	const float ZFAR = 0.1f;
	const float FRAME_DURATION = 1.f / 60.f;
	const float EXPOSURE_DELTA = 1.0f;
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
		m_Renderer->CreateHDRTexture(width, height);

		std::vector<D3D11_INPUT_ELEMENT_DESC> opaque = {
			D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTanBitan, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTexNorTanBitan, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTanBitan, nor), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTanBitan, tan), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTanBitan, bitan), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[0]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[1]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[2]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(OpaqueInstances::Instance, matrix[3]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> emissive = {
			D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(LightInstances::Instance, color), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(LightInstances::Instance, matrix[0]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(LightInstances::Instance, matrix[1]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(LightInstances::Instance, matrix[2]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			D3D11_INPUT_ELEMENT_DESC{ "MAT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(LightInstances::Instance, matrix[3]), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};

		ShaderManager::Get().LoadShader("instanceBRDF", "shaders/opaque.hlsl", &opaque);
		ShaderManager::Get().LoadShader("lightInstance", "shaders/emissive.hlsl", &emissive);
		ShaderManager::Get().LoadShader("skybox", "shaders/sky.hlsl", nullptr);
		ShaderManager::Get().LoadShader("resolve", "shaders/resolve.hlsl", nullptr);

		Model* pSphere = &ModelManager::Get().GetUnitSphere();
		Model* pCube = &ModelManager::Get().GetUnitCube();

		std::vector<OpaqueInstances::Material> cubeContainerTexture = { OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("BambooWall", "assets/bamboo/BambooWall_albedo.dds"), 
																									&TextureManager::Get().LoadTexture2D("BambooWallRoughness", "assets/bamboo/BambooWall_roughness.dds"), 
																						nullptr, &TextureManager::Get().LoadTexture2D("BambooWallNormal", "assets/bamboo/BambooWall_normal.dds"),  0.9f, 0.0f, true) };
		std::vector<OpaqueInstances::Material> cubeWallTexture = { OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("TilesGlass", "assets/tiles/TilesGlass4_albedo.dds"), 
																							&TextureManager::Get().LoadTexture2D("TilesGlassRoughness", "assets/tiles/TilesGlass4_roughness.dds"),
																					nullptr, &TextureManager::Get().LoadTexture2D("TilesGlassNormals", "assets/tiles/TilesGlass4_normal.dds"),  0.9f, 0.0f, true) };

		Model* pSamurai = &ModelManager::Get().LoadModel("Samurai", "assets/Samurai/Samurai.fbx");
		std::vector<OpaqueInstances::Material> samuraiTextures =
		{
			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Sword", "assets/Samurai/dds/Sword_BaseColor.dds"),
										&TextureManager::Get().LoadTexture2D("SwordRoughness", "assets/Samurai/dds/Sword_Roughness.dds"),
										&TextureManager::Get().LoadTexture2D("SwordMetallic", "assets/Samurai/dds/Sword_Metallic.dds"),
										&TextureManager::Get().LoadTexture2D("SwordNormal", "assets/Samurai/dds/Sword_Normal.dds"),  0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Head", "assets/Samurai/dds/Head_BaseColor.dds"),
									  &TextureManager::Get().LoadTexture2D("HeadRoughness", "assets/Samurai/dds/Head_Roughness.dds"), nullptr,
									  &TextureManager::Get().LoadTexture2D("HeadNormal", "assets/Samurai/dds/Head_Normal.dds"),  0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Eye", "assets/Samurai/dds/Eye_BaseColor.dds"), nullptr,nullptr, nullptr,  0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Helmet", "assets/Samurai/dds/Helmet_BaseColor.dds"),
										&TextureManager::Get().LoadTexture2D("HelmetRoughness", "assets/Samurai/dds/Helmet_Roughness.dds"),
										&TextureManager::Get().LoadTexture2D("HelmetMetallic", "assets/Samurai/dds/Helmet_Metallic.dds"),
										&TextureManager::Get().LoadTexture2D("HelmetNormal", "assets/Samurai/dds/Helmet_Normal.dds"),  0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Torso", "assets/Samurai/dds/Torso_BaseColor.dds"),
										&TextureManager::Get().LoadTexture2D("TorsoRoughness", "assets/Samurai/dds/Torso_Roughness.dds"),
										&TextureManager::Get().LoadTexture2D("TorsoMetallic", "assets/Samurai/dds/Torso_Metallic.dds"),
										&TextureManager::Get().LoadTexture2D("TorsoNormal", "assets/Samurai/dds/Torso_Normal.dds"),  0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Legs", "assets/Samurai/dds/Legs_BaseColor.dds"),
										&TextureManager::Get().LoadTexture2D("LegsRoughness", "assets/Samurai/dds/Legs_Roughness.dds"),
										&TextureManager::Get().LoadTexture2D("LegsMetallic", "assets/Samurai/dds/Legs_Metallic.dds"),
										&TextureManager::Get().LoadTexture2D("LegsNormal", "assets/Samurai/dds/Legs_Normal.dds"), 0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Hand", "assets/Samurai/dds/Hand_BaseColor.dds"),
										&TextureManager::Get().LoadTexture2D("HandRoughness", "assets/Samurai/dds/Hand_Roughness.dds"), nullptr,
										&TextureManager::Get().LoadTexture2D("HandNormal", "assets/Samurai/dds/Hand_Normal.dds"),  0.9f, 0.0f),

			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Torso", "assets/Samurai/dds/Torso_BaseColor.dds"),
										&TextureManager::Get().LoadTexture2D("TorsoRoughness", "assets/Samurai/dds/Torso_Roughness.dds"),
										&TextureManager::Get().LoadTexture2D("TorsoMetallic", "assets/Samurai/dds/Torso_Metallic.dds"),
										&TextureManager::Get().LoadTexture2D("TorsoNormal", "assets/Samurai/dds/Torso_Normal.dds"), 0.9f, 0.0f),
		};

		Model* pHorse = &ModelManager::Get().LoadModel("Horse", "assets/KnightHorse/KnightHorse.fbx");
		std::vector<OpaqueInstances::Material> horseTextures =
		{
			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Armor", "assets/KnightHorse/dds/Armor_BaseColor.dds"),
									  &TextureManager::Get().LoadTexture2D("ArmorRoughness", "assets/KnightHorse/dds/Armor_Roughness.dds"),
									  &TextureManager::Get().LoadTexture2D("ArmorMetallic", "assets/KnightHorse/dds/Armor_Metallic.dds"),
									  &TextureManager::Get().LoadTexture2D("ArmorNormal", "assets/KnightHorse/dds/Armor_Normal.dds"), 0.9f, 0.0f),
			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Horse", "assets/KnightHorse/dds/Horse_BaseColor.dds"),
									  &TextureManager::Get().LoadTexture2D("HorseRoughness", "assets/KnightHorse/dds/Horse_Roughness.dds"), nullptr,
									  &TextureManager::Get().LoadTexture2D("HorseNormal", "assets/KnightHorse/dds/Horse_Normal.dds"), 0.9f, 0.0f),
			OpaqueInstances::Material(&TextureManager::Get().LoadTexture2D("Tail", "assets/KnightHorse/dds/Tail_BaseColor.dds"), nullptr, nullptr,
									  &TextureManager::Get().LoadTexture2D("TailNormal", "assets/KnightHorse/dds/Tail_Normal.dds"),  0.9f, 0.0f),
		};

		TransformSystem::Transform transform;

		transform.position = { 0.f, 0.5f, -4.f };
		transform.scale = { 1.f, 1.f, 1.f };
		transform.rotation = { 0.f, 0.f, 0.f };
		MeshSystem::Get().GetOpaqueInstances().AddInstance(pCube, cubeContainerTexture, transform);

		transform.position = { 0.f, -5.f, 0.f };
		transform.scale = { 10.f, 10.f, 10.f };
		transform.rotation = { 0.f, 0.f, 0.f };
		MeshSystem::Get().GetOpaqueInstances().AddInstance(pCube, cubeWallTexture, transform);

		for (uint32_t i = 0; i < 3; ++i)
		{
			transform.position = { -4.0f + i * 3.0f, 0.0f, 0.0f };
			transform.scale = { 1.f, 1.f, 1.f };
			transform.rotation = { 0.f, 0.f, 0.f };
			MeshSystem::Get().GetOpaqueInstances().AddInstance(pSamurai, samuraiTextures, transform);

			transform.position = { -3.0f + i * 3.0f, 0.0f, 0.0f };
			transform.scale = { 1.f, 1.f, 1.f };
			transform.rotation = { 0.f, 0.f, 0.f };
			MeshSystem::Get().GetOpaqueInstances().AddInstance(pHorse, horseTextures, transform);
		}

		m_Renderer->GetSky().SetSky("skybox", "shaders/sky.hlsl", "assets/night_street.dds");

		{
			LightSystem::PointLight light({ -2.0f, 5.0f, -3.0f }, { 1.0f, 1.0f, 1.0f }, 0.15f, 8.0f);
			LightSystem::Get().AddPointLight(light);
		}

		{
			LightSystem::PointLight light({ 2.0f, 5.0f, -3.0f }, { 1.0f, 1.0f, 1.0f }, 0.15f, 8.0f);
			LightSystem::Get().AddPointLight(light);
		}
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

		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent& e)
			{
				uint32_t width = e.GetWidth() > 0 ? e.GetWidth() : 8;
				uint32_t height = e.GetHeight() > 0 ? e.GetHeight() : 8;

				Globals::Get().CreateDepthBuffer(width, height);
				m_Renderer->CreateHDRTexture(width, height);
				
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

		DirectX::XMStoreFloat4(&p.cameraPos, camera.Position());

		XMVECTOR TL, TR, BL, xDir, yDir;
		TL = camera.Unproject(XMVectorSet(-1.0f, 1.0f, 1.0f, 1.0f)) - camera.Position();
		TR = camera.Unproject(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f)) - camera.Position();
		BL = camera.Unproject(XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f)) - camera.Position();
		xDir = TR - TL;
		yDir = BL - TL;
		XMStoreFloat4(&(p.frustumCorners[0]), TL);
		XMStoreFloat4(&(p.frustumCorners[1]), xDir);
		XMStoreFloat4(&(p.frustumCorners[2]), yDir);

		float EV100 = m_Renderer->GetEV100();

		if (m_Window->IsKeyPressed(Key::OEM_PLUS))
			EV100 -= delta * EXPOSURE_DELTA;
		if (m_Window->IsKeyPressed(Key::OEM_MINUS))
			EV100 += delta * EXPOSURE_DELTA;
		
		if (m_Window->IsKeyPressed(Key::RMB))
		{
			Ray ray = m_CameraController->GetPickingRay();

			if (m_FirstRMB)
			{
				m_FirstRMB = false;

				m_Query.reset();

				MeshSystem::Get().PickMesh(ray, m_Query);
			}

			if (m_Query.usable)
			{
				DirectX::XMFLOAT3 off;
				DirectX::XMVECTOR pos = ray.PointAtLine(m_Query.t);
				DirectX::XMStoreFloat3(&off, DirectX::XMVectorSubtract(pos, DirectX::XMLoadFloat3(&m_Query.pos)));
				auto& transform = TransformSystem::Get().GetTransforms()[m_Query.transform];
				transform.position.x += off.x;
				transform.position.y += off.y;
				transform.position.z += off.z;
				DirectX::XMStoreFloat3(&m_Query.pos, pos);
			}
		}
		else {
			m_FirstRMB = true;
		}

		m_Renderer->SetEV100(EV100);

		MeshSystem::Get().Update();

		m_Renderer->Update();
	}
	
	void Application::Render()
	{
		const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

		m_Renderer->Render(*m_Window, m_CameraController->GetCamera());
		m_Window->Flush();
	}
}