#include "Shader.h"
#include "Debug.h"
#include "ShaderManager.h"

namespace engine
{
	Shader& Shader::LoadFromFile(const std::string& filepath, const std::vector<D3D11_INPUT_ELEMENT_DESC>* inputAttributes, int type)
	{
		m_Type = type;

		DxResPtr<ID3D10Blob> VS, PS, GS, CS, error;

		UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		std::wstring stemp = std::wstring(filepath.begin(), filepath.end());

		HRESULT result;

		if ((m_Type & ShaderType::Vertex) == ShaderType::Vertex)
		{
			result = D3DCompileFromFile(stemp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", flags, 0, VS.reset(), error.reset());
			if (FAILED(result)) {
				if (error.valid()) {
					OutputDebugStringA((char*)error->GetBufferPointer());
					error->Release();
				}
				if (VS.valid()) { VS->Release(); }
				ALWAYS_ASSERT(false);
			}

			result = s_Device->CreateVertexShader(
				VS->GetBufferPointer(),
				VS->GetBufferSize(),
				NULL,
				m_VertexShader.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if ((m_Type & ShaderType::Pixel) == ShaderType::Pixel)
		{
			result = D3DCompileFromFile(stemp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", flags, 0, PS.reset(), error.reset());
			if (FAILED(result)) {
				if (error.valid()) {
					OutputDebugStringA((char*)error->GetBufferPointer());
					error->Release();
				}
				if (PS.valid()) { PS->Release(); }
				ALWAYS_ASSERT(false);
			}

			result = s_Device->CreatePixelShader(
				PS->GetBufferPointer(),
				PS->GetBufferSize(),
				NULL,
				m_PixelShader.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if ((m_Type & ShaderType::Geometry) == ShaderType::Geometry)
		{
			result = D3DCompileFromFile(stemp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "gs_main", "gs_5_0", flags, 0, GS.reset(), error.reset());
			if (FAILED(result)) {
				if (error.valid()) {
					OutputDebugStringA((char*)error->GetBufferPointer());
					error->Release();
				}
				if (GS.valid()) { GS->Release(); }
				ALWAYS_ASSERT(false);
			}

			result = s_Device->CreateGeometryShader(
				GS->GetBufferPointer(),
				GS->GetBufferSize(),
				NULL,
				m_GeometryShader.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if ((m_Type & ShaderType::Compute) == ShaderType::Compute)
		{
			result = D3DCompileFromFile(stemp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "cs_main", "cs_5_0", flags, 0, CS.reset(), error.reset());
			if (FAILED(result)) {
				if (error.valid()) {
					OutputDebugStringA((char*)error->GetBufferPointer());
					error->Release();
				}
				if (CS.valid()) { CS->Release(); }
				ALWAYS_ASSERT(false);
			}

			result = s_Device->CreateComputeShader(
				CS->GetBufferPointer(),
				CS->GetBufferSize(),
				NULL,
				m_ComputeShader.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		m_Layout = nullptr;

		if(inputAttributes != nullptr && (m_Type & ShaderType::Vertex))
			m_Layout = &ShaderManager::Get().LoadInputLayout(*inputAttributes, VS.ptr());

		return *this;
	}

	void Shader::SetShaders()
	{		if (m_Layout != nullptr)			m_Layout->SetInputLayout();		else 			s_Devcon->IASetInputLayout(NULL);
		
		s_Devcon->VSSetShader(m_VertexShader.ptr(), NULL, 0);
		s_Devcon->PSSetShader(m_PixelShader.ptr(), NULL, 0);
		s_Devcon->GSSetShader(m_GeometryShader.ptr(), NULL, 0);
		s_Devcon->CSSetShader(m_ComputeShader.ptr(), NULL, 0);
	}
}