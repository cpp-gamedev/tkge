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

	[[nodiscard]] auto framebufferSize() const -> glm::ivec2;
	[[nodiscard]] auto isRunning() const -> bool;

	vk::CommandBuffer nextFrame();
	// TODO: return Renderer
	void beginRender(kvf::Color clear = kvf::black_v);
	void endRender();

  private:
	[[nodiscard]] auto createWindow(WindowSurface const& surface) -> kvf::UniqueWindow;

	kvf::UniqueWindow m_window;
	kvf::RenderDevice m_render_device;
	kvf::RenderPass m_render_pass;

	vk::CommandBuffer m_cmd{};
};
} // namespace tkge
