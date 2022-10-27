#pragma once
#include "ConstantBuffer.h"
#include "Texture2D.h"
#include "RenderTarget.h"
#include "Shader.h"
#include <DirectXMath.h>

namespace engine
{
	class Postprocess
	{
	public:
		struct ResolveConstants
		{
			float EV100;
			DirectX::XMFLOAT3 padding;
		};

		struct ShaderDescription
		{
			enum Bindings : uint32_t { HDR_TEXTURE = 0, CONSTANTS = 0 };
		};

	public:
		Postprocess();

		void static Init();

		void static Fini();

		void SetShader(Shader* resolveShader) { m_ResolveShader = resolveShader; }

		static Postprocess& Get() { return *s_Instance; }

		void ResolveMS(Texture2D& hdrInput, RenderTarget& ldrOutput);

		void Resolve(Texture2D& hdrInput, RenderTarget& ldrOutput);

		void Update(const ResolveConstants& constants);

	private:
		ConstantBuffer m_ResolveConstants;

		Shader* m_ResolveShader = nullptr;

	private:
		static Postprocess* s_Instance;
	};
}