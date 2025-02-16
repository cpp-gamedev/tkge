#pragma once
#include <kvf/render_device.hpp>
#include <kvf/vma.hpp>
#include <tkge/graphics/shader.hpp>
#include <unordered_map>
#include <vector>

namespace tkge::graphics
{
	/// \brief Cached storage for Vulkan Graphics Pipelines.
	class PipelinePool
	{
	  public:
		/// \brief Description of a unique Pipeline's fixed state.
		struct State
		{
			/// \brief Colour image format of target Render Pass.
			vk::Format colourFormat{};

			vk::PrimitiveTopology topology{vk::PrimitiveTopology::eTriangleList};
			vk::PolygonMode polygonMode{vk::PolygonMode::eFill};
		};

		/// \param renderDevice Pointer to valid RenderDevice.
		/// \param framebufferSamples Sample count of target Render Pass' colour attachment.
		explicit PipelinePool(gsl::not_null<const kvf::RenderDevice*> renderDevice, vk::SampleCountFlagBits framebufferSamples);

		[[nodiscard]] vk::PipelineLayout PipelineLayout() const { return *_pipelineLayout; }

		/// \brief Get the Pipeline identified by the input parameters.
		/// \param shader Shader that will be used in draw calls (dynamic Pipeline state).
		/// \param state Fixed Pipeline state.
		/// \returns Existing Pipeline if already cached, otherwise a newly created one (unless creation fails).
		[[nodiscard]] vk::Pipeline GetPipeline(const Shader& shader, const State& state);

	  private:
		gsl::not_null<const kvf::RenderDevice*> _renderDevice;
		vk::SampleCountFlagBits _framebufferSamples;

		vk::UniquePipelineLayout _pipelineLayout{};
		std::unordered_map<std::size_t, vk::UniquePipeline> _pipelines{};
	};

	/// \brief Cached storage for Vulkan Host Buffers.
	class BufferPool
	{
	  public:
		explicit BufferPool(gsl::not_null<kvf::RenderDevice*> renderDevice) : _renderDevice(renderDevice) {}

		/// \brief Allocate a Buffer for given usage and of given size.
		[[nodiscard]] kvf::vma::Buffer& Allocate(vk::BufferUsageFlags usage, vk::DeviceSize size);

		/// \brief Make all buffers for the current frame available for use.
		void NextFrame();

	  private:
		struct Pool
		{
			std::vector<kvf::vma::Buffer> buffers{}; // buffer pool for a specific usage
			std::size_t next{};						 // index of next available buffer
		};

		using PoolMap = std::unordered_map<std::size_t, Pool>; // map of hash(usage) => Pool

		gsl::not_null<kvf::RenderDevice*> _renderDevice;

		kvf::Buffered<PoolMap> _poolMaps{}; // double/triple/etc buffered pools
	};

	class ResourcePool
	{
	  public:
		explicit ResourcePool(gsl::not_null<kvf::RenderDevice*> renderDevice, vk::SampleCountFlagBits framebufferSamples)
			: pipelinePool(renderDevice, framebufferSamples), bufferPool(renderDevice)
		{
		}

		void NextFrame();

		PipelinePool pipelinePool;
		BufferPool bufferPool;
	};
} // namespace tkge::graphics
