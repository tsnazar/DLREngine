#pragma once
#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Debug.h"

namespace engine
{
	class Buffer
	{
	public:

		template<typename T>
		void Create(D3D11_USAGE usage, const T* data, const uint32_t size, uint32_t bindFlags, uint32_t miscFlags, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

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
		bool m_Dynamic;
		uint32_t m_Size;
		uint32_t m_Stride, m_Offset;
		DxResPtr<ID3D11Buffer> m_Buffer;
		DxResPtr<ID3D11UnorderedAccessView> m_UAV;
		DxResPtr<ID3D11ShaderResourceView> m_SRV;
	};

	template<typename T>
	inline void Buffer::Create(D3D11_USAGE usage, const T* data, const uint32_t size, uint32_t bindFlags, uint32_t miscFlags, DXGI_FORMAT format)
	{
		if (m_Buffer.valid())
			m_Buffer.release();

		m_Dynamic = usage == D3D11_USAGE_DYNAMIC ? true : false;
		bool uav = (bindFlags & D3D11_BIND_UNORDERED_ACCESS) == D3D11_BIND_UNORDERED_ACCESS ? true : false;
		bool srv = (bindFlags & D3D11_BIND_SHADER_RESOURCE) == D3D11_BIND_SHADER_RESOURCE ? true : false;

		m_Size = size;
		m_Stride = sizeof(T);
		m_Offset = 0;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = m_Size * m_Stride;
		desc.Usage = usage;
		desc.BindFlags = bindFlags;
		desc.CPUAccessFlags = m_Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = miscFlags;
		desc.StructureByteStride = m_Stride;

		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT result = s_Device->CreateBuffer(&desc, data == nullptr ? nullptr : &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));

		if (uav)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Format = format;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = m_Size;

			result = s_Device->CreateUnorderedAccessView(m_Buffer.ptr(), &uavDesc, m_UAV.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}

		if (srv)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.FirstElement = 0;
			srvDesc.Buffer.NumElements = m_Size;

			result = s_Device->CreateShaderResourceView(m_Buffer.ptr(), &srvDesc, m_SRV.reset());
			ALWAYS_ASSERT(SUCCEEDED(result));
		}
	}

	template<typename T>
	void Buffer::Update(const T* data, const uint32_t size)
	{
		ALWAYS_ASSERT(m_Stride == sizeof(T) && m_Dynamic);
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		memcpy((void*)mappedResource.pData, data, size);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}

	inline void* Buffer::Map()
	{
		ALWAYS_ASSERT(m_Dynamic);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		return mappedResource.pData;
	}

	inline void Buffer::UnMap()
	{
		ALWAYS_ASSERT(m_Dynamic);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}
}