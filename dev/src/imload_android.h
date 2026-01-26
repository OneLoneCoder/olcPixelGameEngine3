#pragma once

//! START CUSTOMHEADER
#include "imload_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
//! END STDHEADER

//! START DECLARATION
#include <android/asset_manager.h>

#if !defined(PGE_IMAGELOADER_NDK_IMAGEDECODER_DECLARED)
namespace olc::imload
{

    // Create an image resource based on an image file asset
    class ImageLoader_NDKImageDecoder : public ImageLoader
    {
    public:
        ImageLoader_NDKImageDecoder() = default;
        ImageLoader_NDKImageDecoder(AAssetManager* assetManager) : assetManager(assetManager) {}

        // Create an image resource based on an image file asset on disk
        bool CreateImageFromFile(olc::Image& image, const std::string& sFileName) override;

        // Create an image resource based on an image file asset in memory
        bool CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes) override;

        // Create an image resource based on an image file asset in memory
        bool CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data) override;

        // Store an image as a file asset on disk
        bool WriteImageToFile(const olc::Image& image, const std::string& sFileName) override;

        // Store an image as a file asset in memory
        bool WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data) override;

    protected:
        AAssetManager* assetManager = nullptr;
    };
}
#define PGE_IMAGELOADER_NDK_IMAGEDECODER_DECLARED 1
#endif
//! END DECLARATION
