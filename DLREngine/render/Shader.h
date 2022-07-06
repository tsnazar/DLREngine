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
		Shader() {};
		Shader(const VertexType& type, const std::string& filepath);
		
		void LoadFromFile(const VertexType& type, const std::string& filepath);

		void SetShaders();
	private:

		DxResPtr<ID3D11VertexShader> m_VertexShader;
		DxResPtr<ID3D11PixelShader> m_PixelShader;
	};
}

