#pragma once
#include "ConstantBuffer.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Camera.h"

namespace engine
{
	class Sky
	{
	public:
		struct ShaderDescription
		{
			enum Bindings : uint32_t { CUBEMAP_TEXTURE = 0 };
		};

		struct IblResources
		{
			bool hasResources;
			Texture2D* irradiance, * reflection, * reflectance;
		};

	public:
		Sky();

		void SetSky(const std::string& name, const std::string& shaderPath, const std::string& texturePath, const char* irradiancePath = nullptr, const char* reflectionPath = nullptr, const char* reflectancePath = nullptr);
		
		void Render(Camera& camera);

		bool HasIBLTextures() { return m_hasIBLTextures; }

		IblResources GetIBLResources() { return {m_hasIBLTextures, m_Irradiance, m_Reflection, m_Reflectance}; }


	private:
		Shader* m_Shader = nullptr;
		Texture2D* m_Texture = nullptr;
		Texture2D *m_Irradiance = nullptr, *m_Reflection = nullptr, *m_Reflectance = nullptr;
		bool m_hasIBLTextures = false;
	};
}
