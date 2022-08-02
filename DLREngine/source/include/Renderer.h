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

namespace engine
{
	class Renderer
	{
	protected:

	public:
		
	public:
		Renderer() {};

		bool Render(MainWindow& win, Camera& camera);

		Sky& GetSky() { return m_Sky; }
	private:
		Sky m_Sky;
	};
}
