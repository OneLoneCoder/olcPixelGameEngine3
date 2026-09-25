#include "imload_ios.h"

//! START CUSTOMHEADER
#include "api_ios.h" // For ios_imageloader_getPixelData function
//! END CUSTOMHEADER

//! START IMPLEMENTATION
namespace olc::imload
{
    
    bool ImageLoader_iOS::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        try {
            
            // Create iOS API wrapper image loader
            olc::apis::ios::ImageLoader loader;
            
            // OK Working with mobile storage, we need to check for the file in various locations
            std::filesystem::path filePath(sFileName);
            
            // Lets check if the developer has provided a full path to the file
            if (!std::filesystem::exists(filePath)) {
                
                // File does not exist at the provided path, try to locate it in the app bundle
                char* cAppFilePath = imageloader_getApplicationPath(loader.getCHandle());
                if (cAppFilePath) {
                    
                    // we need the directory of the app bundle to search for the file
                    std::filesystem::path fsAppDir(cAppFilePath);
                    std::filesystem::path potentialPath = fsAppDir / sFileName;
                    std::filesystem::path normalizedPath = std::filesystem::weakly_canonical(potentialPath);
                    
                    // lets check if the file exists in the app bundle directory (Project Directory)
                    if(std::filesystem::exists(normalizedPath)) {
                        filePath = normalizedPath;
                    }
                    else
                    {
                        bool bFound = false;
                        // Try multiple locations
                        for(auto& entry : std::filesystem::directory_iterator(fsAppDir))
                        {
                            if (entry.is_directory()) {
                                potentialPath = entry.path() / sFileName;
                                normalizedPath = std::filesystem::weakly_canonical(potentialPath);
                                
                                if (std::filesystem::exists(normalizedPath)) {
                                    filePath = potentialPath;
                                    bFound = true;
                                    break;
                                }
                                
                            } // end is entry directory
                        } // end directory search
                      
                        if(!bFound) return false; // all done nothing found
                        
                    } // end (Project Directory) search

                }
                else
                {
                    return false; // Failed to get application path
                }
                    
            }
              
            
            // Convert back to string for the loader
            std::string fullPath = filePath.string();

            // Load the image file
            if (!loader.loadFromFile(fullPath) || !loader.isLoaded()) {
                return false; // Failed to load file
            }
            
            // Get image dimensions and info
            int width =0; int height = 0; int bytesPerPixel = 0;
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
            printf("Exception in CreateImageFromFile: %s\n", e.what());
            return false;
        }
    }

    bool ImageLoader_iOS::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        if(!data) return false;

        // Create macOS API wrapper image loader
        olc::apis::ios::ImageLoader loader;
        
        if (!loader.loadFromMemory(data, bytes) || !loader.isLoaded()) {
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
        
        // The api_macos will provide RGBA format with 4 bytes per pixel
        std::memcpy(image.GetPixels().data(), pixelData, width * height * 4);
        
        return true;

    }

    bool ImageLoader_iOS::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return CreateImageFromMemory(image, data.data(), data.size());
    }

    bool ImageLoader_iOS::WriteImageToFile(const olc::Image& image, const std::string& sFileName)
    {
        olc_IgnoreUnused(image, sFileName);
        return false;
    }

    bool ImageLoader_iOS::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
    {
        olc_IgnoreUnused(image, data);
        return false;
    }
}
//! END IMPLEMENTATION
