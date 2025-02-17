#include <imgui.h>
#include <Tkge/Engine.hpp>
#include <Tkge/Graphics/Drawable.hpp>
#include <Tkge/Graphics/Shader.hpp>
#include <Tkge/Utilities.hpp>
#include <klib/assert.hpp>
#include <kvf/time.hpp>
#include <cmath>
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
		const auto vertexSpirV = LoadSpirV((assets_path / "Shaders/Default.vert").string().c_str());
		const auto fragmentSpirV = LoadSpirV((assets_path / "Shaders/Default.frag").string().c_str());
		const auto& renderDevice = engine.RenderDevice();
		if (!shader.Load(renderDevice.get_device(), vertexSpirV, fragmentSpirV)) { throw std::runtime_error{"Failed to load shaders"}; }

		auto quad = Tkge::Graphics::Quad{};
		quad.Create(glm::vec2{400.0f});
		quad.transform.position.x = -250.0f;
		quad.tint = kvf::magenta_v;

		auto instancedQuad = Tkge::Graphics::InstancedQuad{};
		instancedQuad.Create(glm::vec2{150.0f});

		instancedQuad.instances.resize(2);
		instancedQuad.instances[0].transform.position = {250.0f, -100.0f};
		instancedQuad.instances[0].tint = kvf::cyan_v;
		instancedQuad.instances[1].transform.position = {250.0f, 100.0f};
		instancedQuad.instances[1].tint = kvf::yellow_v;

		auto wireframe = false;
		auto lineWidth = 3.0f;

		auto deltaTime = kvf::DeltaTime{};
		auto elapsed = kvf::Seconds{};

		while (engine.IsRunning())
		{
			engine.NextFrame();

			const auto dt = deltaTime.tick();
			elapsed += dt;

			instancedQuad.instances[0].tint.w = kvf::Color::to_u8((0.5f * std::sin(elapsed.count())) + 0.5f);
			instancedQuad.instances[1].tint.w = kvf::Color::to_u8((0.5f * std::sin(-elapsed.count())) + 0.5f);
			quad.tint.w = kvf::Color::to_u8((0.5f * std::cos(elapsed.count())) + 0.5f);

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
				instancedQuad.Draw(renderer);
				quad.Draw(renderer);
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
		const auto assets_path = Upfind(Tkge::Utilities::GetCurrentExecutablePath(), "Assets");
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
