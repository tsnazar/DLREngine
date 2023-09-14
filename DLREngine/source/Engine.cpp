#include "Engine.h"
#include "Globals.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "MeshSystem.h"
#include "LightSystem.h"
#include "TransformSystem.h"
#include "Postprocess.h"
#include "ParticleSystem.h"
#include "VegetationSystem.h"

namespace engine
{
	void Engine::Init()
	{
		Globals::Init();
		ShaderManager::Init();
		TextureManager::Init();
		ModelManager::Init();
		TransformSystem::Init();
		MeshSystem::Init();
		LightSystem::Init();
		Postprocess::Init();
		ParticleSystem::Init();
		VegetationSystem::Init();
	}
	void Engine::Fini()
	{
		VegetationSystem::Fini();
		ParticleSystem::Fini();
		Postprocess::Fini();
		LightSystem::Fini();
		MeshSystem::Fini();
		TransformSystem::Fini();
		ModelManager::Fini();
		TextureManager::Fini();
		ShaderManager::Fini();
		Globals::Fini();
	}
}