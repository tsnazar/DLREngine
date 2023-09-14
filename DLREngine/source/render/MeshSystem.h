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

		void RenderToGBuffer();

		void ResolveGBuffer(Sky::IblResources iblResources, Texture2D& depth, Texture2D& albedo, Texture2D& normals,
			Texture2D& roughnessMetallic, Texture2D& emission, ConstantBuffer& dimensions);

		void RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights);

		void Update(float dt);

		bool PickMesh(const Ray& ray, Query& query);

		static MeshSystem& Get() { return *s_Instance; }

		OpaqueInstances& GetOpaqueInstances() { return m_OpaqueInstances; }

		LightInstances& GetLightInstances() { return m_LightInstances; }

		DissolutionInstances& GetDissolutionInstances() { return m_DissolutionInstances; }

		//uint32_t& GetMeshID() { return m_MeshIDCounter; }

	private:
		OpaqueInstances m_OpaqueInstances;
		LightInstances m_LightInstances;
		DissolutionInstances m_DissolutionInstances;
		
		//uint32_t m_MeshIDCounter = 1; // start with 1, 0 indicates no mesh

	private:
		static MeshSystem* s_Instance;
	};
}