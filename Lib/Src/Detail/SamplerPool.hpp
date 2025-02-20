#pragma once
#include <Tkge/Graphics/TextureSampler.hpp>
#include <klib/hash_combine.hpp>
#include <kvf/render_device.hpp>
#include <vulkan/vulkan_hash.hpp>
#include <unordered_map>

namespace Tkge::Detail
{
	class SamplerPool
	{
	  public:
		explicit SamplerPool(gsl::not_null<const kvf::RenderDevice*> renderDevice) : _renderDevice(renderDevice) {}

		[[nodiscard]] vk::Sampler GetSampler(const Graphics::TextureSampler& in)
		{
			const auto key = GetHash(in);
			auto it = _samplers.find(key);
			if (it != _samplers.end()) { return *it->second; }

			auto sci = _renderDevice->sampler_info(in.wrap, in.filter);
			sci.setMipmapMode(in.mipMap).setBorderColor(in.borderColour);
			it = _samplers.insert({key, _renderDevice->get_device().createSamplerUnique(sci)}).first;
			return *it->second;
		}

	  private:
		[[nodiscard]] static std::size_t GetHash(const Graphics::TextureSampler& sampler)
		{
			return klib::make_combined_hash(sampler.wrap, sampler.filter, sampler.mipMap, sampler.borderColour);
		}

		gsl::not_null<const kvf::RenderDevice*> _renderDevice;

		std::unordered_map<std::size_t, vk::UniqueSampler> _samplers{};
	};
} // namespace Tkge::Detail
