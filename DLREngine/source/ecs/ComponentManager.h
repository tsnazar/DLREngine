#pragma once
#include "ComponentArray.h"

namespace engine
{
	class ComponentManager
	{
	public:

		template<typename T>
		void RegisterComponent()
		{
			const char* typeName = typeid(T).name();

			ALWAYS_ASSERT(m_ComponentTypes.find(typeName) == m_ComponentTypes.end());

			m_ComponentTypes.insert({ typeName, m_NextComponentType });
			m_ComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

			++m_NextComponentType;
		}

		template<typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();

			ALWAYS_ASSERT(m_ComponentTypes.find(typeName) != m_ComponentTypes.end());

			return m_ComponentTypes[typeName];
		}

		template<typename T>
		void AddComponent(Entity entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template<typename T>
		void RemoveComponent(Entity entity)
		{
			GetComponentArray<T>()->RemoveData(entity);
		}

		template<typename T>
		T& GetComponent(Entity entity)
		{
			return GetComponentArray<T>()->GetData(entity);
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : m_ComponentArrays)
			{
				auto const& component = pair.second;

				component->EntityDestroyed(entity);
			}
		}

	private:
		template<typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray()
		{
			const char* typeName = typeid(T).name();

			ALWAYS_ASSERT(m_ComponentTypes.find(typeName) != m_ComponentTypes.end());

			return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
		}

	private:
		std::unordered_map<const char*, ComponentType> m_ComponentTypes;

		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays;

		ComponentType m_NextComponentType;
	};
}