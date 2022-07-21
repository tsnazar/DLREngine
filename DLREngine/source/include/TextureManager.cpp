#include "TextureManager.h"
#include "Debug.h"

namespace engine
{
	TextureManager* TextureManager::s_Instance = nullptr;

	TextureManager::TextureManager()
	{

	}

	void TextureManager::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);
		s_Instance = new TextureManager();
	}

	void TextureManager::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
	}

	Texture2D& TextureManager::LoadTexture2D(const std::string& name, const std::string& filepath)
	{
		ContainerTextures::iterator iter = m_Textures.find(name);

		if (iter != m_Textures.end()) // texture with this name already exists
			return *(iter->second);

		std::unique_ptr<Texture2D>& texture = m_Textures[name];
		texture = std::make_unique<Texture2D>();

		return texture->LoadFromFile(filepath);
	}

	Texture2D& TextureManager::LoadCubemap(const std::string& name, const std::string& filepath)
	{
		ContainerTextures::iterator iter = m_Textures.find(name);

		if (iter != m_Textures.end()) // texture with this name already exists
			return *(iter->second);

		std::unique_ptr<Texture2D>& texture = m_Textures[name];
		texture = std::make_unique<Texture2D>();

		return texture->LoadFromFile(filepath, D3D11_RESOURCE_MISC_TEXTURECUBE);
	}
	
	bool TextureManager::TextureExists(const std::string& name)
	{
		if (m_Textures.find(name) != m_Textures.end())
			return true;
		return false;
	}
	
	Texture2D& TextureManager::GetTexture(const std::string name)
	{
		ContainerTextures::iterator iter = m_Textures.find(name);

		if (iter == m_Textures.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return *(iter->second);
	}
}