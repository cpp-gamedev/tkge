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
			CreateSetLayouts();
			CreatePipelineLayout();
		}

		[[nodiscard]] vk::PipelineLayout PipelineLayout() const { return *_pipelineLayout; }

		[[nodiscard]] std::span<const vk::DescriptorSetLayout> SetLayouts() const { return _setLayouts; }

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
		void CreateSetLayouts()
		{
			static constexpr auto StageFlags = vk::ShaderStageFlagBits::eAllGraphics;
			// set 0: builtin
			auto set0 = std::array<vk::DescriptorSetLayoutBinding, 3>{};
			// set 0, binding 0: view
			set0[0].setBinding(0).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eUniformBuffer).setStageFlags(StageFlags);
			// set 0, binding 1: instances
			set0[1].setBinding(1).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eStorageBuffer).setStageFlags(StageFlags);
			// set0, binding 2: texture
			set0[2].setBinding(2).setDescriptorCount(1).setDescriptorType(vk::DescriptorType::eCombinedImageSampler).setStageFlags(StageFlags);

			// TODO: set 1: user data

			auto dslci = std::array<vk::DescriptorSetLayoutCreateInfo, 1>{};
			dslci[0].setBindings(set0);

			for (const auto& createInfo : dslci) { _setLayoutStorage.push_back(_renderDevice->get_device().createDescriptorSetLayoutUnique(createInfo)); }
			for (const auto& setLayout : _setLayoutStorage) { _setLayouts.push_back(*setLayout); }
		}

		void CreatePipelineLayout()
		{
			auto plci = vk::PipelineLayoutCreateInfo{};
			plci.setSetLayouts(_setLayouts);
			_pipelineLayout = _renderDevice->get_device().createPipelineLayoutUnique(plci);
		}

		gsl::not_null<const kvf::RenderDevice*> _renderDevice;
		vk::SampleCountFlagBits _framebufferSamples;

		vk::UniquePipelineLayout _pipelineLayout{};
		std::vector<vk::UniqueDescriptorSetLayout> _setLayoutStorage{};
		std::vector<vk::DescriptorSetLayout> _setLayouts{};

		std::unordered_map<std::size_t, vk::UniquePipeline> _pipelines{};
	};
} // namespace Tkge::Detail
