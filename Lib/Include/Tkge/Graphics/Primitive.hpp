#pragma once
#include <Tkge/Graphics/Vertex.hpp>
#include <span>

namespace Tkge::Graphics
{
	struct Primitive
	{
		std::span<const Vertex> vertices{};
		std::span<const std::uint32_t> indices{};

		vk::PrimitiveTopology topology{vk::PrimitiveTopology::eTriangleList};
	};
} // namespace Tkge::Graphics
