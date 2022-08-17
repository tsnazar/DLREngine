#pragma once
#include <string>
#include <vector>
#include "d3d.h"
#include "DxRes.h"
#include "Vertex.h"

namespace engine
{
	class InputLayout
	{
	public:
		InputLayout(){}

		InputLayout& Create(ID3D10Blob* const blob, const std::vector<D3D11_INPUT_ELEMENT_DESC>& attributes);

		void SetInputLayout();
	private:
		DxResPtr<ID3D11InputLayout> m_Layout;
	};
}