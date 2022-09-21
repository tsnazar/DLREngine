#include "ShaderManager.h"
#include "Debug.h"

bool operator<(const D3D11_INPUT_ELEMENT_DESC& lhs, const D3D11_INPUT_ELEMENT_DESC& rhs)
{
	return std::tie(lhs.SemanticName, lhs.SemanticIndex, lhs.InputSlot, lhs.Format, lhs.AlignedByteOffset, lhs.InstanceDataStepRate, lhs.InputSlotClass) <
		std::tie(rhs.SemanticName, rhs.SemanticIndex, rhs.InputSlot, rhs.Format, rhs.AlignedByteOffset, rhs.InstanceDataStepRate, rhs.InputSlotClass);
}

namespace engine
{
	ShaderManager* ShaderManager::s_Instance = nullptr;
	
	ShaderManager::ShaderManager()
	{

	}

	void ShaderManager::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);
		s_Instance = new ShaderManager();

	}

	void ShaderManager::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
	}

	Shader& ShaderManager::LoadShader(const std::string& name, const std::string& filepath, const std::vector<D3D11_INPUT_ELEMENT_DESC>* inputAttributes, bool hasGS)
	{
		ContainerShaders::iterator iter = m_Shaders.find(name);

		if (iter != m_Shaders.end()) // shader  with this name already exists
			return *(iter->second);

		std::unique_ptr<Shader>& shader = m_Shaders[name];
		shader = std::make_unique<Shader>();
		return shader->LoadFromFile(filepath, inputAttributes, hasGS);
	}

	InputLayout& ShaderManager::LoadInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputAttributes, ID3D10Blob* blob)
	{
		ContainerInputLayout::iterator iter = m_InputLayouts.find(inputAttributes);

		if (iter != m_InputLayouts.end()) // input  with this name already exists
			return *(iter->second);

		std::unique_ptr<InputLayout>& layout = m_InputLayouts[inputAttributes];
		layout = std::make_unique<InputLayout>();
		return layout->Create(blob, inputAttributes);
	}

	bool ShaderManager::InputLayoutExists(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputAttributes)
	{
		if (m_InputLayouts.find(inputAttributes) != m_InputLayouts.end())
			return true;
		return false;
	}

	InputLayout& ShaderManager::GetInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayout)
	{
		ContainerInputLayout::iterator iter = m_InputLayouts.find(inputLayout);

		if (iter == m_InputLayouts.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return *(iter->second);
	}

	bool ShaderManager::ShaderExists(const std::string& name)
	{
		if (m_Shaders.find(name) != m_Shaders.end())
			return true;
		return false;
	}

	Shader& ShaderManager::GetShader(const std::string& name)
	{
		ContainerShaders::iterator iter = m_Shaders.find(name);

		if (iter == m_Shaders.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return *(iter->second);
	}

}