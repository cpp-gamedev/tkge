#pragma once

#include "IAsset.hpp"

namespace tkge::Assets
{
	class TextAsset final : public IAsset
	{
	  public:
		explicit TextAsset() = default;
		void Load(ReadonlyByteStream byteStream) override
		{
			const auto vData = byteStream.AsSpan<char>(0, byteStream.GetStreamSize());
			this->_text = std::string{vData.begin(), vData.end()};
		}

		[[nodiscard]] const std::string& text() const noexcept { return this->_text; }

	  private:
		std::string _text;
	};
} // namespace tkge::Assets
