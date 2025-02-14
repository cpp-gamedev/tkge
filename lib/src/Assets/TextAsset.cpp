#include <tkge/Assets/TextAsset.hpp>

std::string tkge::Assets::TextAsset::ReadAllText() const
{
	const auto vData = this->byteStream().AsSpan<char>(0, this->byteStream().GetStreamSize());
	return std::string{ vData.begin(), vData.end() };
}

std::string tkge::Assets::TextAsset::ReadAt(std::size_t position, std::size_t size) const
{
	const auto vData = this->byteStream().AsSpan<char>(position, size);
	return std::string{ vData.begin(), vData.end() };
}
