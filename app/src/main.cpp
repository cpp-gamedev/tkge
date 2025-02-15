#include <imgui.h>
#include <klib/assert.hpp>
#include <tkge/engine.hpp>
#include <exception>
#include <print>

// temporary, until we have shader assets.
#include <tkge/graphics/shader.hpp>
#include <filesystem>
#include <fstream>
#include <vector>

namespace
{
namespace fs = std::filesystem;

fs::path Upfind(fs::path const& leafDir, std::string_view const pattern)
{
	for (auto path = leafDir; path.has_parent_path() && path.parent_path() != path; path = path.parent_path())
	{
		auto ret = path / pattern;
		if (fs::exists(ret)) { return fs::absolute(ret); }
	}
	return fs::current_path();
}

std::vector<std::uint32_t> LoadSpirV(klib::CString const path)
{
	auto file = std::ifstream{path.c_str(), std::ios::binary | std::ios::ate};
	if (!file) { return {}; }

	auto const size = file.tellg();
	if (std::size_t(size) % sizeof(std::uint32_t) != 0) { return {}; } // invalid SPIR-V.

	file.seekg(std::ios::beg, {});
	auto ret = std::vector<std::uint32_t>{};
	ret.resize(std::size_t(size) / sizeof(std::uint32_t));
	file.read(reinterpret_cast<char*>(ret.data()), size); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
	return ret;
}

void Run(fs::path const& assets_path)
{
	static constexpr tkge::WindowSurface surface{.size = {1280, 720}};
	tkge::Engine engine{surface};

	auto shader = tkge::graphics::Shader{};
	auto const vertexSpirV = LoadSpirV((assets_path / "shaders/triangle.vert").string().c_str());
	auto const fragmentSpirV = LoadSpirV((assets_path / "shaders/triangle.frag").string().c_str());
	auto const& renderDevice = engine.RenderDevice();
	if (!shader.Load(renderDevice.get_device(), vertexSpirV, fragmentSpirV)) { throw std::runtime_error{"Failed to load shaders"}; }

	auto const pipelineLayout = renderDevice.get_device().createPipelineLayoutUnique({});

	auto const pipelineState = kvf::PipelineState{
		.vertex_bindings = {},
		.vertex_attributes = {},
		.vertex_shader = shader.VertexModule(),
		.fragment_shader = shader.FragmentModule(),
	};
	auto const pipelineFormat = kvf::PipelineFormat{
		.samples = engine.FramebufferSamples(),
		.color = engine.FramebufferFormat(),
	};
	auto const pipeline = renderDevice.create_pipeline(*pipelineLayout, pipelineState, pipelineFormat);
	if (!pipeline) { throw std::runtime_error{"Failed to create graphics pipeline"}; }

	while (engine.IsRunning())
	{
		auto const commandBuffer = engine.NextFrame();

		ImGui::ShowDemoWindow();

		engine.BeginRender();
		engine.RenderPass().bind_pipeline(*pipeline);
		commandBuffer.draw(3, 1, 0, 0);
		engine.EndRender();
	}

	renderDevice.get_device().waitIdle();
}
} // namespace

int main([[maybe_unused]] int argc, char** argv)
{
	try
	{
		KLIB_ASSERT(argc > 0);
		auto const assets_path = Upfind(*argv, "assets");
		Run(assets_path);
	}
	catch (std::exception const& e)
	{
		std::println(stderr, "PANIC: {}", e.what());
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::println(stderr, "PANIC!");
		return EXIT_FAILURE;
	}
}
