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

namespace engine
{
	class Scene
	{
	protected:

	public:
		
	public:
		Scene() {};

		bool Render(MainWindow& win, Camera& camera);

		VertexBuffer& GetBuffer() { return m_Buffer; }
		Sky& GetSky() { return m_Sky; }
	private:
		VertexBuffer m_Buffer;
		Sky m_Sky;
	};
}
