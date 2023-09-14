#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Debug.h"

namespace engine
{
	class ConstantBuffer
	{
	public:

		template<typename T>
		void Create(D3D11_USAGE usage, const T* data, const uint32_t size);

		template<typename T>
		void Update(const T* data, const uint32_t size);
		
		void BindToVS(uint32_t slot);
		void BindToGS(uint32_t slot);
		void BindToPS(uint32_t slot);
		void BindToCS(uint32_t slot);

	private:
		bool m_Dynamic;
		uint32_t m_Stride, m_Offset, m_ByteWidthGPU;
		DxResPtr<ID3D11Buffer> m_Buffer;
	};

	template<typename T>
	void ConstantBuffer::Create(D3D11_USAGE usage, const T* data, const uint32_t size)
	{
		if (m_Buffer.valid())
			m_Buffer.release();

		m_Stride = sizeof(T);
		m_Offset = 0;
		m_ByteWidthGPU = size * m_Stride;

		m_Dynamic = usage == D3D11_USAGE_DYNAMIC ? true : false;

		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = m_ByteWidthGPU;
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = m_Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;
		sr_data.SysMemPitch = 0;
		sr_data.SysMemSlicePitch = 0;

		HRESULT result = s_Device->CreateBuffer(&desc, data == nullptr ? NULL : &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));
	}

	template<typename T>
	void ConstantBuffer::Update(const T* data, const uint32_t size)
	{
		ALWAYS_ASSERT(m_Stride == sizeof(T) && m_Dynamic);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		memcpy((void*)mappedResource.pData, data, m_Stride * size);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}

	inline void ConstantBuffer::BindToVS(uint32_t slot)
	{
		s_Devcon->VSSetConstantBuffers(slot, 1, m_Buffer.ptrAdr());
	}

	inline void ConstantBuffer::BindToGS(uint32_t slot)
	{
		s_Devcon->GSSetConstantBuffers(slot, 1, m_Buffer.ptrAdr());
	}

	inline void ConstantBuffer::BindToPS(uint32_t slot)
	{
		s_Devcon->PSSetConstantBuffers(slot, 1, m_Buffer.ptrAdr());
	}

	inline void ConstantBuffer::BindToCS(uint32_t slot)
	{
		s_Devcon->CSSetConstantBuffers(slot, 1, m_Buffer.ptrAdr());
	}
}