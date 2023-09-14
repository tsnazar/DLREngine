#include "MeshSystem.h"
#include "LightSystem.h"
#include "Application.h"
#include "Query.h"
#include "Sky.h"

namespace engine
{
	MeshSystem* MeshSystem::s_Instance = nullptr;

	void MeshSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new MeshSystem;
	}

	void MeshSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);

		delete s_Instance;

		s_Instance = nullptr;
	}

	void MeshSystem::Render(Sky::IblResources iblResources)
	{
		m_OpaqueInstances.Render(iblResources);
		m_LightInstances.Render();
	}

	void MeshSystem::RenderToGBuffer()
	{
		m_OpaqueInstances.RenderToGBuffer();
		m_DissolutionInstances.RenderToGBuffer();
	}

	void MeshSystem::ResolveGBuffer(Sky::IblResources iblResources, Texture2D& depth, Texture2D& albedo, Texture2D& normals,
		Texture2D& roughnessMetallic, Texture2D& emission, ConstantBuffer& dimensions)
	{
		m_OpaqueInstances.ResolveGBuffer(iblResources, depth, albedo, normals, roughnessMetallic, emission, dimensions);
	}

	void MeshSystem::RenderToShadowMap(ConstantBuffer& shadowMatrixBuffer, std::vector<LightSystem::ShadowMapMatrices>& matrices, uint32_t numLights)
	{
		m_OpaqueInstances.RenderToShadowMap(shadowMatrixBuffer, matrices, numLights);
	}

	void MeshSystem::Update(float dt)
	{
		m_OpaqueInstances.UpdateInstanceBuffers();
		m_LightInstances.UpdateInstanceBuffers();
		m_DissolutionInstances.Update(dt);
		m_DissolutionInstances.UpdateInstanceBuffers();
	}

	bool MeshSystem::PickMesh(const Ray& ray, Query& query)
	{
		MeshIntersection intersection;
		intersection.reset(0.0f);

		auto& transforms = TransformSystem::Get().GetTransforms();

		bool result = false;
		for (auto& perModel : m_OpaqueInstances.m_PerModel)
		{
			auto& model = *perModel.model;
			for (auto& perMesh : perModel.perMesh)
			{
				for (uint32_t meshIndex = 0; meshIndex < perModel.perMesh.size(); ++meshIndex)
				{
					for (auto& material : perModel.perMesh[meshIndex].perMaterial)
					{
						for (auto& instance : material.instances)
						{
							auto& mesh = model.GetMeshes()[meshIndex];
							auto& transform = transforms[instance.transformID];
							if (mesh.Intersect(ray, intersection, transform.GetInvMatrix(), transform.GetMatrix()))
							{
								result = true;
								query.transformID = instance.transformID;
								query.meshID = instance.objectID;
								query.t = intersection.t;
								query.pos = intersection.pos;
								query.normal = intersection.normal;
								query.usable = true;
							}
						}
					}
				}
			}
		}

		for (auto& perModel : m_LightInstances.m_PerModel)
		{
			auto& model = *perModel.model;

			for (auto& instanceRef : perModel.instanceRefs)
			{
				if (model.Intersect(ray, intersection, transforms[instanceRef.transformId].GetInvMatrix()))
				{
					result = true;
					query.transformID = instanceRef.transformId;
					query.t = intersection.t;
					DirectX::XMStoreFloat3(&query.pos, ray.PointAtLine(intersection.t));
					query.usable = true;
					std::cout << intersection.triangle << " " << intersection.t << std::endl;
				}
			}
		}

		return result;
	}
}