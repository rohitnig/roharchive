#pragma once

#include "ArchiveExtractor.h"
#include <fstream>

namespace ArchiveEngine {

    // TAR header structure (POSIX TAR format)
    struct TarHeader {
        char name[100];        // File name
        char mode[8];          // File mode (octal)
        char uid[8];           // User ID (octal)
        char gid[8];           // Group ID (octal)
        char size[12];         // File size in bytes (octal)
        char mtime[12];        // Last modification time (octal)
        char checksum[8];      // Header checksum (octal)
        char typeflag;         // File type flag
        char linkname[100];    // Link name
        char magic[6];         // POSIX TAR magic ("ustar\0")
        char version[2];       // POSIX TAR version ("00")
        char uname[32];        // User name
        char gname[32];        // Group name
        char devmajor[8];      // Device major number
        char devminor[8];      // Device minor number
        char prefix[155];      // Filename prefix
        char padding[12];      // Padding to 512 bytes
        
        // Helper methods
        bool IsValid() const;
        uint64_t GetFileSize() const;
        uint64_t GetModificationTime() const;
        std::wstring GetFileName() const;
        std::wstring GetLinkName() const;
        bool IsDirectory() const;
        bool IsRegularFile() const;
        bool IsSymbolicLink() const;
        uint32_t GetPermissions() const;
    };

    // TAR archive extractor
    class TarExtractor : public IArchiveExtractor {
    public:
        TarExtractor() = default;
        virtual ~TarExtractor() = default;

        // IArchiveExtractor implementation
        bool CanExtract(const std::wstring& filePath) const override;
        bool GetArchiveInfo(const std::wstring& filePath, std::vector<ArchiveEntry>& entries) const override;
        ExtractionResult Extract(
            const std::wstring& archivePath,
            const std::wstring& destinationPath,
            ProgressCallback callback = nullptr) const override;
        std::vector<std::wstring> GetSupportedExtensions() const override;
        std::wstring GetExtractorName() const override;

    private:
        // Helper methods
        bool ReadTarHeader(std::ifstream& file, TarHeader& header) const;
        bool ValidateChecksum(const TarHeader& header) const;
        uint64_t OctalToDecimal(const char* octal, size_t length) const;
        bool ExtractFile(std::ifstream& tarFile, const TarHeader& header, 
                        const std::wstring& outputPath, ProgressCallback callback) const;
        bool ExtractDirectory(const TarHeader& header, const std::wstring& outputPath) const;
        std::wstring ConvertPath(const std::string& path) const;
        uint64_t GetTotalUncompressedSize(const std::wstring& filePath) const;
    };

    // File type flags for TAR format
    namespace TarFileType {
        constexpr char RegularFile = '0';
        constexpr char AlternateRegularFile = '\0';  // Some implementations use null
        constexpr char HardLink = '1';
        constexpr char SymbolicLink = '2';
        constexpr char CharacterSpecial = '3';
        constexpr char BlockSpecial = '4';
        constexpr char Directory = '5';
        constexpr char FIFO = '6';
        constexpr char ContiguousFile = '7';
        constexpr char GlobalPAXHeader = 'g';
        constexpr char PAXHeader = 'x';
    }

} // namespace ArchiveEngine