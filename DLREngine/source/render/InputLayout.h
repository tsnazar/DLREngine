#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "d3d.h"
#include "DxRes.h"
#include "Vertex.h"
#include "Instance.h"
#include "HashUtils.h"

namespace engine
{
	class InputLayout
	{
	public:
		struct LayoutSignature
		{
			VertexType verType;
			InstanceType insType;

			bool operator<(const LayoutSignature& other)
			{
				return std::tie(verType, insType) < std::tie(other.verType, other.insType);
			}

			friend bool operator==(const LayoutSignature& lhs, const LayoutSignature& rhs)
			{
				return std::tie(lhs.verType, lhs.insType) == std::tie(rhs.verType, rhs.insType);
			}
		};
	public:
		InputLayout(){}

		bool Create(ID3D10Blob* const blob, const LayoutSignature& signature);

		void SetInputLayout();
	private:
		DxResPtr<ID3D11InputLayout> m_Layout;
	};
}

namespace std
{
	template<>
	struct hash<engine::InputLayout::LayoutSignature>
	{
		std::size_t operator()(const engine::InputLayout::LayoutSignature& k) const
		{
			std::size_t res;
			hashCombine(res, k.verType);
			hashCombine(res, k.insType);
			return res;
		}
	};
}