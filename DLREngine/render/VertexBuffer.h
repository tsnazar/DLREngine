#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Vertex.h"
#include "Debug.h"
#include "InputLayout.h"
#include "ResourceManager.h"

namespace engine
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		//VertexBuffer(const VertexType& vertexType, const T* data, const uint32_t vertexCount);

		template<typename T>
		void Create(D3D11_USAGE usage, const T* data, const uint32_t vertexCount);

		void SetBuffer();

		uint32_t GetVertexCount() { return m_VertexCount; }

	private:
		VertexType m_VertexType;
		uint32_t m_VertexCount;
		uint32_t m_Stride, m_Offset;
		DxResPtr<ID3D11Buffer> m_Buffer;
	};


	inline VertexBuffer::VertexBuffer()
	{

	}

	template<typename T>
	inline void VertexBuffer::Create(D3D11_USAGE usage, const T* data, const uint32_t vertexCount)
	{
		m_VertexType = GetVertexTypeFromStruct<T>();
		m_VertexCount = vertexCount;
		m_Stride = sizeof(T);
		m_Offset = 0;
		
		D3D11_BUFFER_DESC desc = { 0 };

		desc.ByteWidth = vertexCount * sizeof(T);
		desc.Usage = usage;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		
		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT hr = s_Device->CreateBuffer(&desc, &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(hr));
	}

	inline void VertexBuffer::SetBuffer()
	{
		if (!ResourceManager::Get().InputLayoutExists(m_VertexType))
			ALWAYS_ASSERT(false);

		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ResourceManager::Get().GetInputLayout(m_VertexType).SetInputLayout();
		s_Devcon->IASetVertexBuffers(0, 1, m_Buffer.ptrAdr(), &m_Stride, &m_Offset);
	}
}