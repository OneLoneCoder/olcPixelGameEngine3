#include "imload_stb_image.h"

//! START IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace olc::imload
{

    // Create an image resource based on an image file asset on disk
    bool ImageLoader_STB_Image::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        std::cout << "ImageLoader: using stb image to load " << sFileName << ".\n";

#if OLC_HOST == OLC_HOST_ANDROID
        AAsset* asset = AAssetManager_open(assetManager, sFileName, AASSET_MODE_BUFFER);
        if(!asset)
        {
            std::cout << "Error: failed to load image <" << sFileName << "> - file not found.\n";
            return false;
        }
    
        size_t size = AAsset_getLength(asset);
        unsigned char* buffer = new unsigned char[size];
        AAsset_read(asset, buffer, size);
        AAsset_close(asset);
        
        bool result = CreateImageFromMemory(image, buffer, size);
        delete[] buffer;
        
        return result;

#else
        stbi_uc* pixelData = nullptr;
        int width = 0, height = 0, cmp = 0;

        // Open file
        if(!std::filesystem::exists(sFileName))
        {
            std::cout << "Error: failed to load image <" << sFileName << "> - file not found.\n";
            return false;
        }
        
        pixelData = stbi_load(sFileName.c_str(), &width, &height, &cmp, 4);
        
        if(!pixelData)
        {
            std::cout << "Error: failed to load image <" << sFileName << "> - failed to allocate memory.\n";
            return false;
        }
        
        image.Create({width, height});
        std::memcpy(reinterpret_cast<void*>(image.Data()), pixelData, width * height * 4);

        delete[] pixelData;
#endif
        
        return true;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_STB_Image::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        stbi_uc* pixelData = nullptr;
        int width = 0, height = 0, cmp = 0;
        pixelData = stbi_load_from_memory(data, bytes, &width, &height, &cmp, 4);

        image.Create({width, height});
        std::memcpy(reinterpret_cast<void*>(image.Data()), pixelData, width * height * 4);
        
        delete[] pixelData;

        return true;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_STB_Image::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        stbi_uc* pixelData = nullptr;
        int width = 0, height = 0, cmp = 0;
        pixelData = stbi_load_from_memory(data.data(), data.size(), &width, &height, &cmp, 4);

        image.Create({width, height});
        std::memcpy(reinterpret_cast<void*>(image.Data()), pixelData, width * height * 4);
        
        delete[] pixelData;

        return false;
    }
    
    // Store an image as a file asset on disk
    bool ImageLoader_STB_Image::WriteImageToFile(const olc::Image& image, const std::string& sFileName) 
    {
        return false;
    }
    
    // Store an image as a file asset in memory
    bool ImageLoader_STB_Image::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return false;
    }

}
//! END IMPLEMENTATION
