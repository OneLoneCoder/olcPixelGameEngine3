#pragma once

//! START CUSTOMHEADER
#include "imload_iface.h"
#include "api_ios_wrapper.hpp"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <filesystem>
//! END STDHEADER


//! START DECLARATION
#if !defined(PGE_IMAGELOADER_IOS_DECLARED)
namespace olc
{
    namespace imload
    {
        class ImageLoader_iOS : public ImageLoader
        {
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
}

#define PGE_IMAGELOADER_IOS_DECLARED 1
#endif
//! END DECLARATION


