#include <tkge/Assets/IAsset.hpp>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _AMD64_
// clang-format off
#include <Memoryapi.h>
#include <wtypes.h>
#include <WinBase.h>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <processthreadsapi.h>
// clang-format on
#elif defined(__linux__)
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace tkge::Assets::detail
{
#ifdef _WIN32
	class Win32MemoryMappedFile final : public MemoryMappedFile
	{
	  public:
		Win32MemoryMappedFile(const std::string& filename)
		{
			this->_hFile = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (this->_hFile == INVALID_HANDLE_VALUE)
			{
				const auto errorCode = GetLastError();
				throw std::runtime_error("Unable to open the file (code: " + std::to_string(errorCode) + ")");
			}

			this->_hMap = CreateFileMapping(this->_hFile, nullptr, PAGE_READWRITE, 0, 0, nullptr);
			if (this->_hMap == INVALID_HANDLE_VALUE)
			{
				const auto errorCode = GetLastError();
				CloseHandle(this->_hFile);
				throw std::runtime_error("Unable to map the file (code: " + std::to_string(errorCode) + ")");
			}

			this->_vView = MapViewOfFile(this->_hMap, PAGE_READWRITE, 0, 0, 0);
			if (this->_vView == nullptr)
			{
				const auto errorCode = GetLastError();
				CloseHandle(this->_hMap);
				CloseHandle(this->_hFile);
				throw std::runtime_error("Unable to create a view to the mapped object (code: " + std::to_string(errorCode) + ")");
			}
		}

		~Win32MemoryMappedFile() noexcept
		{
			if (this->_vView) UnmapViewOfFile(this->_vView);
			if (this->_hMap != INVALID_HANDLE_VALUE) CloseHandle(this->_hMap);
			if (this->_hFile != INVALID_HANDLE_VALUE) CloseHandle(this->_hFile);
		}

		[[nodiscard]] void* GetView() const override { return this->_vView; }
		void Flush(const std::size_t offset, std::size_t size) override
		{
			if (size == 0) size = this->Size();
			if (FlushViewOfFile(static_cast<std::byte*>(this->_vView) + offset, size)) return;
			const auto errorCode = GetLastError();
			throw std::runtime_error("Could not flush the view of file. Error code: " + std::to_string(errorCode));
		}

		void Prefetch(const std::size_t offset, std::size_t size) override
		{
			if (size == 0) size = this->Size();
#ifdef _DEBUG
				// runtime_assert(size + offset <= this->Size());
#endif

			WIN32_MEMORY_RANGE_ENTRY range{};
			range.VirtualAddress = static_cast<BYTE*>(_vView) + offset;
			range.NumberOfBytes = size;

			if (!PrefetchVirtualMemory(GetCurrentProcess(), 1, &range, 0))
			{
				const auto errorCode = GetLastError();
				throw std::runtime_error("Failed to prefetch memory pages (code: " + std::to_string(errorCode) + ")");
			}
		}
		[[nodiscard]] std::size_t Size() const override
		{
			LARGE_INTEGER fileSize;
			if (!GetFileSizeEx(_hFile, &fileSize)) [[unlikely]]
			{
				const auto errorCode = GetLastError();
				throw std::runtime_error("Unable to get file size. Code: " + std::to_string(errorCode));
			}
			return static_cast<std::size_t>(fileSize.QuadPart);
		}
		[[nodiscard]] MemoryFileCapabilities Capabilities() const noexcept override { return MemoryFileCapabilities::Prefetch | MemoryFileCapabilities::ExplicitFlush; }

	  private:
		HANDLE _hFile{INVALID_HANDLE_VALUE};
		HANDLE _hMap{INVALID_HANDLE_VALUE};
		void* _vView{nullptr};
	};

	using MemoryMappedFileImplementation = Win32MemoryMappedFile;
#elif defined(__linux__)
	class LinuxMemoryMappedFile final : public MemoryMappedFile
	{
	  public:
		explicit LinuxMemoryMappedFile(const std::string& filename)
		{
			this->_fd = open(filename.c_str(), O_RDWR);
			if (this->_fd == -1) { throw std::runtime_error("Unable to open the file (code: " + std::to_string(errno) + ")"); }

			off_t fileSize = lseek(this->_fd, 0, SEEK_END);
			if (fileSize == -1)
			{
				close(this->_fd);
				throw std::runtime_error("Unable to get the file size (code: " + std::to_string(errno) + ")");
			}

			this->_vView = mmap(nullptr, static_cast<std::size_t>(fileSize), PROT_READ | PROT_WRITE, MAP_SHARED, this->_fd, 0);
			if (this->_vView == MAP_FAILED)
			{
				close(this->_fd);
				throw std::runtime_error("Unable to create a view to the mapped object (code: " + std::to_string(errno) + ")");
			}

			this->_fileSize = static_cast<std::size_t>(fileSize);
		}

		~LinuxMemoryMappedFile() noexcept
		{
			if (this->_vView != MAP_FAILED) { munmap(this->_vView, this->_fileSize); }
			if (this->_fd != -1) { close(this->_fd); }
		}

		[[nodiscard]] void* GetView() const override { return this->_vView; }

		void Flush(const std::size_t offset, std::size_t size) override
		{
			if (msync(static_cast<char*>(this->_vView) + offset, size, MS_SYNC) == -1)
			{
				throw std::runtime_error("Unable to flush the memory-mapped file (code: " + std::to_string(errno) + ")");
			}
		}

		void Prefetch(const std::size_t offset, std::size_t size) override
		{
			if (madvise(static_cast<char*>(this->_vView) + offset, size, MADV_WILLNEED) == -1)
			{
				throw std::runtime_error("Unable to prefetch memory (code: " + std::to_string(errno) + ")");
			}
		}

		[[nodiscard]] std::size_t Size() const override { return this->_fileSize; }

		[[nodiscard]] MemoryFileCapabilities Capabilities() const noexcept override
		{
			return MemoryFileCapabilities::Prefetch | MemoryFileCapabilities::ExplicitFlush; // Modify based on your requirements
		}

	  private:
		int _fd{-1};
		void* _vView{MAP_FAILED};
		std::size_t _fileSize{0};
	};

	using MemoryMappedFileImplementation = LinuxMemoryMappedFile;
#else
#error Not implemented yet
#endif
} // namespace tkge::Assets::detail

tkge::Assets::ReadonlyByteStream::ReadonlyByteStream(const std::string& filename)
#ifdef ASSET_USE_MEMORY_MAPPED_FILES
	: _file(std::make_unique<tkge::Assets::detail::MemoryMappedFileImplementation>(filename))
#elif defined(ASSET_USE_FS_CACHE)
#error Not implemented yet
#else
#error Not implemented yet
#endif
{
}

std::span<std::byte> tkge::Assets::ReadonlyByteStream::ReadChunk(const std::size_t offset, const std::size_t size, const bool prefetchMemory)
{
#ifdef ASSET_USE_MEMORY_MAPPED_FILES
	std::byte* const vData = static_cast<std::byte*>(this->_file->GetView());
	if (prefetchMemory && this->_file->Capabilities() & detail::MemoryFileCapabilities::Prefetch) this->_file->Prefetch(offset, size);
	return std::span{vData + offset, size};
#elif defined(ASSET_USE_FS_CACHE)
#error Not implemented yet
#else
#error Not implemented yet
#endif
}

std::span<const std::byte> tkge::Assets::ReadonlyByteStream::ReadChunk(const std::size_t offset, const std::size_t size, const bool prefetchMemory) const
{
#ifdef ASSET_USE_MEMORY_MAPPED_FILES
	const std::byte* const vData = static_cast<std::byte*>(this->_file->GetView());
	if (prefetchMemory && this->_file->Capabilities() & detail::MemoryFileCapabilities::Prefetch) this->_file->Prefetch(offset, size);
	return std::span{vData + offset, size};
#elif defined(ASSET_USE_FS_CACHE)
#error Not implemented yet
#else
#error Not implemented yet
#endif
}

std::size_t tkge::Assets::ReadonlyByteStream::GetStreamSize() const
{
#ifdef ASSET_USE_MEMORY_MAPPED_FILES
	return this->_file->Size();
#elif defined(ASSET_USE_FS_CACHE)
#error Not implemented yet
#else
#error Not implemented yet
#endif
}
