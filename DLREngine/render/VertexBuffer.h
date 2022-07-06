#pragma once
#include "d3d.h"
#include "DxRes.h"
#include "Vertex.h"
#include "Debug.h"
#include "InputLayout.h"

namespace engine
{
	template<typename T>
	class VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer(const VertexType& vertexType, const T* data, const uint32_t vertexCount);

		void Create(const VertexType& vertexType, const T* data, const uint32_t vertexCount);

		void SetBuffer();

		uint32_t GetVertexCount() { return m_VertexCount; }

	private:
		VertexType m_VertexType;
		uint32_t m_VertexCount;
		uint32_t m_Stride, m_Offset;
		DxResPtr<ID3D11Buffer> m_Buffer;
	};


	template<typename T>
	inline VertexBuffer<T>::VertexBuffer()
	{

	}

	template<typename T>
	inline VertexBuffer<T>::VertexBuffer(const VertexType& vertexType, const T* data, const uint32_t vertexCount)
	{
		Create(vertexType, data, vertexCount);
	}

	template<typename T>
	inline void VertexBuffer<T>::Create(const VertexType& vertexType, const T* data, const uint32_t vertexCount)
	{
		m_VertexType = vertexType;
		m_VertexCount = vertexCount;
		m_Stride = sizeof(T);
		m_Offset = 0;
		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.ByteWidth = vertexCount * sizeof(T);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		
		D3D11_SUBRESOURCE_DATA sr_data = { 0 };
		sr_data.pSysMem = data;

		HRESULT hr = s_Device->CreateBuffer(&desc, &sr_data, m_Buffer.reset());
		ALWAYS_ASSERT(SUCCEEDED(hr));
	}

	template<typename T>
	inline void VertexBuffer<T>::SetBuffer()
	{
		if (s_Layouts.find(m_VertexType) == s_Layouts.end())
			ALWAYS_ASSERT(false);

		s_Devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		s_Layouts.at(m_VertexType)->SetInputLayout();
		s_Devcon->IASetVertexBuffers(0, 1, m_Buffer.ptrAdr(), &m_Stride, &m_Offset);
	}
}