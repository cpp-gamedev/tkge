#include <tkge/Assets/TextAsset.hpp>

bool tkge::Assets::TextAsset::Load(ReadonlyByteStream byteStream)
{
	const auto vData = byteStream.AsSpan<char>(0, byteStream.GetStreamSize());
	this->_text = std::string{ vData.begin(), vData.end() };
	return true;
}
