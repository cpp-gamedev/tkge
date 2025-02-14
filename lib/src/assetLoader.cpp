#include <tkge\assetLoader.hpp>
#include <filesystem>

#ifdef _WIN32
#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN			
#define _AMD64_
#endif
#include <minwindef.h>	  
#include <libloaderapi.h>
#endif

std::vector<std::filesystem::path> tkge::AssetLoader::GetSearchPaths() const
{
	std::vector<std::filesystem::path> paths{};

	paths.emplace_back(".");

#ifdef _WIN32
	char buffer[MAX_PATH]{};
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);

	paths.push_back(std::filesystem::path{ buffer }.parent_path());
	paths.push_back(paths.back() / "assets");
#else
#endif

	return paths;
}
