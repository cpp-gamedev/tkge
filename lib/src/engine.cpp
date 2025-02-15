#include <klib/assert.hpp>
#include <kvf/util.hpp>
#include <tkge/engine.hpp>

namespace tkge
{
Engine::Engine(WindowSurface const& surface, vk::SampleCountFlagBits const aa)
	: _window(CreateWindow(surface)), _renderDevice(_window.get()), _renderPass(&_renderDevice, aa)
{
}

glm::ivec2 Engine::FramebufferSize() const { return kvf::util::to_glm_vec<int>(_renderDevice.get_framebuffer_extent()); }

bool Engine::IsRunning() const { return glfwWindowShouldClose(_window.get()) == GLFW_FALSE; }

vk::CommandBuffer Engine::NextFrame()
{
	_cmd = _renderDevice.next_frame();
	return _cmd;
}

kvf::UniqueWindow Engine::CreateWindow(WindowSurface const& surface)
{
	auto ret = kvf::create_window(surface.size, surface.title, surface.decorated);
	KLIB_ASSERT(ret);
	glfwSetWindowUserPointer(ret.get(), this);
	// TODO
	// setGlfwCallbacks(ret.get());
	return ret;
}

void Engine::BeginRender(kvf::Color const clear)
{
	_renderPass.clear_color = clear.to_linear();
	_renderPass.begin_render(_cmd, _renderDevice.get_framebuffer_extent());
}

void Engine::EndRender()
{
	_renderPass.end_render();
	_renderDevice.render(_renderPass.render_target());
}
} // namespace tkge
