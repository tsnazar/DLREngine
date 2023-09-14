#pragma once
#include <DirectXMath.h>
#include <limits>
#include "Model.h"

namespace engine
{
	struct Query
	{
		enum class ShadingGroup { Opaque, Disintegration, Lights, None};

		float t;
		uint32_t transformID;
		uint32_t objectID;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
		bool usable;
		ShadingGroup shadingGroup;
		Model* pModel;

		void reset()
		{
			objectID = 0;
			t = std::numeric_limits<float>::infinity();
			pos = { 0,0,0 };
			normal = { 0,0,0 };
			usable = false;
			shadingGroup = ShadingGroup::None;
			pModel = nullptr;
		}
	};
}