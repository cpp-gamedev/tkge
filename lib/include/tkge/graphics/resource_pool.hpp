#pragma once
#include <kvf/vma.hpp>
#include <tkge/graphics/pipeline_fixed_state.hpp>
#include <tkge/graphics/shader.hpp>

namespace tkge::graphics
{
	using Buffer = kvf::vma::Buffer;

	class IResourcePool : public klib::Polymorphic
	{
	  public:
		[[nodiscard]] virtual vk::PipelineLayout PipelineLayout() const = 0;

		/// \brief Get the Pipeline identified by the input parameters.
		/// \param shader Shader that will be used in draw calls (dynamic Pipeline state).
		/// \param state Fixed Pipeline state.
		/// \returns Existing Pipeline if already cached, otherwise a newly created one (unless creation fails).
		[[nodiscard]] virtual vk::Pipeline GetPipeline(const Shader& shader, const PipelineFixedState& state) = 0;

		/// \brief Allocate a Buffer for given usage and of given size.
		[[nodiscard]] virtual Buffer& AllocateBuffer(vk::BufferUsageFlags usage, vk::DeviceSize size) = 0;
	};
} // namespace tkge::graphics
