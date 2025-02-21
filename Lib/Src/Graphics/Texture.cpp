#include <Tkge/Graphics/Texture.hpp>
#include <kvf/image_bitmap.hpp>
#include <kvf/util.hpp>

namespace Tkge::Graphics
{
	namespace
	{
		constexpr auto ImageCreateInfo = kvf::vma::ImageCreateInfo{
			.format = vk::Format::eR8G8B8A8Srgb,
		};

	}

	Texture::Texture(gsl::not_null<kvf::RenderDevice*> renderDevice) : _image(renderDevice, ImageCreateInfo) {}

	bool Texture::Create(const kvf::Bitmap& bitmap)
	{
		if (bitmap.bytes.empty()) { return false; }
		return kvf::util::write_to(_image, bitmap);
	}

	bool Texture::Decompress(std::span<const std::byte> compressedImage)
	{
		if (compressedImage.empty()) { return false; }
		const auto image = kvf::ImageBitmap{compressedImage};
		if (!image.is_loaded()) { return false; }
		return Create(image.bitmap());
	}

	glm::ivec2 Texture::GetSize() const { return kvf::util::to_glm_vec<int>(_image.get_extent()); }
} // namespace Tkge::Graphics
