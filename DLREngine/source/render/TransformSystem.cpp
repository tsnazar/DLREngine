#include "TransformSystem.h"

namespace engine
{
	TransformSystem* TransformSystem::s_Instance = nullptr;

	void TransformSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new TransformSystem;
	}

	void TransformSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);

		delete s_Instance;

		s_Instance = nullptr;
	}
}