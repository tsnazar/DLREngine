#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Vertex.h"
#include "Debug.h"
#include "InputLayout.h"
#include "ShaderManager.h"

namespace engine
{
	class VertexBuffer
	{
	public:

		template<typename T>
		void Create(D3D11_USAGE usage, const T* data, const uint32_t vertexCount);

		template<typename T>
		void Update(const T* data, const uint32_t size);

		void SetBuffer(uint32_t slot);

		void* Map();

		void UnMap();

		uint32_t GetVertexCount() { return m_VertexCount; }

		bool IsValid() { return m_Buffer.valid(); }

	private:
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

		bool dynamic = usage == D3D11_USAGE_DYNAMIC;
		
		D3D11_BUFFER_DESC desc = { 0 };
		desc.ByteWidth = vertexCount * m_Stride;
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		
		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT result = s_Device->CreateBuffer(&desc, data == nullptr ? nullptr : &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(result));
	}

	template<typename T>
	void VertexBuffer::Update(const T* data, const uint32_t size)
	{
		m_VertexType = GetVertexTypeFromStruct<T>();
		m_Offset = size;
		m_Stride = sizeof(T);
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		memcpy((void*)mappedResource.pData, data, size);

		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}

	inline void VertexBuffer::SetBuffer(uint32_t slot)
	{
		//if (!ShaderManager::Get().InputLayoutExists(m_VertexType))
			//ALWAYS_ASSERT(false);

		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//ShaderManager::Get().GetInputLayout(m_VertexType).SetInputLayout();
		s_Devcon->IASetVertexBuffers(slot, 1, m_Buffer.ptrAdr(), &m_Stride, &m_Offset);
	}

	inline void* VertexBuffer::Map()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		HRESULT result = s_Devcon->Map(m_Buffer.ptr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		ALWAYS_ASSERT(SUCCEEDED(result));

		return mappedResource.pData;
	}

	inline void VertexBuffer::UnMap()
	{
		s_Devcon->Unmap(m_Buffer.ptr(), 0);
	}
}