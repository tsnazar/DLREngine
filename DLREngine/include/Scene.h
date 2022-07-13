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

namespace engine
{
	class Scene
	{
	protected:

	public:
		struct FrustumCorners
		{
			DirectX::XMFLOAT4 pos[3];
		};
	public:
		Scene() {};

		bool Render(MainWindow& win, Camera& camera);

		engine::VertexBuffer& GetBuffer() { return m_Buffer; }
		engine::ConstantBuffer& GetConstantBuffer() { return m_ConstantBuffer; }

	private:
		engine::VertexBuffer m_Buffer;
		engine::ConstantBuffer m_ConstantBuffer;
	};
}
