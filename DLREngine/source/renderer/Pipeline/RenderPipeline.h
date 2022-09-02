#pragma once
#include <vector>
#include "Texture2D.h"
#include "RenderPipelinePass.h"

namespace engine
{
	class RenderPipeline
	{
	public:

	private:
		std::vector<RenderPipelinePass*> m_Passes;
		std::vector<std::unique_ptr<Texture2D>> m_RenderTargets;
	};
}