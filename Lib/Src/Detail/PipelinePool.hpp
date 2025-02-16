#pragma once
#include <Tkge/Graphics/PipelineFixedState.hpp>
#include <Tkge/Graphics/Shader.hpp>
#include <Tkge/Graphics/Vertex.hpp>
#include <klib/hash_combine.hpp>
#include <kvf/render_device.hpp>
#include <unordered_map>

namespace Tkge::Detail
{
	/// \brief Cached storage for Vulkan Graphics Pipelines.
	class PipelinePool
	{
	  public:
		using FixedState = Graphics::PipelineFixedState;

		/// \param renderDevice Pointer to valid RenderDevice.
		/// \param framebufferSamples Sample count of target Render Pass' colour attachment.
		explicit PipelinePool(gsl::not_null<const kvf::RenderDevice*> renderDevice, vk::SampleCountFlagBits framebufferSamples)
			: _renderDevice(renderDevice), _framebufferSamples(framebufferSamples)
		{
			// TODO: descriptor set layouts
			_pipelineLayout = renderDevice->get_device().createPipelineLayoutUnique({});
		}

		[[nodiscard]] vk::PipelineLayout PipelineLayout() const { return *_pipelineLayout; }

		/// \brief Get the Pipeline identified by the input parameters.
		/// \param shader Shader that will be used in draw calls (dynamic Pipeline state).
		/// \param state Fixed Pipeline state.
		/// \returns Existing Pipeline if already cached, otherwise a newly created one (unless creation fails).
		[[nodiscard]] vk::Pipeline GetPipeline(const Graphics::Shader& shader, const FixedState& state)
		{
			using Graphics::Vertex;

			// A single Vertex Buffer is bound during draw (index 0), containing vertices.
			static constexpr auto VertexBindings = std::array{
				vk::VertexInputBindingDescription{0, sizeof(Vertex)},
			};

			// Attributes for Vertex Buffer at index 0: list of interleaved vertices (span<Vertex>).
			static constexpr auto VertexAttributes = std::array{
				vk::VertexInputAttributeDescription{0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position)},
				vk::VertexInputAttributeDescription{1, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, colour)},
				vk::VertexInputAttributeDescription{2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
			};

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

	  private:
		gsl::not_null<const kvf::RenderDevice*> _renderDevice;
		vk::SampleCountFlagBits _framebufferSamples;

		vk::UniquePipelineLayout _pipelineLayout{};
		std::unordered_map<std::size_t, vk::UniquePipeline> _pipelines{};
	};
} // namespace Tkge::Detail
