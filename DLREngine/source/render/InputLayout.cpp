#include "InputLayout.h"
#include "Debug.h"

namespace engine
{
	InputLayout& InputLayout::Create(ID3D10Blob* const blob, const std::vector<D3D11_INPUT_ELEMENT_DESC>& attributes)
	{

		HRESULT result = s_Device->CreateInputLayout(attributes.data(), attributes.size(), blob->GetBufferPointer(), blob->GetBufferSize(), m_Layout.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		return *this;
	}

	void InputLayout::SetInputLayout()
	{
		s_Devcon->IASetInputLayout(m_Layout.ptr());
	}
}


