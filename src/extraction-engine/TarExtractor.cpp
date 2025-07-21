#include "TarExtractor.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <cstring>

namespace ArchiveEngine {

    // TarHeader implementation
    bool TarHeader::IsValid() const {
        // Check magic number for POSIX TAR
        return (strncmp(magic, "ustar", 5) == 0);
    }

    uint64_t TarHeader::GetFileSize() const {
        // Convert octal string to decimal
        uint64_t result = 0;
        for (int i = 0; i < 11 && size[i] != '\0' && size[i] != ' '; ++i) {
            if (size[i] >= '0' && size[i] <= '7') {
                result = result * 8 + (size[i] - '0');
            }
        }
        return result;
    }

    uint64_t TarHeader::GetModificationTime() const {
        uint64_t result = 0;
        for (int i = 0; i < 11 && mtime[i] != '\0' && mtime[i] != ' '; ++i) {
            if (mtime[i] >= '0' && mtime[i] <= '7') {
                result = result * 8 + (mtime[i] - '0');
            }
        }
        return result;
    }

    std::wstring TarHeader::GetFileName() const {
        std::string fullName;
        
        // Check if prefix is used
        if (prefix[0] != '\0') {
            fullName = std::string(prefix, strnlen(prefix, sizeof(prefix)));
            fullName += "/";
        }
        
        fullName += std::string(name, strnlen(name, sizeof(name)));
        
        // Convert to wide string
        return std::wstring(fullName.begin(), fullName.end());
    }

    std::wstring TarHeader::GetLinkName() const {
        std::string link = std::string(linkname, strnlen(linkname, sizeof(linkname)));
        return std::wstring(link.begin(), link.end());
    }

    bool TarHeader::IsDirectory() const {
        return typeflag == TarFileType::Directory;
    }

    bool TarHeader::IsRegularFile() const {
        return typeflag == TarFileType::RegularFile || typeflag == TarFileType::AlternateRegularFile;
    }

    bool TarHeader::IsSymbolicLink() const {
        return typeflag == TarFileType::SymbolicLink;
    }

    uint32_t TarHeader::GetPermissions() const {
        uint32_t result = 0;
        for (int i = 0; i < 7 && mode[i] != '\0' && mode[i] != ' '; ++i) {
            if (mode[i] >= '0' && mode[i] <= '7') {
                result = result * 8 + (mode[i] - '0');
            }
        }
        return result;
    }

    // TarExtractor implementation
    bool TarExtractor::CanExtract(const std::wstring& filePath) const {
        std::wstring extension = Utils::ToLowerCase(Utils::GetFileExtension(filePath));
        return extension == L".tar";
    }

    bool TarExtractor::GetArchiveInfo(const std::wstring& filePath, std::vector<ArchiveEntry>& entries) const {
        entries.clear();
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        TarHeader header;
        while (ReadTarHeader(file, header)) {
            if (!header.IsValid()) {
                // Skip invalid headers or end of archive
                continue;
            }

            ArchiveEntry entry;
            entry.name = header.GetFileName();
            entry.size = header.GetFileSize();
            entry.compressedSize = header.GetFileSize(); // TAR is uncompressed
            entry.isDirectory = header.IsDirectory();
            entry.lastModified = header.GetModificationTime();
            entry.permissions = header.GetPermissions();
            entry.linkTarget = header.GetLinkName();

            entries.push_back(entry);

            // Skip file data
            uint64_t fileSize = header.GetFileSize();
            if (fileSize > 0) {
                // Calculate blocks (TAR uses 512-byte blocks)
                uint64_t blocks = (fileSize + 511) / 512;
                file.seekg(blocks * 512, std::ios::cur);
            }
        }

        return true;
    }

    ExtractionResult TarExtractor::Extract(
        const std::wstring& archivePath,
        const std::wstring& destinationPath,
        ProgressCallback callback) const {
        
        ExtractionResult result;
        result.success = false;
        result.bytesProcessed = 0;
        result.extractedFiles.clear();
        
        auto startTime = std::chrono::high_resolution_clock::now();

        try {
            // Ensure destination directory exists
            if (!Utils::CreateDirectoryRecursive(destinationPath)) {
                result.errorMessage = L"Failed to create destination directory: " + destinationPath;
                return result;
            }

            std::ifstream file(archivePath, std::ios::binary);
            if (!file.is_open()) {
                result.errorMessage = L"Cannot open archive file: " + archivePath;
                return result;
            }

            // Get total size for progress reporting
            uint64_t totalSize = GetTotalUncompressedSize(archivePath);
            uint64_t processedBytes = 0;

            TarHeader header;
            while (ReadTarHeader(file, header)) {
                if (!header.IsValid()) {
                    // Check if we've hit the end of the archive (two consecutive null blocks)
                    file.seekg(-512, std::ios::cur);
                    char testBlock[512];
                    file.read(testBlock, 512);
                    bool isNullBlock = true;
                    for (int i = 0; i < 512; ++i) {
                        if (testBlock[i] != 0) {
                            isNullBlock = false;
                            break;
                        }
                    }
                    if (isNullBlock) {
                        break; // End of archive
                    }
                    continue;
                }

                std::wstring fileName = header.GetFileName();
                std::wstring outputPath = Utils::CombinePath(destinationPath, fileName);

                // Security check
                if (!Utils::IsValidExtractionPath(destinationPath, fileName)) {
                    result.errorMessage = L"Security violation: Invalid path in archive: " + fileName;
                    return result;
                }

                // Sanitize the output path
                outputPath = Utils::CombinePath(destinationPath, Utils::SanitizePath(fileName));

                // Report progress
                if (callback) {
                    if (!callback(processedBytes, totalSize, fileName, L"Extracting")) {
                        result.errorMessage = L"Extraction cancelled by user";
                        return result;
                    }
                }

                if (header.IsDirectory()) {
                    // Extract directory
                    if (!Utils::CreateDirectoryRecursive(outputPath)) {
                        result.errorMessage = L"Failed to create directory: " + fileName + L" at " + outputPath;
                        return result;
                    }
                } else if (header.IsRegularFile()) {
                    // Extract regular file
                    if (!ExtractFile(file, header, outputPath, callback)) {
                        result.errorMessage = L"Failed to extract file: " + fileName;
                        return result;
                    }
                } else {
                    // Skip unsupported file types (symbolic links, etc.)
                    uint64_t fileSize = header.GetFileSize();
                    if (fileSize > 0) {
                        uint64_t blocks = (fileSize + 511) / 512;
                        file.seekg(blocks * 512, std::ios::cur);
                    }
                }

                result.extractedFiles.push_back(fileName);
                processedBytes += header.GetFileSize();
            }

            // Final progress update
            if (callback) {
                callback(totalSize, totalSize, L"", L"Complete");
            }

            result.success = true;
            result.bytesProcessed = processedBytes;

        } catch (const std::exception& e) {
            result.errorMessage = L"Exception during extraction: " + 
                std::wstring(e.what(), e.what() + strlen(e.what()));
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.timeElapsed = duration.count() / 1000.0;

        return result;
    }

    std::vector<std::wstring> TarExtractor::GetSupportedExtensions() const {
        return { L".tar" };
    }

    std::wstring TarExtractor::GetExtractorName() const {
        return L"TAR Extractor";
    }

    // Private helper methods
    bool TarExtractor::ReadTarHeader(std::ifstream& file, TarHeader& header) const {
        file.read(reinterpret_cast<char*>(&header), sizeof(TarHeader));
        return file.gcount() == sizeof(TarHeader);
    }

    bool TarExtractor::ValidateChecksum(const TarHeader& header) const {
        // Calculate checksum
        uint32_t calculatedChecksum = 0;
        const char* bytes = reinterpret_cast<const char*>(&header);
        
        for (size_t i = 0; i < sizeof(TarHeader); ++i) {
            if (i >= 148 && i < 156) {
                // Checksum field should be treated as spaces for calculation
                calculatedChecksum += ' ';
            } else {
                calculatedChecksum += static_cast<unsigned char>(bytes[i]);
            }
        }

        // Get stored checksum
        uint32_t storedChecksum = OctalToDecimal(header.checksum, sizeof(header.checksum));
        
        return calculatedChecksum == storedChecksum;
    }

    uint64_t TarExtractor::OctalToDecimal(const char* octal, size_t length) const {
        uint64_t result = 0;
        for (size_t i = 0; i < length && octal[i] != '\0' && octal[i] != ' '; ++i) {
            if (octal[i] >= '0' && octal[i] <= '7') {
                result = result * 8 + (octal[i] - '0');
            }
        }
        return result;
    }

    bool TarExtractor::ExtractFile(std::ifstream& tarFile, const TarHeader& header, 
                                  const std::wstring& outputPath, ProgressCallback callback) const {
        uint64_t fileSize = header.GetFileSize();
        
        // Create parent directory if it doesn't exist
        std::wstring parentDir = Utils::GetParentDirectory(outputPath);
        if (!parentDir.empty() && !Utils::CreateDirectoryRecursive(parentDir)) {
            return false;
        }

        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            return false;
        }

        const size_t bufferSize = 8192;
        char buffer[bufferSize];
        uint64_t bytesRemaining = fileSize;

        while (bytesRemaining > 0) {
            size_t bytesToRead = static_cast<size_t>(std::min(static_cast<uint64_t>(bufferSize), bytesRemaining));
            tarFile.read(buffer, bytesToRead);
            size_t bytesRead = tarFile.gcount();
            
            if (bytesRead == 0) {
                break;
            }

            outputFile.write(buffer, bytesRead);
            bytesRemaining -= bytesRead;
        }

        // Skip to next 512-byte boundary
        if (fileSize % 512 != 0) {
            uint64_t padding = 512 - (fileSize % 512);
            tarFile.seekg(padding, std::ios::cur);
        }

        return true;
    }

    bool TarExtractor::ExtractDirectory(const TarHeader& header, const std::wstring& outputPath) const {
        return Utils::CreateDirectoryRecursive(outputPath);
    }

    std::wstring TarExtractor::ConvertPath(const std::string& path) const {
        return std::wstring(path.begin(), path.end());
    }

    uint64_t TarExtractor::GetTotalUncompressedSize(const std::wstring& filePath) const {
        uint64_t totalSize = 0;
        
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            return 0;
        }

        TarHeader header;
        while (ReadTarHeader(file, header)) {
            if (!header.IsValid()) {
                continue;
            }

            totalSize += header.GetFileSize();

            // Skip file data
            uint64_t fileSize = header.GetFileSize();
            if (fileSize > 0) {
                uint64_t blocks = (fileSize + 511) / 512;
                file.seekg(blocks * 512, std::ios::cur);
            }
        }

        return totalSize;
    }

} // namespace ArchiveEngine