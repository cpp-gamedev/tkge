#include <Tkge/AssetLoader.hpp>
#include <Tkge/Utilities.hpp>
#include <filesystem>

std::vector<std::filesystem::path> Tkge::AssetLoader::GetSearchPaths() const
{
	std::vector<std::filesystem::path> paths{};

	paths.emplace_back(".");

	paths.push_back(Tkge::Utilities::GetCurrentExecutablePath());
	paths.push_back(paths.back() / "Assets");

	return paths;
}
