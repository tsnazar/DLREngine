#include "ShaderManager.h"
#include "Debug.h"

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
		s_Instance == nullptr;
	}

	Shader& ShaderManager::LoadShader(const std::string& name, VertexType type, const std::string& filepath)
	{
		ContainerShaders::iterator iter = m_Shaders.find(name);

		if (iter != m_Shaders.end()) // shader  with this name already exists
			return *(iter->second);

		std::unique_ptr<Shader>& shader = m_Shaders[name];
		shader = std::make_unique<Shader>();
		return shader->LoadFromFile(type, filepath);
	}

	void ShaderManager::LoadInputLayout(VertexType type, ID3D10Blob* blob)
	{
		if (InputLayoutExists(type)) // input  with this type already exists
			return;

		std::unique_ptr<InputLayout>& layout = m_InputLayouts[type];
		layout = std::make_unique<InputLayout>();
		layout->Create(type, blob);
	}

	bool ShaderManager::InputLayoutExists(VertexType type)
	{
		if (m_InputLayouts.find(type) != m_InputLayouts.end())
			return true;
		return false;
	}

	bool ShaderManager::ShaderExists(const std::string& name)
	{
		if (m_Shaders.find(name) != m_Shaders.end())
			return true;
		return false;
	}

	InputLayout& ShaderManager::GetInputLayout(VertexType type)
	{
		ContainerInputLayout::iterator iter = m_InputLayouts.find(type);

		if (iter == m_InputLayouts.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return *(iter->second);
	}

	Shader& ShaderManager::GetShader(const std::string& name)
	{
		ContainerShaders::iterator iter = m_Shaders.find(name);

		if (iter == m_Shaders.end()) // no texture with this name
			ALWAYS_ASSERT(false);

		return *(iter->second);
	}

}