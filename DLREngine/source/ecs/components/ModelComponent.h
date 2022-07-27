#pragma once
#include "Model.h"
#include "Texture2D.h"

namespace engine
{
	struct ModelComponent
	{
		Model* model;
		std::vector<Texture2D*> materials;
	};
}