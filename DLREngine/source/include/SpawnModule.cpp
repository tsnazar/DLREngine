#include "SpawnModule.h"
#include <algorithm>
#include "Globals.h"

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
			MeshSystem::Get().GetOpaqueInstances().AddInstance(iter->pModel, *reinterpret_cast<std::vector<OpaqueInstances::Material>*>((&(iter->materials))), iter->transformId, Globals::Get().GetObjectIDCounter());
			MeshSystem::Get().GetDissolutionInstances().DeleteInstance(iter->pModel, iter->materials, iter->transformId);
		}

		m_Instances.erase(ptr, m_Instances.end());

	}

	void SpawnModule::SpawnInstance(Model* pModel, const std::vector<DissolutionInstances::Material>& materials, uint32_t transformId, float animationTime)
	{
		Instance instance( pModel, materials, transformId, animationTime );

		m_Instances.push_back(instance);

		MeshSystem::Get().GetDissolutionInstances().AddInstance(pModel, materials, transformId, animationTime);
	}
}

