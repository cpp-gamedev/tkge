#include <Tkge/Utilities.hpp>

#ifdef _WIN32
#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#define _AMD64_
#endif
#include <libloaderapi.h>
#include <minwindef.h>
#else // defined(_WIN32)
#include <limits.h>
#include <unistd.h>
#endif

std::filesystem::path Tkge::Utilities::GetCurrentExecutablePath()
{
	static std::filesystem::path CachePath{}; // can never change throughout the process existance
	if (!CachePath.empty()) return CachePath;

#ifdef _WIN32
	char buffer[MAX_PATH]{};
	DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	if (length == 0) return {}; // Error case
	CachePath = std::filesystem::path{std::string(buffer, length)}.parent_path();
#elif defined(__linux__)
	char buffer[PATH_MAX]{};
	ssize_t length = readlink("/proc/self/exe", buffer, PATH_MAX);
	if (length == -1) return {}; // Error case
	CachePath = std::filesystem::path{std::string(buffer, static_cast<std::size_t>(length))}.parent_path();
#else
	static_assert(false, "Unsupported platform");
#endif

	return CachePath;
}
