#pragma once
#include <kvf/render_device.hpp>
#include <kvf/vma.hpp>
#include <vulkan/vulkan_hash.hpp>

namespace Tkge::Detail
{
	/// \brief Cached storage for Vulkan Host Buffers.
	class BufferPool
	{
	  public:
		explicit BufferPool(gsl::not_null<kvf::RenderDevice*> renderDevice) : _renderDevice(renderDevice) {}

		// Allocate a Buffer for given usage and of given size.
		[[nodiscard]] kvf::vma::Buffer& Allocate(vk::BufferUsageFlags usage, vk::DeviceSize size)
		{
			const auto frameIndex = _renderDevice->get_frame_index();
			auto& poolMap = _poolMaps.at(std::size_t(frameIndex));
			auto& pool = poolMap[std::hash<vk::BufferUsageFlags>{}(usage)];
			if (pool.next < pool.buffers.size())
			{
				auto& ret = pool.buffers.at(pool.next++);
				ret.resize(size);
				return ret;
			}
			const auto createInfo = kvf::vma::BufferCreateInfo{
				.usage = usage,
				.type = kvf::vma::BufferType::Host,
			};
			pool.next = pool.buffers.size() + 1;
			return pool.buffers.emplace_back(_renderDevice, createInfo, size);
		}

		// Make all buffers for the current frame available for use.
		void NextFrame()
		{
			// GPU has finished rendering the current frame, these resources can now be reused.
			// (Other frames may still be being rendered, hence the multiple buffering.)
			const auto frameIndex = _renderDevice->get_frame_index();
			auto& poolMap = _poolMaps.at(std::size_t(frameIndex));
			for (auto& [_, pool] : poolMap) { pool.next = 0; }
		}

	  private:
		struct Pool
		{
			std::vector<kvf::vma::Buffer> buffers{}; // buffer pool for a specific usage
			std::size_t next{};						 // index of next available buffer
		};

		using PoolMap = std::unordered_map<std::size_t, Pool>; // map of hash(usage) => Pool

		gsl::not_null<kvf::RenderDevice*> _renderDevice;

		kvf::Buffered<PoolMap> _poolMaps{}; // double/triple/etc buffered pools
	};
} // namespace Tkge::Detail
