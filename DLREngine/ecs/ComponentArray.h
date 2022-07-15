#pragma once
#include "Types.h"
#include "Debug.h"
#include <array>
#include <unordered_map>

namespace engine
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void InsertData(Entity entity, T component)
		{
			ALWAYS_ASSERT(m_EntityToIndexMap.find(entity) == m_EntityToIndex.end());

			uint32_t newIndex = m_Size;
			m_EntityToIndex[entity] = newIndex;
			m_IndexToEntity[newIndex] = entity;
			m_ComponentArray[newIndex] = component;
			++m_Size;
		}

		void RemoveData(Entity entity)
		{
			ALWAYS_ASSERT(m_EntityToIndex.find(entity) != m_EntityToIndex.end());

			uint32_t indexOfRemovedEntity = m_EntityToIndex[entity];
			uint32_t indexOfLastElement = m_Size - 1;
			m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

			Entity entityOfLastElement = m_IndexToEntity[indexOfLastElement];
			m_EntityToIndex[entityOfLastElement] = indexOfRemovedEntity;
			m_IndexToEntity[indexOfRemovedEntity] = entityOfLastElement;

			m_EntityToIndex.erase(entity);
			m_IndexToEntity.erase(indexOfLastElement);

			--m_Size;
		}

		T& GetData(Entity entity)
		{
			ALWAYS_ASSERT(m_EntityToIndex.find(entity) != m_EntityToIndex.end());

			return m_ComponentArray[m_EntityToIndex[entity]];
		}

		void EntityDestroyed(Entity entity) override
		{
			if (m_EntityToIndex[entity] != m_EntityToIndex.end())
				RemoveData(entity);
		}

	private:

		std::array<T, MAX_ENTITIES> m_ComponentArray;

		std::unordered_map<Entity, uint32_t> m_EntityToIndex;

		std::unordered_map<uint32_t, Entity> m_IndexToEntity;

		uint32_t m_Size;
	};

}