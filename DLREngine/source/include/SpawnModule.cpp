#include "SpawnModule.h"
#include <algorithm>
#include "Globals.h"
#include "DecalSystem.h"
#include "ParticleSystem.h"

namespace engine
{
	void SpawnModule::Update(float dt)
	{
		for (auto& instance : m_Instances)
		{
			instance.lifeTime -= dt;
		}

		std::sort(m_Instances.begin(), m_Instances.end(), [](Instance a, Instance b) {
			return a.lifeTime > b.lifeTime;
			});

		auto ptr = std::find_if(m_Instances.begin(), m_Instances.end(), [&](Instance p) { return p.lifeTime < 0.0f; });

		for (auto iter = ptr; iter < m_Instances.end(); ++iter)
		{
			if (iter->type == Instance::Type::Spawn)
			{
				uint32_t objectID = Globals::Get().GetObjectIDCounter()++;
				MeshSystem::Get().GetOpaqueInstances().AddInstance(iter->pModel, *reinterpret_cast<std::vector<OpaqueInstances::Material>*>((&(iter->materials))), iter->transformID, objectID);
				MeshSystem::Get().GetDissolutionInstances().DeleteInstance(iter->pModel, iter->materials, iter->transformID);
			}
			else if (iter->type == Instance::Type::Despawn) 
			{
				DecalSystem::Get().DespawnDecal(iter->transformID);
				MeshSystem::Get().GetDisintegrationInstances().DeleteInstance(iter->pModel, *reinterpret_cast<std::vector<DisintegrationInstances::Material>*>((&(iter->materials))), iter->transformID);
				TransformSystem::Get().GetTransforms().erase(iter->transformID);
			}
		}

		m_Instances.erase(ptr, m_Instances.end());

	}

	void SpawnModule::SpawnInstance(Model* pModel, const std::vector<DissolutionInstances::Material>& materials, uint32_t transformID, float animationTime)
	{
		Instance instance( pModel, materials, transformID, animationTime, Instance::Type::Spawn );

		m_Instances.push_back(instance);

		MeshSystem::Get().GetDissolutionInstances().AddInstance(pModel, materials, transformID, animationTime);
	}

	void SpawnModule::DespawnInstance(Model* pModel, uint32_t transformID, uint32_t objectID, DirectX::XMFLOAT3 spherePos)
	{
		std::vector<DissolutionInstances::Material> materials;

		MeshSystem::Get().GetOpaqueInstances().DeleteInstance(pModel, transformID, *reinterpret_cast<std::vector<OpaqueInstances::Material>*>((&(materials))));

		Instance instance(pModel, materials, transformID, ParticleSystem::GPU_LIFETIME, Instance::Type::Despawn);
		m_Instances.push_back(instance);

		MeshSystem::Get().GetDisintegrationInstances().AddInstance(pModel, *reinterpret_cast<std::vector<DisintegrationInstances::Material>*>((&(materials))), transformID, objectID, spherePos);
	}
}

