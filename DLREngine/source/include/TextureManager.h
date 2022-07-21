#pragma once
#include "Texture2D.h"
#include <unordered_map>

namespace engine
{
	class TextureManager
	{
	public:
		static void Init();

		static void Fini();

		inline static TextureManager& Get() { return *s_Instance; }

		Texture2D& LoadTexture2D(const std::string& name, const std::string& filepath);
		Texture2D& LoadCubemap(const std::string& name, const std::string& filepath);

		bool TextureExists(const std::string& name);

		Texture2D& GetTexture(const std::string name);
	private:
		using ContainerTextures = std::unordered_map<std::string, std::unique_ptr<Texture2D>>;

		ContainerTextures m_Textures;
	protected:
		TextureManager();

		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;
		TextureManager(TextureManager&&) = delete;
		TextureManager& operator=(TextureManager&&) = delete;

		static TextureManager* s_Instance;
	};
}

