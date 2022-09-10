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

	void MeshSystem::RenderToShadowMap()
	{
		m_OpaqueInstances.RenderToShadowMap();
	}

	void MeshSystem::Update()
	{
		m_OpaqueInstances.UpdateInstanceBuffers();
		m_LightInstances.UpdateInstanceBuffers();
	}

	bool MeshSystem::PickMesh(const Ray& ray, Query& query)
	{
		MeshIntersection intersection;
		intersection.reset(0.0f);

		bool result = false;
		for (auto& perModel : m_OpaqueInstances.m_PerModel)
		{
			auto& model = *perModel.model;
			for (auto& perMesh : perModel.perMesh)
			{
				for (auto& perMaterial : perModel.perMesh)
				{
					for (auto& material : perMaterial.perMaterial)
					{
						for (auto& instance : material.instanceIDs)
						{
							DirectX::XMFLOAT4X4 mat;
							DirectX::XMStoreFloat4x4(&mat, TransformSystem::Get().GetTransforms()[instance].GetMatrix());
							if (model.Intersect(ray, intersection, mat))
							{
								result = true;
								query.transform = instance;
								query.t = intersection.t;
								DirectX::XMStoreFloat3(&query.pos, ray.PointAtLine(intersection.t));
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
				DirectX::XMFLOAT4X4 mat;
				DirectX::XMStoreFloat4x4(&mat, TransformSystem::Get().GetTransforms()[instanceRef.transformId].GetMatrix());
				if (model.Intersect(ray, intersection, mat))
				{
					result = true;
					query.transform = instanceRef.transformId;
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