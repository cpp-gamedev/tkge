#include <Tkge/Graphics/Renderer.hpp>
#include <kvf/render_device.hpp>
#include <kvf/util.hpp>
#include <algorithm>

namespace Tkge::Graphics
{
	Renderer::Renderer(kvf::RenderPass* renderPass, IResourcePool* resourcePool, const vk::CommandBuffer commandBuffer, const glm::ivec2 framebufferSize)
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

	void Renderer::Draw(const Primitive& primitive)
	{
		if (!IsRendering() || _shader == nullptr || primitive.vertices.empty()) { return; }

		const auto fixedState = PipelineFixedState{
			.colourFormat = _renderPass->get_color_format(),
			.topology = primitive.topology,
			.polygonMode = _polygonMode,
		};
		const auto pipeline = _resourcePool->GetPipeline(*_shader, fixedState);
		if (!pipeline) { return; }

		if (_pipeline != pipeline)
		{
			_pipeline = pipeline;
			_renderPass->bind_pipeline(_pipeline);
		}

		BindVboAndDraw(primitive);
	}

	void Renderer::BindVboAndDraw(const Primitive& primitive) const
	{
		const auto vertSize = primitive.vertices.size_bytes();
		const auto vboSize = vertSize + primitive.indices.size_bytes();
		auto& vertexBuffer = _resourcePool->AllocateBuffer(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer, vboSize);
		kvf::util::overwrite(vertexBuffer, primitive.vertices);
		if (!primitive.indices.empty()) { kvf::util::overwrite(vertexBuffer, primitive.indices, vertSize); }

		const auto commandBuffer = _renderPass->get_command_buffer();
		commandBuffer.setLineWidth(_lineWidth);

		commandBuffer.bindVertexBuffers(0, vertexBuffer.get_buffer(), vk::DeviceSize{});
		if (primitive.indices.empty()) { commandBuffer.draw(std::uint32_t(primitive.vertices.size()), 1, 0, 0); }
		else
		{
			commandBuffer.bindIndexBuffer(vertexBuffer.get_buffer(), vertSize, vk::IndexType::eUint32);
			commandBuffer.drawIndexed(std::uint32_t(primitive.indices.size()), 1, 0, 0, 0);
		}
	}
} // namespace Tkge::Graphics
