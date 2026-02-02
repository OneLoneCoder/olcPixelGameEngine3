#pragma once

//! START CUSTOMHEADER
#include "imload_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <filesystem>
//! END STDHEADER

//! START DECLARATION
#if OLC_HOST == OLC_HOST_ANDROID
#include <android/asset_manager.h>
#endif

#if !defined(PGE_IMAGELOADER_LIB_PNG_DECLARED)
namespace olc::imload
{
    class ImageLoader_STB_Image : public ImageLoader
    {
    public:
        ImageLoader_STB_Image() = default;
#if OLC_HOST == OLC_HOST_ANDROID
        ImageLoader_STB_Image(AAssetManager* assetManager) : assetManager(assetManager) {}
    protected:
        AAssetManager* assetManager = nullptr;
#endif
    
    public:
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

    };
}

#define PGE_IMAGELOADER_LIB_PNG_DECLARED 1
#endif
//! END DECLARATION
