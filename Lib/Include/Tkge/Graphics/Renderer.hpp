#pragma once
#include <Tkge/Graphics/Primitive.hpp>
#include <Tkge/Graphics/RenderInstance.hpp>
#include <Tkge/Graphics/ResourcePool.hpp>
#include <Tkge/Transform.hpp>
#include <kvf/render_pass.hpp>

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

		explicit Renderer(kvf::RenderPass* renderPass, IResourcePool* resourcePool, vk::CommandBuffer commandBuffer, glm::ivec2 framebufferSize);
		~Renderer() { EndRender(); }

		[[nodiscard]] bool IsRendering() const { return _renderPass != nullptr; }
		void EndRender();

		bool BindShader(const Shader& shader);
		void SetLineWidth(float width);
		void SetWireframe(bool wireframe);

		void Draw(const Primitive& primitive, std::span<const RenderInstance> instances);

		explicit operator bool() const { return IsRendering(); }

		Transform view{};

	  private:
		struct Std430Instance
		{
			glm::mat4 model;
			glm::vec4 tint;
		};

		void UpdateInstances(std::span<const RenderInstance> instances);
		[[nodiscard]] bool WriteSets(const Texture* texture) const;
		void BindVboAndDraw(const Primitive& primitive, std::uint32_t instances) const;

		kvf::RenderPass* _renderPass{};
		IResourcePool* _resourcePool{};

		vk::Viewport _viewport{};

		const Shader* _shader{};
		vk::Pipeline _pipeline{};
		vk::PolygonMode _polygonMode{vk::PolygonMode::eFill};
		float _lineWidth{1.0f};
		std::vector<Std430Instance> _instances{};
	};
} // namespace Tkge::Graphics
