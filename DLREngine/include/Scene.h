#pragma once
#include <vector>
#include <memory>

#include "windows/MainWindow.h"
#include "windows/winapi.hpp"
#include "Camera.h"
#include "Shader.h"
#include "VertexBuffer.h"

namespace engine
{
	class Scene
	{
	protected:

	public:

	public:
		Scene() {};

		bool Render(MainWindow& win);

		engine::VertexBuffer<engine::VertexPosCol>& GetBuffer() { return m_Buffer; }
		engine::Shader& GetShader() { return m_Shader; }

	private:
		engine::Shader m_Shader;
		engine::VertexBuffer<engine::VertexPosCol> m_Buffer;
	};
}
