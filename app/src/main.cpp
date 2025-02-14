#include <tkge/engine.hpp>
#include <exception>
#include <print>

namespace
{
void Run()
{
	tkge::Engine engine;
	while (engine.isRunning())
	{
		engine.nextFrame();
		engine.beginRender();
		engine.endRender();
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
