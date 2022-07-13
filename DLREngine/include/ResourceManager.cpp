#include "ResourceManager.h"

namespace engine
{
	ResourceManager* ResourceManager::s_Instance = nullptr;

	ResourceManager::ResourceManager()
	{
		if (s_Instance != nullptr)
			ALWAYS_ASSERT(false);

		s_Instance = this;
	}
	
	ResourceManager::~ResourceManager()
	{
		s_Instance = nullptr;
	}
	
	void ResourceManager::LoadTexture2D(const std::string& name, const std::string& filepath)
	{
		if (TextureExists(name)) // texture with this name already exists
			return;

		m_Textures[name].LoadFromFile(filepath);
	}
	
	void ResourceManager::LoadCubemap(const std::string& name, const std::string& filepath)
	{
		if (TextureExists(name)) // texture with this name already exists
			return;

		m_Textures[name].LoadFromFile(filepath, D3D11_RESOURCE_MISC_TEXTURECUBE);
	}

	void ResourceManager::LoadShader(const std::string& name, VertexType type, const std::string& filepath)
	{
		if (ShaderExists(name)) // shader with this name already exists
			return;

		m_Shaders[name].LoadFromFile(type, filepath);
	}

	void ResourceManager::LoadInputLayout(VertexType type, ID3D10Blob* blob)
	{
		if (InputLayoutExists(type)) // input  with this type already exists
			return;

		m_InputLayouts[type].Create(type, blob);
	}

	bool ResourceManager::InputLayoutExists(VertexType type)
	{
		if (m_InputLayouts.find(type) != m_InputLayouts.end())
			return true;
		return false;
	}

	bool ResourceManager::TextureExists(const std::string& name)
	{
		if (m_Textures.find(name) != m_Textures.end())
			return true;
		return false;
	}

	bool ResourceManager::ShaderExists(const std::string& name)
	{
		if (m_Shaders.find(name) != m_Shaders.end())
			return true;
		return false;
	}

	InputLayout& ResourceManager::GetInputLayout(VertexType type)
	{
		ContainerInputLayout::iterator iter = m_InputLayouts.find(type);

		if (iter == m_InputLayouts.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return iter->second;
	}

	Shader& ResourceManager::GetShader(const std::string& name)
	{
		ContainerShaders::iterator iter = m_Shaders.find(name);

		if (iter == m_Shaders.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return iter->second;
	}

	Texture2D& ResourceManager::GetTexture(const std::string name)
	{
		ContainerTextures::iterator iter = m_Textures.find(name);

		if (iter == m_Textures.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return iter->second;
	}
}