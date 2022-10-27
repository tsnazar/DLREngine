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
#include "RenderTarget.h"
#include "ConstantBuffer.h"

namespace engine
{
	class Renderer
	{
	public:
		struct GBuffer
		{
			struct Dimensions
			{
				uint32_t width, height;
				DirectX::XMFLOAT2 padding;
			};

			DepthTarget depth; // 24d,8s
			Texture2D depthCopy; // 24d,8s
			RenderTarget albedo; //rgba8 unorm
			RenderTarget normals; // rgba16_snorm
			Texture2D normalsCopy; // rgba16_snorm
			RenderTarget roughnessMetalness; // r8g8_unorm
			RenderTarget emmission; // rgba16_float
			RenderTarget meshID; // r16_uint
			ConstantBuffer dimensions;
		};

	public:

		void CreateHDRTexture(uint32_t width, uint32_t height);

		void CreateGBuffer(uint32_t width, uint32_t height);

		bool Render(MainWindow& win, Camera& camera);

		void DeferredShading(MainWindow& win, Camera& camera);

		void Update(float dt, Camera& camera);

		Sky& GetSky() { return m_Sky; }
		
		RenderTarget& GetHDRRenderTarget() { return m_HDRTarget; }

		float GetEV100() { return m_EV100; }

		void SetEV100(float value) { m_EV100 = value; }

		GBuffer& GetGBuffer() { return m_GBuffer; }
	private:
		float m_EV100;
		Sky m_Sky;
		RenderTarget m_HDRTarget;
		GBuffer m_GBuffer;
	};
}
