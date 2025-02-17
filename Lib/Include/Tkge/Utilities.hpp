#pragma once

#include <filesystem>
#include <string>

namespace Tkge
{
	class Utilities
	{
	  public:
		~Utilities() = delete;
		Utilities() = delete;
		Utilities(const Utilities&) = delete;
		Utilities(Utilities&) = delete;
		Utilities& operator=(const Utilities&) = delete;
		Utilities& operator=(Utilities&) = delete;

		/// <summary>
		/// Gets the current executable directory.
		/// </summary>
		/// <returns>The parent directory of the main module</returns>
		[[nodiscard]] static std::filesystem::path GetCurrentExecutablePath();
	};
} // namespace Tkge
