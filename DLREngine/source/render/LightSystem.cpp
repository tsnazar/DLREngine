#include "LightSystem.h"
#include "Globals.h"
#include "MeshSystem.h"
#include "ModelManager.h"

namespace engine
{
	LightSystem* LightSystem::s_Instance = nullptr;

	void LightSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);
		s_Instance = new LightSystem;
	}

	void LightSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
	}

	void LightSystem::AddPointLight(const PointLight& light)
	{
		TransformSystem::Transform transform;
		transform.position = light.position;
		transform.rotation = { 0,0,0 };
		transform.scale = { light.radius / 2.0f, light.radius / 2.0f, light.radius / 2.0f };

		auto& transforms = TransformSystem::Get().GetTransforms();
		uint32_t id = transforms.insert(transform);

		PointLightRef ref;
		ref.transformId = id;
		ref.radiance = light.radiance;
		ref.radius = light.radius;

		ALWAYS_ASSERT(m_NumLights < MAX_POINT_LIGHTS);
		m_PointLightRefs[m_NumLights++] = ref;

		MeshSystem::Get().GetLightInstances().AddInstance(&ModelManager::Get().GetModel("Sphere"), light.radiance, id);
	}

	void LightSystem::Bind()
	{
		auto& perFrame = Globals::Get().GetPerFrameObj();
		auto& transforms = TransformSystem::Get().GetTransforms();
		
		for (uint32_t i = 0; i < m_NumLights; ++i)
		{
			PointLight light; 
			light.position = transforms[m_PointLightRefs[i].transformId].position;
			light.radiance = m_PointLightRefs[i].radiance;
			light.radius = m_PointLightRefs[i].radius;

			perFrame.pointLights[i] = light;
		}
	}


}