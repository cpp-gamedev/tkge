#pragma once

#include "IAsset.hpp"

namespace Tkge::Assets
{
	class TextAsset final : public IAsset
	{
	  public:
		explicit TextAsset() = default;
		bool Load(ReadonlyByteStream byteStream) override;

		[[nodiscard]] const std::string& Text() const noexcept { return this->_text; }

	  private:
		std::string _text;
	};
} // namespace Tkge::Assets
