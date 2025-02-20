#pragma once
#include <vulkan/vulkan.hpp>

namespace Tkge::Graphics
{
	struct TextureSampler
	{
		vk::SamplerAddressMode wrap{vk::SamplerAddressMode::eRepeat};
		vk::Filter filter{vk::Filter::eLinear};
		vk::SamplerMipmapMode mipMap{vk::SamplerMipmapMode::eNearest};
		vk::BorderColor borderColour{vk::BorderColor::eFloatTransparentBlack};

		bool operator==(const TextureSampler&) const = default;
	};
} // namespace Tkge::Graphics
