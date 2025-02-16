#pragma once
#include <tkge/graphics/vertex.hpp>
#include <span>

namespace tkge::graphics
{
	struct Primitive
	{
		std::span<const Vertex> vertices{};
		std::span<const std::uint32_t> indices{};

		vk::PrimitiveTopology topology{vk::PrimitiveTopology::eTriangleList};
	};
} // namespace tkge::graphics
