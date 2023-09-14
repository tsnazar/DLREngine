#pragma once
#include "MeshSystem.h"
#include "ModelManager.h"
#include "TextureManager.h"

namespace engine
{
	class SpawnModule
	{
	public:
		
		struct Instance
		{
			Model* pModel;
			std::vector<DissolutionInstances::Material> materials;
			uint32_t transformId;
			float lifeTime;

			Instance(Model* pModel, std::vector<DissolutionInstances::Material> materials, uint32_t transformId, float lifeTime)
				: pModel(pModel), materials(materials), transformId(transformId), lifeTime(lifeTime) {};
		};

	public:

		void Update(float dt);

		void SpawnInstance(Model* pModel, const std::vector<DissolutionInstances::Material>& materials, uint32_t transformId, float animationTime);

	private:
		std::vector<Instance> m_Instances;
	};
}