#include "ArchiveExtractor.h"
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <windows.h>

namespace ArchiveEngine {
    namespace Utils {

        bool CreateDirectoryRecursive(const std::wstring& path) {
            std::error_code ec;
            bool result = std::filesystem::create_directories(path, ec);
            if (ec) {
                // If it failed, check if directory already exists
                return std::filesystem::exists(path) && std::filesystem::is_directory(path);
            }
            return true; // Either created successfully or already exists
        }

        bool FileExists(const std::wstring& path) {
            return std::filesystem::exists(path);
        }

        bool IsDirectory(const std::wstring& path) {
            return std::filesystem::is_directory(path);
        }

        uint64_t GetFileSize(const std::wstring& path) {
            std::error_code ec;
            auto size = std::filesystem::file_size(path, ec);
            return ec ? 0 : size;
        }

        std::wstring GetFileName(const std::wstring& path) {
            std::filesystem::path p(path);
            return p.filename().wstring();
        }

        std::wstring GetFileExtension(const std::wstring& path) {
            std::filesystem::path p(path);
            std::wstring extension = p.extension().wstring();
            
            // Handle compound extensions like .tar.gz
            if (extension == L".gz" || extension == L".bz2") {
                std::wstring stem = p.stem().wstring();
                if (stem.length() >= 4 && 
                    stem.compare(stem.length() - 4, 4, L".tar") == 0) {
                    extension = L".tar" + extension;
                }
            }
            
            return extension;
        }

        std::wstring GetParentDirectory(const std::wstring& path) {
            std::filesystem::path p(path);
            return p.parent_path().wstring();
        }

        std::wstring CombinePath(const std::wstring& basePath, const std::wstring& relativePath) {
            std::filesystem::path base(basePath);
            std::filesystem::path relative(relativePath);
            return (base / relative).wstring();
        }

        bool IsValidExtractionPath(const std::wstring& basePath, const std::wstring& entryPath) {
            // Prevent directory traversal attacks
            if (entryPath.find(L"..") != std::wstring::npos) {
                return false;
            }
            
            // Check for absolute paths
            std::filesystem::path entry(entryPath);
            if (entry.is_absolute()) {
                return false;
            }
            
            // Ensure the final path is within the base directory
            std::filesystem::path finalPath = std::filesystem::path(basePath) / entry;
            std::filesystem::path canonicalBase = std::filesystem::canonical(basePath);
            std::filesystem::path canonicalFinal;
            
            std::error_code ec;
            canonicalFinal = std::filesystem::canonical(finalPath.parent_path(), ec);
            if (ec) {
                // Parent path doesn't exist yet, check lexically
                canonicalFinal = std::filesystem::weakly_canonical(finalPath);
            }
            
            // Check if the canonical final path starts with the canonical base path
            auto baseStr = canonicalBase.wstring();
            auto finalStr = canonicalFinal.wstring();
            
            if (finalStr.length() < baseStr.length()) {
                return false;
            }
            
            return finalStr.substr(0, baseStr.length()) == baseStr;
        }

        std::wstring SanitizePath(const std::wstring& path) {
            std::wstring sanitized = path;
            
            // Replace invalid characters with underscores
            const std::wstring invalidChars = L"<>:\"|?*";
            for (wchar_t& ch : sanitized) {
                if (invalidChars.find(ch) != std::wstring::npos || ch < 32) {
                    ch = L'_';
                }
            }
            
            // Remove leading/trailing whitespace and dots
            size_t start = sanitized.find_first_not_of(L" \t.");
            size_t end = sanitized.find_last_not_of(L" \t.");
            
            if (start == std::wstring::npos) {
                return L"unnamed";
            }
            
            sanitized = sanitized.substr(start, end - start + 1);
            
            // Ensure it's not a reserved name
            std::wstring upper = ToLowerCase(sanitized);
            const std::vector<std::wstring> reserved = {
                L"con", L"prn", L"aux", L"nul",
                L"com1", L"com2", L"com3", L"com4", L"com5", L"com6", L"com7", L"com8", L"com9",
                L"lpt1", L"lpt2", L"lpt3", L"lpt4", L"lpt5", L"lpt6", L"lpt7", L"lpt8", L"lpt9"
            };
            
            for (const auto& res : reserved) {
                if (upper == res) {
                    sanitized += L"_";
                    break;
                }
            }
            
            return sanitized;
        }

        std::wstring ToLowerCase(const std::wstring& str) {
            std::wstring lower = str;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
            return lower;
        }

        bool EndsWith(const std::wstring& str, const std::wstring& suffix) {
            if (suffix.length() > str.length()) {
                return false;
            }
            return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
        }

        std::vector<std::wstring> SplitPath(const std::wstring& path) {
            std::vector<std::wstring> components;
            std::filesystem::path p(path);
            
            for (const auto& component : p) {
                if (component != L"/" && component != L"\\") {
                    components.push_back(component.wstring());
                }
            }
            
            return components;
        }

        std::wstring FormatDuration(double seconds) {
            if (seconds < 1.0) {
                return std::to_wstring(static_cast<int>(seconds * 1000)) + L"ms";
            } else if (seconds < 60.0) {
                return std::to_wstring(static_cast<int>(seconds)) + L"s";
            } else {
                int minutes = static_cast<int>(seconds / 60);
                int secs = static_cast<int>(seconds) % 60;
                return std::to_wstring(minutes) + L"m " + std::to_wstring(secs) + L"s";
            }
        }

        std::wstring FormatFileSize(uint64_t bytes) {
            const wchar_t* units[] = { L"B", L"KB", L"MB", L"GB", L"TB" };
            int unitIndex = 0;
            double size = static_cast<double>(bytes);
            
            while (size >= 1024.0 && unitIndex < 4) {
                size /= 1024.0;
                unitIndex++;
            }
            
            std::wstringstream ss;
            ss << std::fixed << std::setprecision(unitIndex == 0 ? 0 : 1) << size << units[unitIndex];
            return ss.str();
        }

    } // namespace Utils

    // Exception implementations
    ArchiveException::ArchiveException(const std::wstring& msg) {
        // Convert wide string to narrow string
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        message = converter.to_bytes(msg);
    }

    UnsupportedFormatException::UnsupportedFormatException(const std::wstring& format) 
        : ArchiveException(L"Unsupported archive format: " + format) {}

    ExtractionException::ExtractionException(const std::wstring& details)
        : ArchiveException(L"Extraction failed: " + details) {}

    SecurityException::SecurityException(const std::wstring& details)
        : ArchiveException(L"Security violation: " + details) {}

} // namespace ArchiveEngine