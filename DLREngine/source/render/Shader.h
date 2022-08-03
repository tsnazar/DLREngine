#pragma once
#include <memory>
#include <string>
#include "d3d.h"
#include "DxRes.h"
#include "InputLayout.h"

namespace engine
{
	class Shader
	{
	public:
		Shader& LoadFromFile(const std::string& filepath, const std::vector<D3D11_INPUT_ELEMENT_DESC>* inputLayout);

		void SetShaders();
	private:

		DxResPtr<ID3D11VertexShader> m_VertexShader;
		DxResPtr<ID3D11PixelShader> m_PixelShader;
		InputLayout* m_Layout = nullptr;
	};
}

