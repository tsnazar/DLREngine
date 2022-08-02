#pragma once
#include "OpaqueInstances.h"

namespace engine
{
	class MeshSystem
	{
	public:
		static void Init();
		
		static void Fini();
		
		void Render();

		void Update();

		static MeshSystem& Get() { return *s_Instance; }
		
		OpaqueInstances& GetOpaqueInstances() { return m_OpaqueInstances; }

	private:
		OpaqueInstances m_OpaqueInstances;
	
	private:
		static MeshSystem* s_Instance;
	};
}