#pragma once
#include <unordered_map>
#include <map>
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

		Shader& LoadShader(const std::string& name, const std::string& filepath, const std::vector<D3D11_INPUT_ELEMENT_DESC>* inputAttributes, bool hasGS = false);
		
		InputLayout& LoadInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputAttributes, ID3D10Blob* blob);

		bool ShaderExists(const std::string& name);
		
		bool InputLayoutExists(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputAttributes);

		Shader& GetShader(const std::string& name);
		
		InputLayout& GetInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputAttributes);

	private:
		using ContainerShaders = std::unordered_map<std::string, std::unique_ptr<Shader>>;
		
		using ContainerInputLayout = std::map<std::vector<D3D11_INPUT_ELEMENT_DESC>, std::unique_ptr<InputLayout>>;

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
