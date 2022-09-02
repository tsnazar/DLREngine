#pragma once
#include <DirectXMath.h>
#include <limits>
#include "MathUtils.h"
#include "windows/winapi.hpp"

namespace engine
{
	struct Box
	{
		DirectX::XMFLOAT3 min;
		DirectX::XMFLOAT3 max;

		static constexpr float Inf = std::numeric_limits<float>::infinity();
		inline static constexpr Box empty() {
			return { {Inf, Inf, Inf}, {-Inf, -Inf, -Inf} };
		}
		inline static constexpr Box unit() {
			return { {1.f, 1.f, 1.f}, {-1.f, -1.f, -1.f} };
		}

		inline DirectX::XMFLOAT3 size() const { return max - min; }
		inline DirectX::XMFLOAT3 center() const { return (min + max) / 2.0f; }
		inline float radius() const { return Length(size()) / 2.0f; }

		inline void reset()
		{
			constexpr float maxf = std::numeric_limits<float>::max();
			min = { maxf, maxf, maxf };
			max = -min;
		}

		inline void expand(const Box& other)
		{
			min = min < other.min ? min : other.min;
			max = max < other.max ? other.max : max;
		}

		inline bool contains(const DirectX::XMFLOAT3& P)
		{
			return min.x <= P.x && P.x <= max.x && min.y <= P.y && P.y <= max.y && min.z <= P.z && P.z <= max.z;
		}
	};

	
}
