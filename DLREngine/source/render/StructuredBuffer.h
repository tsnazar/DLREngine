#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Debug.h"

namespace engine
{
	class StructuredBuffer
	{
	public:

		template<typename T>
		void Create(const T* data, const uint32_t vertexCount);

		template<typename T>
		void Update(const T* data, const uint32_t size);

		void* Map();

		void UnMap();

		void BindToVS(uint32_t slot)
		{
			ALWAYS_ASSERT(s_Devcon != nullptr && m_SRV.valid());
			s_Devcon->VSSetShaderResources(slot, 1, m_SRV.ptrAdr());
		}

		void BindToGS(uint32_t slot)
		{
			ALWAYS_ASSERT(s_Devcon != nullptr && m_SRV.valid());
			s_Devcon->GSSetShaderResources(slot, 1, m_SRV.ptrAdr());
		}

		void BindToPS(uint32_t slot)
		{
			ALWAYS_ASSERT(s_Devcon != nullptr && m_SRV.valid());
			s_Devcon->PSSetShaderResources(slot, 1, m_SRV.ptrAdr());
		}

		void BindToCS(uint32_t slot)
		{
			ALWAYS_ASSERT(s_Devcon != nullptr && m_UAV.valid());
			s_Devcon->CSSetUnorderedAccessViews(slot, 1, m_UAV.ptrAdr(), &m_Offset);
		}

		DxResPtr<ID3D11UnorderedAccessView>& GetUAV() { return m_UAV; }

		DxResPtr<ID3D11Buffer>& GetBuffer() { return m_Buffer; }

		uint32_t GetSize() { return m_Size; }

		bool IsValid() { return m_Buffer.valid(); }

	private:
		uint32_t m_Size;
		uint32_t m_Stride, m_Offset;
		DxResPtr<ID3D11Buffer> m_Buffer;
		DxResPtr<ID3D11UnorderedAccessView> m_UAV;
		DxResPtr<ID3D11ShaderResourceView> m_SRV;
	};

	template<typename T>
	inline void StructuredBuffer::Create(const T* data, const uint32_t size)
	{
		if (m_Buffer.valid())
			m_Buffer.release();

		m_Size = size;
		m_Stride = sizeof(T);
		m_Offset = 0;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = m_Size * m_Stride;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = m_Stride;

		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT result = s_Device->CreateBuffer(&desc, data == nullptr ? nullptr : &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_Size;

		result = s_Device->CreateUnorderedAccessView(m_Buffer.ptr(), &uavDesc, m_UAV.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		result = s_Device->CreateShaderResourceView(m_Buffer.ptr(), NULL, m_SRV.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));
	}

	template<typename T>
	void StructuredBuffer::Update(const T* data, const uint32_t size)
	{
		ALWAYS_ASSERT(m_Stride == sizeof(T));
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		memcpy((void*)mappedResource.pData, data, size);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}

	inline void* StructuredBuffer::Map()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		return mappedResource.pData;
	}

	inline void StructuredBuffer::UnMap()
	{
		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}
}