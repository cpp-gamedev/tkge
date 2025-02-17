#pragma once
#include <Tkge/Graphics/Vertex.hpp>
#include <klib/polymorphic.hpp>
#include <cstdint>
#include <span>

namespace Tkge::Graphics
{
	class IGeometry : public klib::Polymorphic
	{
	  public:
		[[nodiscard]] virtual std::span<const Vertex> GetVertices() const = 0;
		[[nodiscard]] virtual std::span<const std::uint32_t> GetIndices() const = 0;
		[[nodiscard]] virtual vk::PrimitiveTopology GetTopology() const = 0;
	};
} // namespace Tkge::Graphics
