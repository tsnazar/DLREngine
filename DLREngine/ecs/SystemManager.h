#pragma once
#include "Types.h"
#include "System.h"
#include "Debug.h"
#include <unordered_map>

namespace engine
{
	class SystemManager
	{
	public:
		template<typename T> 
		std::shared_ptr<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();

			ALWAYS_ASSERT(m_Systems.find(typeName) == m_Systems.end());

			auto system = std::make_shared<T>();
			m_Systems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();

			ALWAYS_ASSERT(m_Systems.find(typeName) != m_Systems.end());

			m_Signatures.insert({ typeName, signature });
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : m_Systems)
			{
				auto const& system = pair.second;

				system->m_Entities.erase(entity);
			}
		}

		void EntitySignatureChanged(Entity entity, Signature entitySignature)
		{
			for (auto const& pair : m_Systems)
			{
				auto const& type = pair.first;
				auto const& system = pair.second;
				auto const& systemSignature = m_Signatures[type];

				if ((entitySignature & systemSignature) == systemSignature)
					system->m_Entities.insert(entity);
				else
					system->m_Entities.erase(entity);
			}
		}

	private:
		std::unordered_map<const char*, Signature> m_Signatures;
		std::unordered_map<const char*, std::shared_ptr<System>> m_Systems;
	};
}