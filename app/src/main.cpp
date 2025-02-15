#include <imgui.h>
#include <tkge/engine.hpp>
#include <exception>
#include <print>

namespace
{
void Run()
{
	static constexpr tkge::WindowSurface surface{.size = {1280, 720}};
	tkge::Engine engine{surface};
	while (engine.IsRunning())
	{
		engine.NextFrame();
		ImGui::ShowDemoWindow();
		engine.BeginRender();
		engine.EndRender();
	}
}
} // namespace

int main()
{
	try
	{
		Run();
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
