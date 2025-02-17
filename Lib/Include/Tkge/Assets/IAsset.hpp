#pragma once

#include <klib/polymorphic.hpp>
#include <cstddef>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>

namespace Tkge::Assets
{
	namespace Detail
	{
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
	} // namespace Detail

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

		std::unique_ptr<Detail::MemoryMappedFile> _file;
	};

	class IAsset : public klib::Polymorphic
	{
	  public:
		virtual bool Load(ReadonlyByteStream) = 0;
	};
} // namespace Tkge::Assets
