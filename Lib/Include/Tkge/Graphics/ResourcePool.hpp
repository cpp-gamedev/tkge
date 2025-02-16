#pragma once
#include <Tkge/graphics/Shader.hpp>
#include <kvf/render_device.hpp>
#include <unordered_map>

namespace Tkge::Graphics
{
	class PipelinePool
	{
	  public:
		struct Params
		{
			vk::Format colourFormat{};

			vk::PrimitiveTopology topology{vk::PrimitiveTopology::eTriangleList};
			vk::PolygonMode polygonMode{vk::PolygonMode::eFill};
		};

		explicit PipelinePool(gsl::not_null<const kvf::RenderDevice*> renderDevice, vk::SampleCountFlagBits framebufferSamples);

		[[nodiscard]] vk::PipelineLayout PipelineLayout() const { return *_pipelineLayout; }

		[[nodiscard]] vk::Pipeline GetPipeline(const Shader& shader, const Params& params);

	  private:
		gsl::not_null<const kvf::RenderDevice*> _renderDevice;
		vk::SampleCountFlagBits _framebufferSamples;

		vk::UniquePipelineLayout _pipelineLayout{};
		std::unordered_map<std::size_t, vk::UniquePipeline> _pipelines{};
	};

	class ResourcePool
	{
	  public:
		explicit ResourcePool(gsl::not_null<const kvf::RenderDevice*> renderDevice, vk::SampleCountFlagBits framebufferSamples)
			: pipelinePool(renderDevice, framebufferSamples)
		{
		}

		PipelinePool pipelinePool;
	};
} // namespace Tkge::Graphics
