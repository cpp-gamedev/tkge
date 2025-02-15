#pragma once
#include <vulkan/vulkan.hpp>
#include <cstdint>
#include <span>

namespace tkge::graphics
{
class Shader
{
  public:
	[[nodiscard]] bool IsLoaded() const { return _vertex && _fragment; }
	bool Load(vk::Device device, std::span<std::uint32_t const> vertCode, std::span<std::uint32_t const> fragCode);

	[[nodiscard]] vk::ShaderModule VertexModule() const { return *_vertex; }
	[[nodiscard]] vk::ShaderModule FragmentModule() const { return *_fragment; }
	[[nodiscard]] std::size_t GetHash() const { return _hash; }

  private:
	vk::UniqueShaderModule _vertex{};
	vk::UniqueShaderModule _fragment{};
	std::size_t _hash{};
};
} // namespace tkge::graphics
