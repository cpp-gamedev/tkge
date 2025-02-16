#pragma once
#include <vulkan/vulkan.hpp>

namespace tkge::graphics
{
	/// \brief Description of a unique Pipeline's fixed state.
	struct PipelineFixedState
	{
		/// \brief Colour image format of target Render Pass.
		vk::Format colourFormat{};

		vk::PrimitiveTopology topology{vk::PrimitiveTopology::eTriangleList};
		vk::PolygonMode polygonMode{vk::PolygonMode::eFill};
	};
} // namespace tkge::graphics
