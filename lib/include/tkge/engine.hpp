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

		[[nodiscard]] glm::ivec2 framebufferSize() const;
		[[nodiscard]] bool isRunning() const;

		vk::CommandBuffer nextFrame();
		// TODO: return Renderer
		void beginRender(kvf::Color clear = kvf::black_v);
		void endRender();

		[[nodiscard]] AssetLoader& GetAssetLoader() noexcept { return this->_assetLoader; }
		[[nodiscard]] const AssetLoader& GetAssetLoader() const noexcept { return this->_assetLoader; }

	  private:
		[[nodiscard]] kvf::UniqueWindow createWindow(const WindowSurface& surface);

		kvf::UniqueWindow _window;
		kvf::RenderDevice _renderDevice;
		kvf::RenderPass _renderPass;

		vk::CommandBuffer _cmd{};

		AssetLoader _assetLoader;
	};
} // namespace tkge
