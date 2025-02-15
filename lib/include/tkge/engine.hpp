#pragma once
#include <glm/vec2.hpp>
#include <klib/c_string.hpp>
#include <kvf/color.hpp>
#include <kvf/render_device.hpp>
#include <kvf/render_pass.hpp>
#include <kvf/window.hpp>

namespace tkge
{
struct WindowSurface
{
	glm::ivec2 size{600};
	klib::CString title{"tkge"};
	bool decorated{true};
};

class Engine
{
  public:
	static constexpr auto AntiAliasing = vk::SampleCountFlagBits::e2;

	explicit Engine(WindowSurface const& surface = {}, vk::SampleCountFlagBits aa = AntiAliasing);

	[[nodiscard]] glm::ivec2 FramebufferSize() const;
	[[nodiscard]] bool IsRunning() const;

	vk::CommandBuffer NextFrame();
	// TODO: return Renderer
	void BeginRender(kvf::Color clear = kvf::black_v);
	void EndRender();

  private:
	[[nodiscard]] kvf::UniqueWindow CreateWindow(WindowSurface const& surface);

	kvf::UniqueWindow _window;
	kvf::RenderDevice _renderDevice;
	kvf::RenderPass _renderPass;

	vk::CommandBuffer _cmd{};
};
} // namespace tkge
