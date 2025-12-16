
#include "imload_macos.h"

//! START IMPLEMENTATION
namespace olc::imload
{
    
    bool ImageLoader_MacOS::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        if(!std::filesystem::exists(sFileName)) {
            return false; // File does not exist
        }

        try {
            // Create macOS API wrapper image loader
            olc::apis::macos::ImageLoader loader;
            
            // Load the image file
            if (!loader.loadFromFile(sFileName) || !loader.isLoaded()) {
                return false; // Failed to load file
            }
            
            // Get image dimensions and info
            int width, height, bytesPerPixel;
            loader.getImageInfo(width, height, bytesPerPixel);
            
            if (width <= 0 || height <= 0) {
                return false; // Invalid dimensions
            }
            
            // Get raw pixel data from the loader
            unsigned char* pixelData = imageloader_getPixelData(loader.getCHandle());
            if (!pixelData) {
                return false; // Failed to get pixel data
            }
            
            // Create our olc::Image
            if (!image.Create({width, height})) {
                return false; // Failed to create image
            }
            
            // Clear and resize the pixel vector
            image.GetPixels().clear();
            image.GetPixels().resize(width * height);
            
            // Copy pixel data - assuming the loader provides RGBA data
            // The api_macos should provide RGBA format with 4 bytes per pixel
            std::memcpy(image.GetPixels().data(), pixelData, width * height * 4);
            
            return true;
        }
        catch (const std::exception& e) {
            // Handle any exceptions from the wrapper
            return false;
        }
    }

    bool ImageLoader_MacOS::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        return false;
    }

    bool ImageLoader_MacOS::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return false;
    }

    bool ImageLoader_MacOS::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
    {
        return false;
    }

    bool ImageLoader_MacOS::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return false;
    }
}
//! END IMPLEMENTATION
