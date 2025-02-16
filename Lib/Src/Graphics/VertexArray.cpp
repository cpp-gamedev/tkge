#include <Tkge/Graphics/VertexArray.hpp>
#include <algorithm>

namespace Tkge::Graphics
{
	namespace
	{
		void AppendVerts(VertexArray& out, std::span<const Vertex> vertices, std::span<const std::uint32_t> indices)
		{
			const auto indexOffset = std::uint32_t(out.vertices.size());
			out.vertices.insert(out.vertices.end(), vertices.begin(), vertices.end());
			out.indices.reserve(out.indices.size() + indices.size());
			std::ranges::transform(indices, std::back_inserter(out.indices), [indexOffset](std::uint32_t i) { return indexOffset + i; });
		}
	} // namespace

	void VertexArray::Reserve(const std::size_t vertexCount, const std::size_t indexCount)
	{
		vertices.reserve(vertices.size() + vertexCount);
		indices.reserve(indices.size() + indexCount);
	}

	void VertexArray::Clear()
	{
		vertices.clear();
		indices.clear();
	}

	auto VertexArray::Append(std::span<const Vertex> vertices, std::span<const std::uint32_t> indices) -> VertexArray&
	{
		AppendVerts(*this, vertices, indices);
		return *this;
	}
} // namespace Tkge::Graphics
