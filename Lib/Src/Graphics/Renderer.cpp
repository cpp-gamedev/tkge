#include <Tkge/Graphics/Renderer.hpp>
#include <kvf/util.hpp>
#include <algorithm>

namespace Tkge::Graphics
{
	Renderer::Renderer(kvf::RenderPass* renderPass, ResourcePool* resourcePool, const vk::CommandBuffer commandBuffer, const glm::ivec2 framebufferSize)
		: _renderPass(renderPass), _resourcePool(resourcePool)
	{
		_renderPass->begin_render(commandBuffer, kvf::util::to_vk_extent(framebufferSize));
	}

	void Renderer::EndRender()
	{
		if (_renderPass == nullptr) { return; }
		_renderPass->end_render();
		_renderPass = nullptr;
	}

	bool Renderer::BindShader(const Shader& shader)
	{
		if (!IsRendering() || !shader.IsLoaded()) { return false; }
		_shader = &shader;
		return true;
	}

	void Renderer::SetWireframe(const bool wireframe) { _polygonMode = wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill; }

	void Renderer::SetLineWidth(const float width)
	{
		if (_renderPass == nullptr) { return; }
		const auto limits = _renderPass->get_render_device().get_gpu().properties.limits.lineWidthRange;
		_lineWidth = std::clamp(width, limits[0], limits[1]);
	}

	void Renderer::Draw(const std::uint32_t vertices)
	{
		if (!IsRendering() || _shader == nullptr) { return; }

		const auto pipelineParams = PipelinePool::Params{
			.colourFormat = _renderPass->get_color_format(),
			.polygonMode = _polygonMode,
		};
		const auto pipeline = _resourcePool->pipelinePool.GetPipeline(*_shader, pipelineParams);
		if (!pipeline) { return; }

		if (_pipeline != pipeline)
		{
			_pipeline = pipeline;
			_renderPass->bind_pipeline(_pipeline);
		}

		const auto commandBuffer = _renderPass->get_command_buffer();
		commandBuffer.setLineWidth(_lineWidth);
		commandBuffer.draw(vertices, 1, 0, 0);
	}
} // namespace Tkge::Graphics
