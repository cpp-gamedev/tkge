#pragma once
#include <Tkge/Graphics/IGeometry.hpp>
#include <kvf/color.hpp>
#include <kvf/rect.hpp>
#include <array>

namespace Tkge::Graphics
{
	class QuadShape : public IGeometry
	{
	  public:
		static constexpr auto Indices = std::array{0u, 1u, 2u, 2u, 3u, 0u};
		static constexpr std::size_t VertexCount{6};

		[[nodiscard]] std::span<const Vertex> GetVertices() const final { return m_vertices; }
		[[nodiscard]] std::span<const std::uint32_t> GetIndices() const final { return Indices; }
		[[nodiscard]] vk::PrimitiveTopology GetTopology() const final { return vk::PrimitiveTopology::eTriangleList; }

		void Create(const kvf::Rect<>& rect, const kvf::UvRect& uv = kvf::uv_rect_v, kvf::Color colour = kvf::white_v);
		void Create(const glm::vec2 size) { Create(kvf::Rect<>::from_size(size)); }

		[[nodiscard]] kvf::Rect<> GetRect() const;
		[[nodiscard]] kvf::UvRect GetUv() const;
		[[nodiscard]] glm::vec2 GetSize() const { return GetRect().size(); }
		[[nodiscard]] kvf::Color GetColour() const { return m_vertices.front().colour; }

	  private:
		std::array<Vertex, VertexCount> m_vertices{};
	};
} // namespace Tkge::Graphics
