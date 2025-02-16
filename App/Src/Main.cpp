#include <imgui.h>
#include <Tkge/Engine.hpp>
#include <Tkge/Graphics/Shader.hpp>
#include <klib/assert.hpp>
#include <exception>
#include <print>

// temporary, until we have shader Assets.
#include <filesystem>
#include <fstream>
#include <vector>

namespace
{
	namespace fs = std::filesystem;

	fs::path Upfind(const fs::path& leafDir, const std::string_view pattern)
	{
		for (auto path = leafDir; path.has_parent_path() && path.parent_path() != path; path = path.parent_path())
		{
			auto ret = path / pattern;
			if (fs::exists(ret)) { return fs::absolute(ret); }
		}
		return fs::current_path();
	}

	std::vector<std::uint32_t> LoadSpirV(const klib::CString path)
	{
		auto file = std::ifstream{path.c_str(), std::ios::binary | std::ios::ate};
		if (!file) { return {}; }

		const auto size = file.tellg();
		if (std::size_t(size) % sizeof(std::uint32_t) != 0) { return {}; } // invalid SPIR-V.

		file.seekg(std::ios::beg, {});
		auto ret = std::vector<std::uint32_t>{};
		ret.resize(std::size_t(size) / sizeof(std::uint32_t));
		file.read(reinterpret_cast<char*>(ret.data()), size); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
		return ret;
	}

	void Run(const fs::path& assets_path)
	{
		static constexpr Tkge::WindowSurface surface{.size = {1280, 720}};
		Tkge::Engine engine{surface};

		auto shader = Tkge::Graphics::Shader{};
		const auto vertexSpirV = LoadSpirV((assets_path / "shaders/triangle.vert").string().c_str());
		const auto fragmentSpirV = LoadSpirV((assets_path / "shaders/triangle.frag").string().c_str());
		const auto& renderDevice = engine.RenderDevice();
		if (!shader.Load(renderDevice.get_device(), vertexSpirV, fragmentSpirV)) { throw std::runtime_error{"Failed to load shaders"}; }

		constexpr auto vertices = std::array{
			Tkge::Graphics::Vertex{.position = {-0.5f, -0.5f}, .colour = kvf::red_v.to_vec4()},
			Tkge::Graphics::Vertex{.position = {0.5f, -0.5f}, .colour = kvf::green_v.to_vec4()},
			Tkge::Graphics::Vertex{.position = {0.5f, 0.5f}, .colour = kvf::blue_v.to_vec4()},
			Tkge::Graphics::Vertex{.position = {-0.5f, 0.5f}, .colour = kvf::yellow_v.to_vec4()},
		};

		constexpr auto indices = std::array{
			0u, 1u, 2u, 2u, 3u, 0u,
		};

		const auto primitive = Tkge::Graphics::Primitive{
			.vertices = vertices,
			.indices = indices,
		};

		auto wireframe = false;
		auto lineWidth = 3.0f;

		while (engine.IsRunning())
		{
			engine.NextFrame();

			if (ImGui::Begin("Misc"))
			{
				ImGui::Checkbox("wireframe", &wireframe);
				ImGui::DragFloat("line width", &lineWidth, 1.0f, 1.0f, 100.0f);
			}
			ImGui::End();

			if (auto renderer = engine.BeginRender())
			{
				renderer.BindShader(shader);
				renderer.SetLineWidth(lineWidth);
				renderer.SetWireframe(wireframe);
				renderer.Draw(primitive);
			}

			engine.Present();
		}

		renderDevice.get_device().waitIdle();
	}
} // namespace

int main([[maybe_unused]] int argc, char** argv)
{
	try
	{
		KLIB_ASSERT(argc > 0);
		const auto assets_path = Upfind(*argv, "Assets");
		Run(assets_path);
	}
	catch (const std::exception& e)
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
