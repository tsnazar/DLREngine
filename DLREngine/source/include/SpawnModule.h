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
			enum Type { Spawn, Despawn };

			Model* pModel;
			std::vector<DissolutionInstances::Material> materials;
			uint32_t transformID;
			float lifeTime;
			Type type;

			Instance(Model* pModel, std::vector<DissolutionInstances::Material> materials, uint32_t transformID, float lifeTime, Type type)
				: pModel(pModel), materials(materials), transformID(transformID), lifeTime(lifeTime), type(type) {};
		};

	public:

		void Update(float dt);

		void SpawnInstance(Model* pModel, const std::vector<DissolutionInstances::Material>& materials, uint32_t transformId, float animationTime);
		
		void DespawnInstance(Model* pModel,  uint32_t transformId, uint32_t objectID, DirectX::XMFLOAT3 spherePos);
		

	private:
		std::vector<Instance> m_Instances;
	};
}