#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "DxRes.h"
#include "Vertex.h"
#include "d3d.h"
#include <memory>

namespace engine
{
	struct VertexAttribute
	{
		VertexAttribute(const std::string& name, const uint32_t location, const uint32_t binding, const uint32_t format, const uint32_t offset)
			: name(name), location(location), binding(binding), format(format), offset(offset) {}

		std::string name;
		uint32_t location;
		uint32_t binding;
		uint32_t format;
		uint32_t offset;
	};


	class InputLayout
	{
	public:
		InputLayout(){}

		bool Create(const VertexType& type, ID3D10Blob* const blob = nullptr);

		void SetInputLayout();
	private:
		DxResPtr<ID3D11InputLayout> m_Layout;
	};

	extern std::unordered_map<VertexType, std::shared_ptr<InputLayout>> s_Layouts;
}