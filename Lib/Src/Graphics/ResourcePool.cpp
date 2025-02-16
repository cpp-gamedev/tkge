#include <klib/hash_combine.hpp>
#include <Tkge/Graphics/ResourcePool.hpp>
#include <vulkan/vulkan_hash.hpp>

namespace Tkge::Graphics
{
	PipelinePool::PipelinePool(gsl::not_null<const kvf::RenderDevice*> renderDevice, const vk::SampleCountFlagBits framebufferSamples)
		: _renderDevice(renderDevice), _framebufferSamples(framebufferSamples)
	{
		// TODO: descriptor set layouts
		_pipelineLayout = renderDevice->get_device().createPipelineLayoutUnique({});
	}

	vk::Pipeline PipelinePool::GetPipeline(const Shader& shader, const Params& params)
	{
		const auto key = klib::make_combined_hash(shader.GetHash(), params.colourFormat, params.polygonMode, params.topology);
		auto it = _pipelines.find(key);
		if (it != _pipelines.end()) { return *it->second; }

		const auto pipelineState = kvf::PipelineState{
			.vertex_bindings = {},
			.vertex_attributes = {},
			.vertex_shader = shader.VertexModule(),
			.fragment_shader = shader.FragmentModule(),
			.topology = params.topology,
			.polygon_mode = params.polygonMode,
		};
		const auto pipelineFormat = kvf::PipelineFormat{
			.samples = _framebufferSamples,
			.color = params.colourFormat,
		};
		auto ret = _renderDevice->create_pipeline(*_pipelineLayout, pipelineState, pipelineFormat);
		if (!ret) { return {}; }

		it = _pipelines.insert({key, std::move(ret)}).first;
		return *it->second;
	}

} // namespace Tkge::Graphics
