#pragma once
#include <glm/vec2.hpp>
#include <klib/c_string.hpp>
#include <kvf/color.hpp>
#include <kvf/render_device.hpp>
#include <kvf/render_pass.hpp>
#include <kvf/window.hpp>
#include "assetLoader.hpp"

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

		explicit Engine(const WindowSurface& surface = {}, vk::SampleCountFlagBits aa = AntiAliasing);

		[[nodiscard]] const kvf::RenderDevice& RenderDevice() const { return _renderDevice; }
		[[nodiscard]] const kvf::RenderPass& RenderPass() const { return _renderPass; }

		[[nodiscard]] glm::ivec2 FramebufferSize() const;
		[[nodiscard]] auto FramebufferFormat() const -> vk::Format { return _renderPass.get_color_format(); }
		[[nodiscard]] auto FramebufferSamples() const -> vk::SampleCountFlagBits { return _renderPass.get_samples(); }

		[[nodiscard]] bool IsRunning() const;
		vk::CommandBuffer NextFrame();
		// TODO: return Renderer
		void BeginRender(kvf::Color clear = kvf::black_v);
		void EndRender();

		[[nodiscard]] AssetLoader& GetAssetLoader() noexcept { return this->_assetLoader; }
		[[nodiscard]] const AssetLoader& GetAssetLoader() const noexcept { return this->_assetLoader; }

	  private:
		[[nodiscard]] kvf::UniqueWindow CreateWindow(const WindowSurface& surface);

		kvf::UniqueWindow _window;
		kvf::RenderDevice _renderDevice;
		kvf::RenderPass _renderPass;

		vk::CommandBuffer _cmd{};
		AssetLoader _assetLoader;
	};
} // namespace tkge
