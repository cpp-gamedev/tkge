#pragma once
#include <kvf/render_pass.hpp>
#include <Tkge/graphics/ResourcePool.hpp>

namespace Tkge::Graphics
{
	class Renderer
	{
	  public:
		Renderer(Renderer&&) = delete;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;

		Renderer() = default;

		explicit Renderer(kvf::RenderPass* renderPass, ResourcePool* resourcePool, vk::CommandBuffer commandBuffer, glm::ivec2 framebufferSize);
		~Renderer() { EndRender(); }

		[[nodiscard]] bool IsRendering() const { return _renderPass != nullptr; }
		void EndRender();

		bool BindShader(const Shader& shader);
		void SetLineWidth(float width);
		void SetWireframe(bool wireframe);

		// temporary, until we have vertices, primitives, etc
		void Draw(std::uint32_t vertices);

		explicit operator bool() const { return IsRendering(); }

	  private:
		kvf::RenderPass* _renderPass{};
		ResourcePool* _resourcePool{};

		const Shader* _shader{};
		vk::Pipeline _pipeline{};
		vk::PolygonMode _polygonMode{vk::PolygonMode::eFill};
		float _lineWidth{1.0f};
	};
} // namespace Tkge::Graphics
