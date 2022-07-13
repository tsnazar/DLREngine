#pragma once
#include <unordered_map>
#include "Texture2D.h"
#include "Shader.h"
#include "InputLayout.h"

namespace engine
{
	class ResourceManager
	{
	public:
		ResourceManager();
		~ResourceManager();

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

		inline static ResourceManager& Get() { return *s_Instance; }

		void LoadTexture2D(const std::string& name, const std::string& filepath);
		void LoadCubemap(const std::string& name, const std::string& filepath);
		void LoadShader(const std::string& name, VertexType type, const std::string& filepath);
		void LoadInputLayout(VertexType type, ID3D10Blob* blob);

		bool InputLayoutExists(VertexType type);
		bool TextureExists(const std::string& name);
		bool ShaderExists(const std::string& name);

		InputLayout& GetInputLayout(VertexType type);
		Shader& GetShader(const std::string& name);
		Texture2D& GetTexture(const std::string name);

	private:
		using ContainerTextures = std::unordered_map<std::string, Texture2D>;
		using ContainerShaders = std::unordered_map<std::string, Shader>;
		using ContainerInputLayout = std::unordered_map<VertexType, InputLayout>;

		ContainerTextures m_Textures;
		ContainerShaders m_Shaders;
		ContainerInputLayout m_InputLayouts;

	private:
		static ResourceManager* s_Instance;
	};
}
