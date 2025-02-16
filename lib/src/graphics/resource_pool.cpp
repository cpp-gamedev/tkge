#include <klib/hash_combine.hpp>
#include <tkge/graphics/resource_pool.hpp>
#include <tkge/graphics/vertex.hpp>
#include <vulkan/vulkan_hash.hpp>
#include <array>

namespace tkge::graphics
{
	namespace
	{
		// A single Vertex Buffer is bound during draw (index 0), containing vertices.
		constexpr auto VertexBindings = std::array{
			vk::VertexInputBindingDescription{0, sizeof(Vertex)},
		};

		// Attributes for Vertex Buffer at index 0: list of interleaved vertices (span<Vertex>).
		constexpr auto VertexAttributes = std::array{
			vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)},
			vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, colour)},
			vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
		};
	} // namespace

	// Pipelines do not need to be multiple buffered.
	PipelinePool::PipelinePool(gsl::not_null<const kvf::RenderDevice*> renderDevice, const vk::SampleCountFlagBits framebufferSamples)
		: _renderDevice(renderDevice), _framebufferSamples(framebufferSamples)
	{
		// TODO: descriptor set layouts
		_pipelineLayout = renderDevice->get_device().createPipelineLayoutUnique({});
	}

	vk::Pipeline PipelinePool::GetPipeline(const Shader& shader, const State& state)
	{
		const auto key = klib::make_combined_hash(shader.GetHash(), state.colourFormat, state.polygonMode, state.topology);
		auto it = _pipelines.find(key);
		if (it != _pipelines.end()) { return *it->second; }

		const auto pipelineState = kvf::PipelineState{
			.vertex_bindings = VertexBindings,
			.vertex_attributes = VertexAttributes,
			.vertex_shader = shader.VertexModule(),
			.fragment_shader = shader.FragmentModule(),
			.topology = state.topology,
			.polygon_mode = state.polygonMode,
		};
		const auto pipelineFormat = kvf::PipelineFormat{
			.samples = _framebufferSamples,
			.color = state.colourFormat,
		};
		auto ret = _renderDevice->create_pipeline(*_pipelineLayout, pipelineState, pipelineFormat);
		if (!ret) { return {}; }

		it = _pipelines.insert({key, std::move(ret)}).first;
		return *it->second;
	}

	// This pattern enables reusing previously allocated buffers instead of
	// allocating fresh ones for each draw.
	kvf::vma::Buffer& BufferPool::Allocate(const vk::BufferUsageFlags usage, const vk::DeviceSize size)
	{
		const auto frameIndex = _renderDevice->get_frame_index();
		auto& poolMap = _poolMaps.at(std::size_t(frameIndex));
		auto& pool = poolMap[std::hash<vk::BufferUsageFlags>{}(usage)];
		if (pool.next < pool.buffers.size())
		{
			auto& ret = pool.buffers.at(pool.next++);
			ret.resize(size);
			return ret;
		}
		const auto createInfo = kvf::vma::BufferCreateInfo{
			.usage = usage,
			.type = kvf::vma::BufferType::Host,
		};
		pool.next = pool.buffers.size() + 1;
		return pool.buffers.emplace_back(_renderDevice, createInfo, size);
	}

	void BufferPool::NextFrame()
	{
		// GPU has finished rendering the current frame, these resources can now be reused.
		// (Other frames may still be being rendered, hence the multiple buffering.)
		const auto frameIndex = _renderDevice->get_frame_index();
		auto& poolMap = _poolMaps.at(std::size_t(frameIndex));
		for (auto& [_, pool] : poolMap) { pool.next = 0; }
	}

	void ResourcePool::NextFrame() { bufferPool.NextFrame(); }

} // namespace tkge::graphics
