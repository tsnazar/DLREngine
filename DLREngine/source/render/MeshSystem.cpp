#include "MeshSystem.h"

namespace engine
{
	MeshSystem* MeshSystem::s_Instance = nullptr;

	void MeshSystem::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);

		s_Instance = new MeshSystem;
	}

	void MeshSystem::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);

		delete s_Instance;

		s_Instance = nullptr;
	}

	void MeshSystem::Render()
	{
		m_OpaqueInstances.Render();
	}

	void MeshSystem::Update()
	{
		m_OpaqueInstances.UpdateInstanceBuffers();
	}
}