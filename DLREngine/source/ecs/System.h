#pragma once
#include "Types.h"
#include <set>

namespace engine
{
	class Coordinator;

	class System
	{
	public:
		std::set<Entity> m_Entities;
		Coordinator* m_Coordinator;
	};
}