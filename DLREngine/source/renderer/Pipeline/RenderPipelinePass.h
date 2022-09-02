#pragma once
#include <string>

namespace engine
{
	class RenderPipelinePass
	{
	public:
		RenderPipelinePass(const std::string& name);
		virtual	~RenderPipelinePass();

		void SetName(const std::string& name);

		std::string GetName() const;

		virtual bool GetRenderTargetDescriptions()
	private:

	};
}