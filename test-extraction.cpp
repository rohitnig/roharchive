#include "src/extraction-engine/ArchiveExtractor.h"
#include <iostream>
#include <filesystem>

int wmain(int argc, wchar_t* argv[]) {
    if (argc != 3) {
        std::wcout << L"Usage: test-extraction.exe <archive-file> <destination-directory>" << std::endl;
        return 1;
    }

    std::wstring archivePath = argv[1];
    std::wstring destPath = argv[2];

    std::wcout << L"Testing extraction of: " << archivePath << std::endl;
    std::wcout << L"To destination: " << destPath << std::endl;

    try {
        // Create extractor
        auto extractor = ArchiveEngine::ArchiveExtractorFactory::CreateExtractor(archivePath);
        
        if (!extractor) {
            std::wcout << L"ERROR: No suitable extractor found for this archive format." << std::endl;
            return 1;
        }

        std::wcout << L"Using extractor: " << extractor->GetExtractorName() << std::endl;

        // Check if we can extract
        if (!extractor->CanExtract(archivePath)) {
            std::wcout << L"ERROR: Extractor cannot handle this file." << std::endl;
            return 1;
        }

        // Get archive info first
        std::vector<ArchiveEngine::ArchiveEntry> entries;
        if (!extractor->GetArchiveInfo(archivePath, entries)) {
            std::wcout << L"ERROR: Failed to read archive information." << std::endl;
            return 1;
        }

        std::wcout << L"Archive contains " << entries.size() << L" entries:" << std::endl;
        for (const auto& entry : entries) {
            std::wcout << L"  " << (entry.isDirectory ? L"[DIR] " : L"[FILE]") 
                      << entry.name << L" (" << ArchiveEngine::Utils::FormatFileSize(entry.size) << L")" << std::endl;
        }

        // Create destination directory
        std::filesystem::create_directories(destPath);

        // Extract with progress callback
        auto progressCallback = [](uint64_t current, uint64_t total, const std::wstring& fileName, const std::wstring& operation) -> bool {
            if (total > 0) {
                int percent = static_cast<int>((current * 100) / total);
                std::wcout << L"\r" << operation << L": " << fileName << L" (" << percent << L"%)";
                std::wcout.flush();
            }
            return true; // Continue
        };

        std::wcout << L"\nStarting extraction..." << std::endl;
        auto result = extractor->Extract(archivePath, destPath, progressCallback);

        std::wcout << std::endl; // New line after progress

        if (result.success) {
            std::wcout << L"SUCCESS! Extracted " << result.extractedFiles.size() << L" files." << std::endl;
            std::wcout << L"Processed: " << ArchiveEngine::Utils::FormatFileSize(result.bytesProcessed) << std::endl;
            std::wcout << L"Time: " << ArchiveEngine::Utils::FormatDuration(result.timeElapsed) << std::endl;
            
            std::wcout << L"\nExtracted files:" << std::endl;
            for (const auto& file : result.extractedFiles) {
                std::wcout << L"  " << file << std::endl;
            }
        } else {
            std::wcout << L"FAILED: " << result.errorMessage << std::endl;
            return 1;
        }

    } catch (const ArchiveEngine::ArchiveException& e) {
        std::wcout << L"Archive Exception: " << std::wstring(e.what(), e.what() + strlen(e.what())) << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::wcout << L"Exception: " << std::wstring(e.what(), e.what() + strlen(e.what())) << std::endl;
        return 1;
    }

    return 0;
}