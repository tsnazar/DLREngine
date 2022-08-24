#pragma once
#include "ConstantBuffer.h"
#include "Texture2D.h"
#include "RenderTarget.h"
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

		static Postprocess& Get() { return *s_Instance; }

		void Resolve(Texture2D& hdrInput, RenderTarget& ldrOutput);

		void Update(const ResolveConstants& constants);

	private:
		ConstantBuffer m_ResolveConstants;
	private:
		static Postprocess* s_Instance;
	};
}