#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "DxRes.h"
#include "Vertex.h"
#include "d3d.h"
#include <memory>

namespace engine
{
	class InputLayout
	{
	public:
		InputLayout(){}

		bool Create(const VertexType& type, ID3D10Blob* const blob = nullptr);

		void SetInputLayout();
	private:
		DxResPtr<ID3D11InputLayout> m_Layout;
	};
}