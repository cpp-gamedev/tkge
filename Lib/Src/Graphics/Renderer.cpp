#include <Tkge/Graphics/Renderer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <klib/assert.hpp>
#include <klib/flex_array.hpp>
#include <kvf/render_device.hpp>
#include <kvf/util.hpp>
#include <algorithm>

namespace Tkge::Graphics
{
	Renderer::Renderer(kvf::RenderPass* renderPass, IResourcePool* resourcePool, const vk::CommandBuffer commandBuffer, const glm::ivec2 framebufferSize)
		: _renderPass(renderPass), _resourcePool(resourcePool)
	{
		_renderPass->begin_render(commandBuffer, kvf::util::to_vk_extent(framebufferSize));
		_viewport = _renderPass->to_viewport(kvf::uv_rect_v);
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

		if (!WriteSets()) { return; }

		_renderPass->get_command_buffer().setViewport(0, _viewport);

		BindVboAndDraw(primitive);
	}

	bool Renderer::WriteSets() const
	{
		auto& renderDevice = _renderPass->get_render_device();

		const auto setLayouts = _resourcePool->SetLayouts();
		auto descriptorSets = std::array<vk::DescriptorSet, 1>{};
		KLIB_ASSERT(setLayouts.size() == descriptorSets.size()); // expected set count will change until render flow is stable
		if (!renderDevice.allocate_sets(descriptorSets, setLayouts)) { return false; }

		auto bufferInfos = klib::FlexArray<vk::DescriptorBufferInfo, 4>{};
		auto descriptorWrites = klib::FlexArray<vk::WriteDescriptorSet, 8>{};
		const auto pushBufferWrite = [&](vk::DescriptorSet set, std::uint32_t binding, const Buffer& buffer, const vk::DescriptorType type)
		{
			bufferInfos.push_back({});
			auto& dbi = bufferInfos.back();
			dbi.setBuffer(buffer.get_buffer()).setRange(buffer.get_size());
			auto wds = vk::WriteDescriptorSet{};
			wds.setBufferInfo(dbi).setDescriptorCount(1).setDescriptorType(type).setDstSet(set).setDstBinding(binding);
			descriptorWrites.push_back(wds);
		};

		auto& ubo00 = _resourcePool->AllocateBuffer(vk::BufferUsageFlagBits::eUniformBuffer, sizeof(glm::mat4));
		const auto halfRenderArea = 0.5f * glm::vec2{_viewport.width, -_viewport.height};
		const auto matProj = glm::ortho(-halfRenderArea.x, halfRenderArea.x, -halfRenderArea.y, halfRenderArea.y);
		const auto matVP = matProj * view.ToView();
		kvf::util::overwrite(ubo00, matVP);
		pushBufferWrite(descriptorSets[0], 0, ubo00, vk::DescriptorType::eUniformBuffer);

		const auto writeSpan = std::span{descriptorWrites.data(), descriptorWrites.size()};
		renderDevice.get_device().updateDescriptorSets(writeSpan, {});

		const auto setSpan = std::span{descriptorSets.data(), descriptorSets.size()};
		_renderPass->get_command_buffer().bindDescriptorSets(vk::PipelineBindPoint::eGraphics, _resourcePool->PipelineLayout(), 0, setSpan, {});

		return true;
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
