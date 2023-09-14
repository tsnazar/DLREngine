#pragma once
#include <vector>
#include <memory>

#include "windows/MainWindow.h"
#include "windows/winapi.hpp"
#include "Camera.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Texture2D.h"
#include "ConstantBuffer.h"
#include "Sky.h"
#include "OpaqueInstances.h"
#include "RenderTarget.h"
#include "ConstantBuffer.h"

namespace engine
{
	class Renderer
	{
	public:

		void CreateHDRTexture(uint32_t width, uint32_t height);

		bool Render(MainWindow& win, Camera& camera);

		void Update(float dt, Camera& camera);

		Sky& GetSky() { return m_Sky; }
		
		RenderTarget& GetHDRRenderTarget() { return m_HDRTarget; }

		float GetEV100() { return m_EV100; }

		void SetEV100(float value) { m_EV100 = value; }

	private:
		float m_EV100;
		Sky m_Sky;
		RenderTarget m_HDRTarget;
	};
}
