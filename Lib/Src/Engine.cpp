#include <Tkge/Engine.hpp>
#include <klib/assert.hpp>
#include <kvf/is_positive.hpp>
#include <kvf/util.hpp>
#include <print>

namespace Tkge
{
	Engine::Engine(const WindowSurface& surface, const vk::SampleCountFlagBits aa)
		: _window(CreateWindow(surface)), _renderDevice(_window.get()), _renderPass(&_renderDevice, aa), _resourcePool(&_renderDevice, aa)
	{
		_renderPass.set_color_target();
	}

	glm::ivec2 Engine::FramebufferSize() const { return kvf::util::to_glm_vec<int>(_renderDevice.get_framebuffer_extent()); }

	bool Engine::IsRunning() const { return glfwWindowShouldClose(_window.get()) == GLFW_FALSE; }

	vk::CommandBuffer Engine::NextFrame()
	{
		_cmd = _renderDevice.next_frame();
		return _cmd;
	}

	kvf::UniqueWindow Engine::CreateWindow(const WindowSurface& surface)
	{
		auto ret = kvf::create_window(surface.size, surface.title, surface.decorated);
		KLIB_ASSERT(ret);
		glfwSetWindowUserPointer(ret.get(), this);
		// TODO
		// setGlfwCallbacks(ret.get());
		return ret;
	}

	graphics::Renderer Engine::BeginRender(const kvf::Color clear)
	{
		if (!_cmd) { return {}; }

		const auto framebufferSize = FramebufferSize();
		if (!kvf::is_positive(framebufferSize)) { return {}; }

		_renderPass.clear_color = clear.to_linear();
		return graphics::Renderer{&_renderPass, &_resourcePool, _cmd, framebufferSize};
	}

	void Engine::Present()
	{
		if (_renderPass.get_command_buffer()) { _renderPass.end_render(); }
		_renderDevice.render(_renderPass.render_target());
	}
} // namespace Tkge
