#pragma once
#include "OpaqueInstances.h"
#include "LightInstances.h"
#include "Application.h"

namespace engine
{
	struct Query;

	class MeshSystem
	{
	public:
		static void Init();
		
		static void Fini();
		
		void Render();

		void Update();

		bool PickMesh(const Ray& ray, Query& query);

		static MeshSystem& Get() { return *s_Instance; }
		
		OpaqueInstances& GetOpaqueInstances() { return m_OpaqueInstances; }
		
		LightInstances& GetLightInstances() { return m_LightInstances; }

	private:
		OpaqueInstances m_OpaqueInstances;
		LightInstances m_LightInstances;
	
	private:
		static MeshSystem* s_Instance;
	};
}