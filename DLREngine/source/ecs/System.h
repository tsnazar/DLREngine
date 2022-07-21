#pragma once
#include "Types.h"
#include <set>

namespace engine
{
	class System
	{
	public:
		std::set<Entity> m_Entities;
	};
}