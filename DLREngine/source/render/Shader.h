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
		Shader& LoadFromFile(const std::string& filepath, const std::vector<D3D11_INPUT_ELEMENT_DESC>* inputLayout, bool hasGS = false);

		void SetShaders();
	private:
		DxResPtr<ID3D11GeometryShader> m_GeometryShader;
		DxResPtr<ID3D11VertexShader> m_VertexShader;
		DxResPtr<ID3D11PixelShader> m_PixelShader;
		InputLayout* m_Layout = nullptr;
		bool m_HasGS = false;
	};
}

