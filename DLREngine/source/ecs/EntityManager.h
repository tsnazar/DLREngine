#pragma once
#include <queue>
#include <array>
#include "Types.h"
#include "Debug.h"

namespace engine
{


	class EntityManager
	{
	public:
		EntityManager()
		{
			for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
				m_AvailableEntitites.push(entity);
		}

		Entity CreateEntity()
		{
			ALWAYS_ASSERT(m_LivingEntitiesCount < MAX_ENTITIES);

			Entity id = m_AvailableEntitites.front();
			m_AvailableEntitites.pop();
			++m_LivingEntitiesCount;
			return id;
		}

		void DestroyEntity(Entity entity)
		{
			ALWAYS_ASSERT(entity < MAX_ENTITIES);

			m_Signatures[entity].reset();

			m_AvailableEntitites.push(entity);
			--m_LivingEntitiesCount;
		}

		void SetSignature(Entity entity, Signature signature)
		{
			ALWAYS_ASSERT(entity < MAX_ENTITIES);

			m_Signatures[entity] = signature;
		}

		Signature GetSignature(Entity entity)
		{
			ALWAYS_ASSERT(entity < MAX_ENTITIES);

			return m_Signatures[entity];
		}

	private:
		std::queue<Entity> m_AvailableEntitites;

		std::array<Signature, MAX_ENTITIES> m_Signatures;

		uint32_t m_LivingEntitiesCount;
	};

}