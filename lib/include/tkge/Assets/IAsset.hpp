#pragma once

#include <cstddef>
#include <fstream>
#include <span>
#include <string>

// Possible optimisations:
#define ASSET_USE_MEMORY_MAPPED_FILES
// #define ASSET_USE_FS_CACHE

namespace tkge::Assets
{
	namespace detail
	{
#ifdef ASSET_USE_MEMORY_MAPPED_FILES
		enum struct MemoryFileCapabilities : std::uint8_t
		{
			None = 0,
			Prefetch = 1 << 0,
			ExplicitFlush = 1 << 1,
		};
		constexpr bool operator&(const MemoryFileCapabilities left, const MemoryFileCapabilities right)
		{
			return (static_cast<std::underlying_type_t<MemoryFileCapabilities>>(left) & static_cast<std::underlying_type_t<MemoryFileCapabilities>>(right)) !=
				   0;
		}
		constexpr MemoryFileCapabilities operator|(const MemoryFileCapabilities left, const MemoryFileCapabilities right)
		{
			return static_cast<MemoryFileCapabilities>(static_cast<std::underlying_type_t<MemoryFileCapabilities>>(left) |
													   static_cast<std::underlying_type_t<MemoryFileCapabilities>>(right));
		}
		class MemoryMappedFile
		{
		  public:
			virtual ~MemoryMappedFile() = default;

			/// <summary>
			/// Get the view to the mapped file.
			/// </summary>
			/// <returns>Beginning of the mapped file. Contiguous array.</returns>
			[[nodiscard]] virtual void* GetView() const = 0;

			/// <summary>
			/// Explicitly flush (if supported, please check Capabilities()) a range of the memory mapped file.
			/// Flushing more bytes than the file size is undefined.
			/// </summary>
			/// <param name="offset">An implementation may or may not use or suggest the flushing range with this parameter.</param>
			/// <param name="size">Size of the flushing. An implementation may or may not use or suggest the flushing range with this parameter. It is defined
			/// that when this parameter is 0, the entire file should get flushed</param>
			virtual void Flush(std::size_t offset = 0, std::size_t size = 0) = 0;

			/// <summary>
			/// Prefetch (if supported, please check Capabilities()) a range in the memory mapped file.
			/// Prefetching outside of the file size is undefined.
			/// </summary>
			/// <param name="offset">An implementation may either use it directly, or suggest the real prefetching offset. The range has to include the offset +
			/// size, of course.</param> <param name="size">An implementation may use or suggest the size of flushing range, but the range size has to be at
			/// least the specified size. The behaviour is unspecified if the size is 0.</param>
			virtual void Prefetch(std::size_t offset, std::size_t size) = 0;

			/// <summary>
			/// Retrieve the size of the memory mapped file.
			/// </summary>
			/// <returns>Size of the file</returns>
			[[nodiscard]] virtual std::size_t Size() const = 0;

			/// <summary>
			/// Get the capabilities of the implementation.
			/// </summary>
			/// <returns>Capabilities of the implementation</returns>
			[[nodiscard]] virtual MemoryFileCapabilities Capabilities() const noexcept = 0;
		};
#elif defined(ASSET_USE_FS_CACHE)
		class AssetFileCache
		{
		  public:
		  private:
		};
#endif
	} // namespace detail

	class ReadonlyByteStream
	{
	  public:
		explicit ReadonlyByteStream(const std::string& filename);
		~ReadonlyByteStream() = default;
		ReadonlyByteStream(const ReadonlyByteStream&) = delete;
		ReadonlyByteStream(ReadonlyByteStream&&) = default;
		ReadonlyByteStream& operator=(const ReadonlyByteStream&) = delete;
		ReadonlyByteStream& operator=(ReadonlyByteStream&&) = default;

		/// <summary>
		/// Get a read-write span at loaded offset
		/// </summary>
		/// <typeparam name="TUnit">Unit to be used. Default: std::byte</typeparam>
		/// <typeparam name="TSize">Static size of load. Default: specify in loadSize parameter</typeparam>
		/// <param name="offset">Offset IN BYTES to the data. Default 0</param>
		/// <param name="loadSize">Load size in elements. Default: TSize</param>
		/// <returns>A read-write span to the data</returns>
		template <typename TUnit = std::byte, std::size_t TSize = std::dynamic_extent>
			requires std::is_trivially_copy_constructible_v<TUnit>
		[[nodiscard]] std::span<TUnit, TSize> AsSpan(const std::size_t offset = 0, const std::size_t loadSize = TSize)
		{
			if (loadSize == std::dynamic_extent) throw std::logic_error("Must specify loadSize when using dynamic_extent");

			const std::size_t loadBytesSize = sizeof(TUnit) * loadSize;
			auto data = ReadChunk(offset, loadBytesSize, false);

			return {reinterpret_cast<TUnit*>(data.data()), loadSize};
		}
		/// <summary>
		/// Get a readonly span at loaded offset
		/// </summary>
		/// <typeparam name="TUnit">Unit to be used. Default: std::byte</typeparam>
		/// <typeparam name="TSize">Static size of load. Default: specify in loadSize parameter</typeparam>
		/// <param name="offset">Offset IN BYTES to the data. Default 0</param>
		/// <param name="loadSize">Load size in elements. Default: TSize</param>
		/// <returns>A readonly span to the data</returns>
		template <typename TUnit = std::byte, std::size_t TSize = std::dynamic_extent>
			requires std::is_trivially_copy_constructible_v<TUnit>
		[[nodiscard]] std::span<const TUnit, TSize> AsSpan(const std::size_t offset = 0, const std::size_t loadSize = TSize) const
		{
			if (loadSize == std::dynamic_extent) throw std::logic_error("Must specify loadSize when using dynamic_extent");

			const std::size_t loadBytesSize = sizeof(TUnit) * loadSize;
			auto data = ReadChunk(offset, loadBytesSize, false);

			return {reinterpret_cast<const TUnit*>(data.data()), loadSize};
		}

		/// <summary>
		/// Gets the size of the entire stream (file size)
		/// </summary>
		/// <returns>Size of the stream</returns>
		[[nodiscard]] std::size_t GetStreamSize() const;

	  private:
		std::span<std::byte> ReadChunk(std::size_t offset, std::size_t size, bool prefetchMemory);
		std::span<const std::byte> ReadChunk(std::size_t offset, std::size_t size, bool prefetchMemory) const;

#ifdef ASSET_USE_MEMORY_MAPPED_FILES
		std::unique_ptr<detail::MemoryMappedFile> _file;
#elif defined(ASSET_USE_FS_CACHE)
		AssetFileCache _cache;
#else
		std::ifstream _file;
#endif
	};

	class IAsset
	{
	  public:
		virtual ~IAsset() = default;

	  protected:
		[[nodiscard]] virtual ReadonlyByteStream& byteStream() noexcept = 0;
		[[nodiscard]] virtual const ReadonlyByteStream& byteStream() const noexcept = 0;
	};

	template <typename T>
	class IMoveableAsset : public virtual IAsset
	{
	  public:
		explicit IMoveableAsset(std::string filename) : _byteStream(std::move(filename)) {}
		//		static_assert(!std::is_base_of_v<ICopyableAsset<T>, T>, "Cannot derive from both move and copy assets!");
		~IMoveableAsset() override = default;
		[[nodiscard]] ReadonlyByteStream& byteStream() noexcept override { return this->_byteStream; }
		[[nodiscard]] const ReadonlyByteStream& byteStream() const noexcept override { return this->_byteStream; }

	  private:
		ReadonlyByteStream _byteStream;

		friend T;
	};
	template <typename T>
	class ICopyableAsset : public virtual IAsset
	{
	  public:
		explicit ICopyableAsset(std::string filename) : _byteStream(std::make_shared<ReadonlyByteStream>(std::move(filename))) {}
		//		static_assert(!std::is_base_of_v<IMoveableAsset<T>, T>, "Cannot derive from both move and copy assets!");
		~ICopyableAsset() override = default;
		[[nodiscard]] ReadonlyByteStream& byteStream() noexcept override { return *this->_byteStream; }
		[[nodiscard]] const ReadonlyByteStream& byteStream() const noexcept override { return *this->_byteStream; }

	  private:
		std::shared_ptr<ReadonlyByteStream> _byteStream;

		friend T;
	};
} // namespace tkge::Assets
