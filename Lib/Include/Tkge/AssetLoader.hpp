#pragma once

#include <Tkge/Assets/IAsset.hpp>
#include <filesystem>
#include <string>
#include <type_traits>

namespace Tkge
{
	class AssetLoader
	{
	  public:
		explicit AssetLoader() : _paths(this->GetSearchPaths()) {}
		AssetLoader(const AssetLoader&) = delete;
		AssetLoader(AssetLoader&&) = default;
		AssetLoader& operator=(const AssetLoader&) = delete;
		AssetLoader& operator=(AssetLoader&&) = default;
		~AssetLoader() = default;

		template <typename T>
			requires std::is_base_of_v<Assets::IAsset, T>
		std::unique_ptr<T> LoadAsset(const std::string& fileName) const
		{
			for (const auto& path : this->_paths)
			{
				if (std::filesystem::exists(path / fileName))
				{
					auto asset = std::make_unique<T>();
					asset->Load(Assets::ReadonlyByteStream{(path / fileName).string()});
					return asset;
				}
			}

			throw std::runtime_error("Asset not found: " + fileName);
		}

		[[nodiscard]] std::vector<std::filesystem::path> GetSearchPaths() const;

	  private:
		std::vector<std::filesystem::path> _paths;
	};
} // namespace Tkge
