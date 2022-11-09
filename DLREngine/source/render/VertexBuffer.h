#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Vertex.h"
#include "Debug.h"

namespace engine
{
	class VertexBuffer
	{
	public:

		template<typename T>
		void Create(D3D11_USAGE usage, const T* data, const uint32_t vertexCount);

		template<typename T>
		void Update(const T* data, const uint32_t size);

		void SetBuffer(uint32_t slot, D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void* Map();

		void UnMap();

		uint32_t GetVertexCount() { return m_VertexCount; }

		bool IsValid() { return m_Buffer.valid(); }

	private:
		bool m_Dynamic;
		VertexType m_VertexType;
		uint32_t m_VertexCount;
		uint32_t m_Stride, m_Offset;
		DxResPtr<ID3D11Buffer> m_Buffer;
	};

	template<typename T>
	inline void VertexBuffer::Create(D3D11_USAGE usage, const T* data, const uint32_t vertexCount)
	{
		if (m_Buffer.valid())
			m_Buffer.release();

		m_VertexType = GetVertexTypeFromStruct<T>();
		m_VertexCount = vertexCount;
		m_Stride = sizeof(T);
		m_Offset = 0;

		m_Dynamic = usage == D3D11_USAGE_DYNAMIC ? true : false;
		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = m_VertexCount * m_Stride;
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = m_Dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		
		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT result = s_Device->CreateBuffer(&desc, data == nullptr ? nullptr : &sr_data, m_Buffer.reset());
		if (FAILED(result))
		{
			result = s_Device->GetDeviceRemovedReason();
		}
		ALWAYS_ASSERT(SUCCEEDED(result));
	}

	template<typename T>
	void VertexBuffer::Update(const T* data, const uint32_t size)
	{
		ALWAYS_ASSERT(m_Stride == sizeof(T) && m_Dynamic);

		//m_VertexType = GetVertexTypeFromStruct<T>();
		//m_Offset = 0;
		//m_Stride = sizeof(T);
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		memcpy((void*)mappedResource.pData, data, size);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}

	inline void VertexBuffer::SetBuffer(uint32_t slot, D3D_PRIMITIVE_TOPOLOGY topology)
	{
		s_Devcon->IASetPrimitiveTopology(topology);
		s_Devcon->IASetVertexBuffers(slot, 1, m_Buffer.ptrAdr(), &m_Stride, &m_Offset);
	}

	inline void* VertexBuffer::Map()
	{
		ALWAYS_ASSERT(m_Dynamic);

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		return mappedResource.pData;
	}

	inline void VertexBuffer::UnMap()
	{
		ALWAYS_ASSERT(m_Dynamic);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}
}