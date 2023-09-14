#pragma once
#include "OpaqueInstances.h"
#include "LightInstances.h"
#include "DissolutionInstances.h"

namespace engine
{
	struct Query;

	class MeshSystem
	{
	public:
		static void Init();

		static void Fini();

		void Render(Sky::IblResources iblResources);

		void RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights);

		void Update(float dt);

		bool PickMesh(const Ray& ray, Query& query);

		static MeshSystem& Get() { return *s_Instance; }

		OpaqueInstances& GetOpaqueInstances() { return m_OpaqueInstances; }

		LightInstances& GetLightInstances() { return m_LightInstances; }

		DissolutionInstances& GetDissolutionInstances() { return m_DissolutionInstances; }

	private:
		OpaqueInstances m_OpaqueInstances;
		LightInstances m_LightInstances;
		DissolutionInstances m_DissolutionInstances;
	
	private:
		static MeshSystem* s_Instance;
	};
}