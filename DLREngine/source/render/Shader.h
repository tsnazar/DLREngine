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
		enum ShaderType 
		{
			Vertex = 1 << 0, Pixel = 1 << 1, VertexPixel = 3, Geometry = 1 << 2, VertexPixelGeometry = 7, Compute = 1 << 3
		};
	public:
		Shader& LoadFromFile(const std::string& filepath, const std::vector<D3D11_INPUT_ELEMENT_DESC>* inputLayout, int type);

		void SetShaders();
	private:
		DxResPtr<ID3D11GeometryShader> m_GeometryShader;
		DxResPtr<ID3D11VertexShader> m_VertexShader;
		DxResPtr<ID3D11PixelShader> m_PixelShader;
		DxResPtr<ID3D11ComputeShader> m_ComputeShader;
		InputLayout* m_Layout = nullptr;
		int m_Type;
		//bool m_HasGS = false;
	};
}

