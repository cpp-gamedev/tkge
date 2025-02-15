#pragma once

#include "IAsset.hpp"

namespace tkge::Assets
{
	class TextAsset final : public ICopyableAsset<TextAsset>
	{
	  public:
		explicit TextAsset(std::string filename) : ICopyableAsset(std::move(filename)) {}
		[[nodiscard]] std::string ReadAllText() const;
		[[nodiscard]] std::string ReadAt(std::size_t position, std::size_t size) const;
	};
} // namespace tkge::Assets
