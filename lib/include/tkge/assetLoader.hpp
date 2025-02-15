#pragma once

#include <tkge/Assets/IAsset.hpp>
#include <filesystem>
#include <string>
#include <type_traits>

namespace tkge
{
	class AssetLoader
	{
	  public:
		template <typename T>
			requires std::is_base_of_v<Assets::IAsset, T>
		std::unique_ptr<T> LoadAsset(const std::string& fileName) const
		{
			const std::vector<std::filesystem::path> paths = this->GetSearchPaths();

			for (const auto& path : paths)
			{
				if (std::filesystem::exists(path / fileName))
				{
					auto asset = std::make_unique<T>();
					Assets::ReadonlyByteStream byteStream{(path / fileName).string()};
					asset->Load(std::move(byteStream));
					return asset;
				}
			}

			throw std::runtime_error("Asset not found: " + fileName);
		}

		[[nodiscard]] std::vector<std::filesystem::path> GetSearchPaths() const;
	};
} // namespace tkge
