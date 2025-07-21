#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace ArchiveEngine {

    // Progress callback signature
    // Parameters: current bytes processed, total bytes, current file name, operation (extract/decompress)
    using ProgressCallback = std::function<bool(uint64_t current, uint64_t total, const std::wstring& fileName, const std::wstring& operation)>;

    // Extraction result information
    struct ExtractionResult {
        bool success;
        std::wstring errorMessage;
        std::vector<std::wstring> extractedFiles;
        uint64_t bytesProcessed;
        double timeElapsed; // seconds
    };

    // Archive entry information
    struct ArchiveEntry {
        std::wstring name;
        uint64_t size;
        uint64_t compressedSize;
        bool isDirectory;
        uint64_t lastModified; // Unix timestamp
        uint32_t permissions;
        std::wstring linkTarget; // For symbolic links
    };

    // Abstract base class for archive extractors
    class IArchiveExtractor {
    public:
        virtual ~IArchiveExtractor() = default;

        // Check if the file can be handled by this extractor
        virtual bool CanExtract(const std::wstring& filePath) const = 0;

        // Get archive information without extracting
        virtual bool GetArchiveInfo(const std::wstring& filePath, std::vector<ArchiveEntry>& entries) const = 0;

        // Extract archive to destination directory
        virtual ExtractionResult Extract(
            const std::wstring& archivePath,
            const std::wstring& destinationPath,
            ProgressCallback callback = nullptr) const = 0;

        // Get supported file extensions
        virtual std::vector<std::wstring> GetSupportedExtensions() const = 0;

        // Get extractor name/type
        virtual std::wstring GetExtractorName() const = 0;
    };

    // Archive type detection and management
    enum class ArchiveType {
        Unknown,
        Gzip,           // .gz
        Bzip2,          // .bz2
        Tar,            // .tar
        TarGzip,        // .tar.gz, .tgz
        TarBzip2        // .tar.bz2, .tbz2
    };

    // Archive factory for creating appropriate extractors
    class ArchiveExtractorFactory {
    public:
        static ArchiveType DetectArchiveType(const std::wstring& filePath);
        static std::unique_ptr<IArchiveExtractor> CreateExtractor(ArchiveType type);
        static std::unique_ptr<IArchiveExtractor> CreateExtractor(const std::wstring& filePath);
        
        // Get all supported extensions
        static std::vector<std::wstring> GetAllSupportedExtensions();
    };

    // Utility functions
    namespace Utils {
        // File system utilities
        bool CreateDirectoryRecursive(const std::wstring& path);
        bool FileExists(const std::wstring& path);
        bool IsDirectory(const std::wstring& path);
        uint64_t GetFileSize(const std::wstring& path);
        std::wstring GetFileName(const std::wstring& path);
        std::wstring GetFileExtension(const std::wstring& path);
        std::wstring GetParentDirectory(const std::wstring& path);
        std::wstring CombinePath(const std::wstring& basePath, const std::wstring& relativePath);
        
        // Path validation and security
        bool IsValidExtractionPath(const std::wstring& basePath, const std::wstring& entryPath);
        std::wstring SanitizePath(const std::wstring& path);
        
        // String utilities
        std::wstring ToLowerCase(const std::wstring& str);
        bool EndsWith(const std::wstring& str, const std::wstring& suffix);
        std::vector<std::wstring> SplitPath(const std::wstring& path);
        
        // Time utilities
        std::wstring FormatDuration(double seconds);
        std::wstring FormatFileSize(uint64_t bytes);
    }

    // Exception classes for error handling
    class ArchiveException : public std::exception {
    private:
        std::string message;
        
    public:
        explicit ArchiveException(const std::string& msg) : message(msg) {}
        explicit ArchiveException(const std::wstring& msg);
        
        const char* what() const noexcept override {
            return message.c_str();
        }
        
        const std::string& GetMessage() const { return message; }
    };

    class UnsupportedFormatException : public ArchiveException {
    public:
        explicit UnsupportedFormatException(const std::wstring& format);
    };

    class ExtractionException : public ArchiveException {
    public:
        explicit ExtractionException(const std::wstring& details);
    };

    class SecurityException : public ArchiveException {
    public:
        explicit SecurityException(const std::wstring& details);
    };

} // namespace ArchiveEngine