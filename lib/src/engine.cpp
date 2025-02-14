#include <klib/assert.hpp>
#include <kvf/util.hpp>
#include <tkge/engine.hpp>

namespace tkge
{
Engine::Engine(WindowSurface const& surface, vk::SampleCountFlagBits const aa)
	: m_window(createWindow(surface)), m_render_device(m_window.get()), m_render_pass(&m_render_device, aa)
{
}

auto Engine::framebufferSize() const -> glm::ivec2 { return kvf::util::to_glm_vec<int>(m_render_device.get_framebuffer_extent()); }

auto Engine::isRunning() const -> bool { return glfwWindowShouldClose(m_window.get()) == GLFW_FALSE; }

vk::CommandBuffer Engine::nextFrame()
{
	m_cmd = m_render_device.next_frame();
	return m_cmd;
}

auto Engine::createWindow(WindowSurface const& surface) -> kvf::UniqueWindow
{
	auto ret = kvf::create_window(surface.size, surface.title, surface.decorated);
	KLIB_ASSERT(ret);
	glfwSetWindowUserPointer(ret.get(), this);
	// TODO
	// setGlfwCallbacks(ret.get());
	return ret;
}

void Engine::beginRender(kvf::Color const clear)
{
	m_render_pass.clear_color = clear.to_linear();
	m_render_pass.begin_render(m_cmd, m_render_device.get_framebuffer_extent());
}

void Engine::endRender()
{
	m_render_pass.end_render();
	m_render_device.render(m_render_pass.render_target());
}
} // namespace tkge
