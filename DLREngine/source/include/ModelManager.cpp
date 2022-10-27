#include "ModelManager.h"
#include "Debug.h"

namespace engine
{
	ModelManager* ModelManager::s_Instance = nullptr;

	ModelManager::ModelManager()
	{

	}

	void ModelManager::Init()
	{
		ALWAYS_ASSERT(s_Instance == nullptr);
		s_Instance = new ModelManager();

		s_Instance->CreateUnitShapes();
		//s_Instance->CreateModel("Cube").InitUnitCube();
		//s_Instance->CreateModel("Sphere").InitUnitSphere();
	}

	void ModelManager::CreateUnitShapes()
	{
		m_Cube = std::make_unique<Model>();
		m_Sphere = std::make_unique<Model>();
		m_Cube->InitUnitCube();
		m_Sphere->InitUnitSphere();
	}

	void ModelManager::Fini()
	{
		ALWAYS_ASSERT(s_Instance != nullptr);
		delete s_Instance;
		s_Instance = nullptr;
	}

	Model& ModelManager::CreateModel(const std::string& name)
	{
		ContainerModels::iterator iter = m_Models.find(name);

		if (iter != m_Models.end()) // model with this name already exists
			return *(iter->second);

		std::unique_ptr<Model>& model = m_Models[name];
		model = std::make_unique<Model>();

		return *model;
	}

	Model& ModelManager::LoadModel(const std::string& name, const std::string& filepath)
	{
		ContainerModels::iterator iter = m_Models.find(name);

		if (iter != m_Models.end()) // model with this name already exists
			return *(iter->second);

		std::unique_ptr<Model>& model= m_Models[name];
		model = std::make_unique<Model>();

		return model->LoadFromFile(filepath);
	}

	bool ModelManager::ModelExists(const std::string& name)
	{
		if (m_Models.find(name) != m_Models.end())
			return true;
		return false;
	}

	Model& ModelManager::GetModel(const std::string name)
	{
		ContainerModels::iterator iter = m_Models.find(name);

		if (iter == m_Models.end()) // no model with this name
			ALWAYS_ASSERT(false);

		return *(iter->second);
	}
	Model& ModelManager::GetUnitCube()
	{
		return *m_Cube;
	}
	Model& ModelManager::GetUnitSphere()
	{
		return *m_Sphere;
	}
}