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

namespace engine
{
	void Engine::Init()
	{
		Globals::Init();
		ShaderManager::Init();
		TextureManager::Init();
		ModelManager::Init();
		MeshSystem::Init();
		LightSystem::Init();
		TransformSystem::Init();
		Postprocess::Init();
		ParticleSystem::Init();
	}
	void Engine::Fini()
	{
		ParticleSystem::Fini();
		Postprocess::Fini();
		TransformSystem::Fini();
		LightSystem::Fini();
		MeshSystem::Fini();
		ModelManager::Fini();
		TextureManager::Fini();
		ShaderManager::Fini();
		Globals::Fini();
	}
}