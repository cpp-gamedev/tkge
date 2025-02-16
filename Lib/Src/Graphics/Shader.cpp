#include <klib/hash_combine.hpp>
#include <Tkge/Graphics/Shader.hpp>
#include <array>

namespace Tkge::graphics
{
	namespace
	{
		void CombineHash(std::size_t& outHash, std::span<const std::uint32_t> spirV)
		{
			for (const auto code : spirV) { klib::hash_combine(outHash, code); }
		}

		[[nodiscard]] std::size_t ComputeHash(std::span<const std::uint32_t> vertCode, std::span<const std::uint32_t> fragCode)
		{
			auto ret = std::size_t{};
			CombineHash(ret, vertCode);
			CombineHash(ret, fragCode);
			return ret;
		}
	} // namespace

	bool Shader::Load(const vk::Device device, std::span<const std::uint32_t> vertCode, std::span<const std::uint32_t> fragCode)
	{
		if (!device || vertCode.empty() || fragCode.empty()) { return false; }

		auto smci = std::array<vk::ShaderModuleCreateInfo, 2>{};
		smci[0].setCode(vertCode);
		smci[1].setCode(fragCode);
		auto vertex = device.createShaderModuleUnique(smci[0]);
		auto fragment = device.createShaderModuleUnique(smci[1]);
		if (!vertex || !fragment) { return false; }

		_vertex = std::move(vertex);
		_fragment = std::move(fragment);
		_hash = ComputeHash(vertCode, fragCode); // needed for caching graphics pipelines.

		return true;
	}
} // namespace Tkge::Graphics
