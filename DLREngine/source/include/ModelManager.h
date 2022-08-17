#pragma once
#include "Model.h"
namespace engine
{
	class ModelManager
	{
	public:
		static void Init();

		static void Fini();

		inline static ModelManager& Get() { return *s_Instance; }

		Model& CreateModel(const std::string& name);

		Model& LoadModel(const std::string& name, const std::string& filepath);

		bool ModelExists(const std::string& name);

		Model& GetModel(const std::string name);
	private:
		using ContainerModels = std::unordered_map<std::string, std::unique_ptr<Model>>;

		ContainerModels m_Models;
	protected:
		ModelManager();

		ModelManager(const ModelManager&) = delete;
		ModelManager& operator=(const ModelManager&) = delete;
		ModelManager(ModelManager&&) = delete;
		ModelManager& operator=(ModelManager&&) = delete;

		static ModelManager* s_Instance;
	};
}