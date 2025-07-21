#include "ArchiveExtractor.h"
#include "TarExtractor.h"
#include <algorithm>

namespace ArchiveEngine {

    ArchiveType ArchiveExtractorFactory::DetectArchiveType(const std::wstring& filePath) {
        std::wstring extension = Utils::ToLowerCase(Utils::GetFileExtension(filePath));
        
        if (extension == L".tar") {
            return ArchiveType::Tar;
        } else if (extension == L".tar.gz" || extension == L".tgz") {
            return ArchiveType::TarGzip;
        } else if (extension == L".tar.bz2" || extension == L".tbz2") {
            return ArchiveType::TarBzip2;
        } else if (extension == L".gz") {
            return ArchiveType::Gzip;
        } else if (extension == L".bz2") {
            return ArchiveType::Bzip2;
        }
        
        return ArchiveType::Unknown;
    }

    std::unique_ptr<IArchiveExtractor> ArchiveExtractorFactory::CreateExtractor(ArchiveType type) {
        switch (type) {
        case ArchiveType::Tar:
            return std::make_unique<TarExtractor>();
        
        case ArchiveType::TarGzip:
        case ArchiveType::TarBzip2:
        case ArchiveType::Gzip:
        case ArchiveType::Bzip2:
            // TODO: Implement compressed extractors in next iteration
            // For now, return nullptr to indicate unsupported
            return nullptr;
        
        default:
            return nullptr;
        }
    }

    std::unique_ptr<IArchiveExtractor> ArchiveExtractorFactory::CreateExtractor(const std::wstring& filePath) {
        ArchiveType type = DetectArchiveType(filePath);
        return CreateExtractor(type);
    }

    std::vector<std::wstring> ArchiveExtractorFactory::GetAllSupportedExtensions() {
        std::vector<std::wstring> extensions;
        
        // Add extensions from all available extractors
        
        // TAR extractor
        auto tarExtractor = std::make_unique<TarExtractor>();
        auto tarExtensions = tarExtractor->GetSupportedExtensions();
        extensions.insert(extensions.end(), tarExtensions.begin(), tarExtensions.end());
        
        // TODO: Add other extractors when implemented
        // extensions.push_back(L".gz");
        // extensions.push_back(L".bz2");
        // extensions.push_back(L".tar.gz");
        // extensions.push_back(L".tar.bz2");
        // extensions.push_back(L".tgz");
        // extensions.push_back(L".tbz2");
        
        // Remove duplicates
        std::sort(extensions.begin(), extensions.end());
        extensions.erase(std::unique(extensions.begin(), extensions.end()), extensions.end());
        
        return extensions;
    }

} // namespace ArchiveEngine