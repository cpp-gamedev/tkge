#include <Tkge/Graphics/Shape.hpp>

namespace Tkge::Graphics
{
	namespace
	{
		constexpr std::size_t LeftBottom{0};
		constexpr std::size_t RightBottom{1};
		constexpr std::size_t RightTop{2};
		constexpr std::size_t LeftTop{3};
	} // namespace

	void QuadShape::Create(const kvf::Rect<>& rect, const kvf::UvRect& uv, const kvf::Color colour)
	{
		m_vertices[LeftBottom] = Vertex{.position = rect.bottom_left(), .colour = colour.to_linear(), .uv = uv.bottom_left()};
		m_vertices[RightBottom] = Vertex{.position = rect.bottom_right(), .colour = colour.to_linear(), .uv = uv.bottom_right()};
		m_vertices[RightTop] = Vertex{.position = rect.top_right(), .colour = colour.to_linear(), .uv = uv.top_right()};
		m_vertices[LeftTop] = Vertex{.position = rect.top_left(), .colour = colour.to_linear(), .uv = uv.top_left()};
	}

	kvf::Rect<> QuadShape::GetRect() const { return kvf::Rect<>{.lt = m_vertices[LeftTop].position, .rb = m_vertices[RightBottom].position}; }

	[[nodiscard]] kvf::UvRect QuadShape::GetUv() const { return {.lt = m_vertices[LeftTop].uv, .rb = m_vertices[RightBottom].uv}; }

} // namespace Tkge::Graphics
