#include "InputLayout.h"
#include "Debug.h"

namespace engine
{
	bool InputLayout::Create(const VertexType& type, ID3D10Blob* const blob)
	{
		if (blob == nullptr)
			return false;

		const uint32_t binding = 0;

		std::vector<D3D11_INPUT_ELEMENT_DESC> attributes;

		if (type == VertexType::Undefined)
			return false;

		if (type == VertexType::Pos)
		{
			attributes =
			{
				{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPos, pos), D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		} else if(type == VertexType::PosCol)
		{
			attributes =
			{
				{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosCol, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosCol, col), D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		}
		else if (type == VertexType::PosTex)
		{
			attributes =
			{
				{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTex, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
		}

		HRESULT result = s_Device->CreateInputLayout(attributes.data(), attributes.size(), blob->GetBufferPointer(), blob->GetBufferSize(), m_Layout.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		return true;
	}
	void InputLayout::SetInputLayout()
	{
		s_Devcon->IASetInputLayout(m_Layout.ptr());
	}
}


