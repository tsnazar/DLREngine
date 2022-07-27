#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Debug.h"

namespace engine
{
	class IndexBuffer
	{
	public:
		template<typename T>
		void Create(D3D11_USAGE usage, const T* data, const uint32_t size);

		template<typename T>
		void Update(const T* data, const uint32_t size);

		void Bind();

	private:
		uint32_t m_IndexCount;
		uint32_t m_Stride, m_Offset;
		DxResPtr<ID3D11Buffer> m_Buffer;
	};


	template<typename T>
	void IndexBuffer::Create(D3D11_USAGE usage, const T* data, const uint32_t size)
	{
		if (m_Buffer.valid())
			m_Buffer.release();

		m_IndexCount = size;
		m_Stride = sizeof(T);
		m_Offset = 0;

		bool dynamic = usage == D3D11_USAGE_DYNAMIC;

		D3D11_BUFFER_DESC desc = { 0 };

		desc.ByteWidth = m_Stride * m_IndexCount;
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT hr = s_Device->CreateBuffer(&desc, &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(hr));
	}

	template<typename T>
	void IndexBuffer::Update(const T* data, const uint32_t size)
	{
		m_IndexCount = size;
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		memcpy((void*)mappedResource.pData, data, size);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}

	inline void IndexBuffer::Bind()
	{
		s_Devcon->IASetIndexBuffer(m_Buffer.ptr(), m_Stride == sizeof(uint16_t) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, m_Offset);
	}
}