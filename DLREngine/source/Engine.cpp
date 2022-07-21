#include "Engine.h"
#include "Globals.h"
#include "ShaderManager.h"
#include"TextureManager.h"

namespace engine
{
	void Engine::Init()
	{
		Globals::Init();
		ShaderManager::Init();
		TextureManager::Init();
	}
	void Engine::Fini()
	{
		TextureManager::Fini();
		ShaderManager::Fini();
		Globals::Fini();
	}
}