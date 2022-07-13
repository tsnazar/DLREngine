#include "Shader.h"
#include "Debug.h"
#include "ResourceManager.h"

namespace engine
{
	Shader::Shader(const VertexType& type, const std::string& filepath)
	{
		LoadFromFile(type, filepath);
	}

	void Shader::LoadFromFile(const VertexType& type, const std::string& filepath)
	{
		DxResPtr<ID3D10Blob> VS, PS, error;

		UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		std::wstring stemp = std::wstring(filepath.begin(), filepath.end());

		HRESULT result = D3DCompileFromFile(stemp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "vs_main", "vs_5_0", flags, 0, VS.reset(), error.reset());
		if (FAILED(result)) {
			if (error.valid()) {
				OutputDebugStringA((char*)error->GetBufferPointer());
				error->Release();
			}
			if (VS.valid()) { VS->Release(); }
			ALWAYS_ASSERT(false);
		}

		result = D3DCompileFromFile(stemp.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "ps_main", "ps_5_0", flags, 0, PS.reset(), error.reset());
		if (FAILED(result)) {
			if (error.valid()) {
				OutputDebugStringA((char*)error->GetBufferPointer());
				error->Release();
			}
			if (PS.valid()) { PS->Release(); }
			ALWAYS_ASSERT(false);
		}

		result = s_Device->CreateVertexShader(
			VS->GetBufferPointer(),
			VS->GetBufferSize(),
			NULL,
			m_VertexShader.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		result = s_Device->CreatePixelShader(
			PS->GetBufferPointer(),
			PS->GetBufferSize(),
			NULL,
			m_PixelShader.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		/*if (type != VertexType::Undefined && s_Layouts.find(type) == s_Layouts.end())
		{
			auto& element = s_Layouts[type];
			element = std::make_shared<InputLayout>();
			element->Create(type, VS.ptr());
		}*/

		if (type != VertexType::Undefined)
			ResourceManager::Get().LoadInputLayout(type, VS.ptr());
	}

	void Shader::SetShaders()
	{		s_Devcon->VSSetShader(m_VertexShader.ptr(), NULL, 0);
		s_Devcon->PSSetShader(m_PixelShader.ptr(), NULL, 0);
	}
}