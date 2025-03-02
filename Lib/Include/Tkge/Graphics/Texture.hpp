#pragma once
#include <Tkge/Graphics/TextureSampler.hpp>
#include <kvf/bitmap.hpp>
#include <kvf/render_device.hpp>
#include <kvf/vma.hpp>
#include <cstddef>
#include <span>

namespace Tkge::Graphics
{
	class Texture
	{
	  public:
		using Sampler = TextureSampler;

		explicit Texture(gsl::not_null<kvf::RenderDevice*> renderDevice);

		bool Create(const kvf::Bitmap& bitmap);
		bool Decompress(std::span<const std::byte> compressedImage);

		[[nodiscard]] const kvf::vma::Image& GetImage() const { return _image; }
		[[nodiscard]] glm::ivec2 GetSize() const;

		Sampler sampler{};

	  private:
		kvf::vma::Image _image{};
	};
} // namespace Tkge::Graphics
