#pragma once

#include <utility>

namespace engine
{
	template <typename T>
	struct DxResPtr
	{
		DxResPtr() { m_Ptr = nullptr; }
		~DxResPtr() { release(); }

		DxResPtr(const DxResPtr& other) { *this = other; }
		DxResPtr& operator=(const DxResPtr& other)
		{
			if (m_Ptr) m_Ptr->Release();
			m_Ptr = other.m_Ptr;
			m_Ptr->AddRef();
			return *this;
		}

		DxResPtr(DxResPtr&& other) noexcept { *this = std::move(other); }
		DxResPtr& operator=(DxResPtr&& other) noexcept
		{
			if (m_Ptr) m_Ptr->Release();
			m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			return *this;
		}

		T* ptr() const { return m_Ptr; }
		T* ptr() { return m_Ptr; }

		T** ptrAdr() const { return &m_Ptr; }
		T** ptrAdr() { return &m_Ptr; }

		T* operator->() const { return m_Ptr; }
		T* operator->() { return m_Ptr; }

		operator T* () const { return m_Ptr; }
		operator T* () { return m_Ptr; }

		T** reset()
		{
			release();
			return &m_Ptr;
		}

		void reset(T* ptr)
		{
			release();
			m_Ptr = ptr;
		}

		void release()
		{
			if (m_Ptr)
			{
				m_Ptr->Release();
				m_Ptr = nullptr;
			}
		}

		bool valid()
		{
			return m_Ptr != nullptr;
		}
	protected:
		T* m_Ptr;
	};
}