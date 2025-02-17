#include <Tkge/Assets/TextAsset.hpp>
#include <klib/unit_test.hpp>
#include <filesystem>
#include <fstream>

namespace
{
	using namespace Tkge;

	struct TempFile
	{
		TempFile(const TempFile&) = delete;
		TempFile(TempFile&&) = delete;
		TempFile& operator=(const TempFile&) = delete;
		TempFile& operator=(TempFile&&) = delete;

		explicit TempFile(std::string path, const std::string_view text) : _path(std::move(path))
		{
			auto file = std::ofstream{_path};
			ASSERT(file.is_open());
			file << text;
		}

		~TempFile() { std::filesystem::remove(_path); }

		[[nodiscard]] const std::string& GetPath() const { return _path; }

	  private:
		std::string _path;
	};

	TEST(TextAsset_Load)
	{
		static constexpr std::string_view Text{"Hello World"};

		const auto file = TempFile{"Test.txt", Text};

		auto stream = Assets::ReadonlyByteStream{file.GetPath()};
		EXPECT(stream.GetStreamSize() > 0);

		auto asset = Assets::TextAsset{};
		EXPECT(asset.Load(std::move(stream)));

		const auto& text = asset.Text();
		EXPECT(text == Text);
	}
} // namespace
