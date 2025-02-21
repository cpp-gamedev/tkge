#include <Detail/BufferPool.hpp>
#include <Detail/PipelinePool.hpp>
#include <Detail/SamplerPool.hpp>
#include <Tkge/Engine.hpp>
#include <klib/assert.hpp>
#include <kvf/is_positive.hpp>
#include <kvf/util.hpp>
#include <print>

namespace Tkge
{
	namespace
	{
		struct PixelBitmap
		{
			[[nodiscard]] constexpr kvf::Bitmap ToBitmap() const { return kvf::Bitmap{.bytes = bytes, .size = {1, 1}}; }

			std::array<std::byte, 4> bytes{};
		};

		constexpr auto WhiteBitmap = std::bit_cast<PixelBitmap>(kvf::white_v);

		class ResourcePool : public Graphics::IResourcePool
		{
		  public:
			explicit ResourcePool(gsl::not_null<kvf::RenderDevice*> renderDevice, vk::SampleCountFlagBits framebufferSamples)
				: _pipelinePool(renderDevice, framebufferSamples), _bufferPool(renderDevice), _samplerPool(renderDevice), _whiteTexture(renderDevice)
			{
				_whiteTexture.Create(WhiteBitmap.ToBitmap());
			}

			[[nodiscard]] vk::PipelineLayout PipelineLayout() const final { return _pipelinePool.PipelineLayout(); }

			[[nodiscard]] std::span<const vk::DescriptorSetLayout> SetLayouts() const final { return _pipelinePool.SetLayouts(); }

			[[nodiscard]] vk::Pipeline GetPipeline(const Graphics::Shader& shader, const Graphics::PipelineFixedState& state) final
			{
				return _pipelinePool.GetPipeline(shader, state);
			}

			[[nodiscard]] Graphics::Buffer& AllocateBuffer(const vk::BufferUsageFlags usage, const vk::DeviceSize size) final
			{
				return _bufferPool.Allocate(usage, size);
			}

			[[nodiscard]] vk::Sampler GetSampler(const Graphics::TextureSampler& sampler) final { return _samplerPool.GetSampler(sampler); }
			[[nodiscard]] const Graphics::Texture& GetFallbackTexture() const final { return _whiteTexture; }

			void NextFrame() { _bufferPool.NextFrame(); }

		  private:
			Detail::PipelinePool _pipelinePool;
			Detail::BufferPool _bufferPool;
			Detail::SamplerPool _samplerPool;
			Graphics::Texture _whiteTexture;
		};
	} // namespace

	Engine::Engine(const WindowSurface& surface, const vk::SampleCountFlagBits aa)
		: _window(CreateWindow(surface)), _renderDevice(_window.get()), _renderPass(&_renderDevice, aa),
		  _resourcePool(std::make_unique<ResourcePool>(&_renderDevice, aa))
	{
		_renderPass.set_color_target();
	}

	glm::ivec2 Engine::FramebufferSize() const { return kvf::util::to_glm_vec<int>(_renderDevice.get_framebuffer_extent()); }

	bool Engine::IsRunning() const { return glfwWindowShouldClose(_window.get()) == GLFW_FALSE; }

	vk::CommandBuffer Engine::NextFrame()
	{
		_cmd = _renderDevice.next_frame();
		static_cast<ResourcePool*>(_resourcePool.get())->NextFrame(); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
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

	Graphics::Renderer Engine::BeginRender(const kvf::Color clear)
	{
		if (!_cmd) { return {}; }

		const auto framebufferSize = FramebufferSize();
		if (!kvf::is_positive(framebufferSize)) { return {}; }

		_renderPass.clear_color = clear.to_linear();
		return Graphics::Renderer{&_renderPass, _resourcePool.get(), _cmd, framebufferSize};
	}

	void Engine::Present()
	{
		if (_renderPass.get_command_buffer()) { _renderPass.end_render(); }
		_renderDevice.render(_renderPass.render_target());
	}
} // namespace Tkge
