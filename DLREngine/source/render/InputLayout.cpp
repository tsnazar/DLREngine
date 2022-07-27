#include "InputLayout.h"
#include "Debug.h"

namespace engine
{
	bool InputLayout::Create(ID3D10Blob* const blob, const LayoutSignature& signature)
	{
		if (blob == nullptr)
			return false;

		const uint32_t binding = 0;

		std::vector<D3D11_INPUT_ELEMENT_DESC> attributes;

		/*if (signature.verType == VertexType::Undefined)
			return true;*/

		if (signature.verType == VertexType::Pos)
		{
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPos, pos), D3D11_INPUT_PER_VERTEX_DATA, 0});
		} else if(signature.verType == VertexType::PosCol)
		{
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosCol, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "COL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosCol, col), D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (signature.verType == VertexType::PosTex)
		{
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTex, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}
		else if (signature.verType == VertexType::PosTexNorTan)
		{
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosTexNorTan, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, nor), D3D11_INPUT_PER_VERTEX_DATA, 0 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosTexNorTan, tan), D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}

		/*if (signature.insType == InstanceType::Undefined);*/

		if (signature.insType == InstanceType::Transform)
		{
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "MAT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(InstanceTransform, matrix[0]), D3D11_INPUT_PER_INSTANCE_DATA, 1 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "MAT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(InstanceTransform, matrix[1]), D3D11_INPUT_PER_INSTANCE_DATA, 1 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "MAT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(InstanceTransform, matrix[2]), D3D11_INPUT_PER_INSTANCE_DATA, 1 });
			attributes.push_back(D3D11_INPUT_ELEMENT_DESC{ "MAT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, offsetof(InstanceTransform, matrix[3]), D3D11_INPUT_PER_INSTANCE_DATA, 1 });
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


