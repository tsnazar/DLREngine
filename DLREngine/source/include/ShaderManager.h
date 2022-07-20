#pragma once
#include <unordered_map>
#include "Shader.h"
#include "InputLayout.h"

namespace engine
{
	class ShaderManager
	{
	public:
		static void Init();

		static void Fini();

		inline static ShaderManager& Get() { return *s_Instance; }

		Shader& LoadShader(const std::string& name, VertexType type, const std::string& filepath);
		void LoadInputLayout(VertexType type, ID3D10Blob* blob);

		bool InputLayoutExists(VertexType type);
		bool ShaderExists(const std::string& name);

		InputLayout& GetInputLayout(VertexType type);
		Shader& GetShader(const std::string& name);

	private:
		using ContainerShaders = std::unordered_map<std::string, std::unique_ptr<Shader>>;
		using ContainerInputLayout = std::unordered_map<VertexType, std::unique_ptr<InputLayout>>;

		ContainerShaders m_Shaders;
		ContainerInputLayout m_InputLayouts;

	protected:
		ShaderManager();

		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		ShaderManager& operator=(ShaderManager&&) = delete;

		static ShaderManager* s_Instance;
	};
}
