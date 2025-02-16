#pragma once
#include <tkge/graphics/vertex.hpp>
#include <cstdint>
#include <span>
#include <vector>

namespace tkge::graphics
{
	struct VertexArray
	{
		std::vector<Vertex> vertices{};
		std::vector<std::uint32_t> indices{};

		void Reserve(std::size_t vertexCount, std::size_t indexCount);
		void Clear();
		auto Append(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices) -> VertexArray&;
	};
} // namespace tkge::graphics
