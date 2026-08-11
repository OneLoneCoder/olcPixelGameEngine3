#include "imload_ios.h"

//! START CUSTOMHEADER
#include "api_ios.h" // For ios_imageloader_getPixelData function
//! END CUSTOMHEADER

//! START IMPLEMENTATION
namespace olc::imload
{
    
    bool ImageLoader_iOS::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        if(!std::filesystem::exists(sFileName)) {
            
            // TODO: we need to get the full path to the file in the app bundle
            return false; // File does not exist
        }

        try {
            // Create macOS API wrapper image loader
            olc::apis::ios::ImageLoader loader;
            
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
            if (!image.CreateNoGPU({width, height})) {
                return false; // Failed to create image
            }
            
            // Clear and resize the pixel vector
            image.GetPixels().clear();
            image.GetPixels().resize(width * height);
            
            // Copy pixel data - assuming the loader provides RGBA data
            // The api_ios should provide RGBA format with 4 bytes per pixel
            std::memcpy(image.GetPixels().data(), pixelData, width * height * 4);
            
            return true;
        }
        catch (const std::exception& e) {
            // Handle any exceptions from the wrapper
            return false;
        }
    }

    bool ImageLoader_iOS::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        // TODO: Implement memory-based image loading for iOS
        return false;
    }

    bool ImageLoader_iOS::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        // TODO: Implement memory-based image loading for iOS
        return false;
    }

    bool ImageLoader_iOS::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
    {
        // TODO: Implement image writing for iOS
        return false;
    }

    bool ImageLoader_iOS::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
    {
        // TODO: Implement image memory writing for iOS
        return false;
    }
}
//! END IMPLEMENTATION
