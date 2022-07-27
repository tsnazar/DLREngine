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
	class Scene
	{
	protected:

	public:
		
	public:
		Scene() {};

		bool Render(MainWindow& win, Camera& camera);
		void Update(float dt);

		Sky& GetSky() { return m_Sky; }
		OpaqueInstances& GetOpaque() { return m_Opaque; }
	private:
		Sky m_Sky;
		OpaqueInstances m_Opaque;
	};
}
