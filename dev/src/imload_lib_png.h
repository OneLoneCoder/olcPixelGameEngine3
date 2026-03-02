#pragma once

//! START CUSTOMHEADER
#include "config.h"
#include "imload_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <filesystem>
//! END STDHEADER

//! START DECLARATION
#if !defined(PGE_IMAGELOADER_LIB_PNG_DECLARED)
#include <png.h>

#if OLC_HOST == OLC_HOST_LINUX_X11

#define ZEROPNG_DECLARE_SYM(func) static decltype(func)* zero_##func;
#define ZEROPNG_LOAD_SYM(lib, sym) \
    *reinterpret_cast<void**>(&zero_##sym) = dlsym(lib, #sym);
#define ZEROPNG(func) zero_##func

ZEROPNG_DECLARE_SYM(png_read_info)
ZEROPNG_DECLARE_SYM(png_get_image_width)
ZEROPNG_DECLARE_SYM(png_get_image_height)
ZEROPNG_DECLARE_SYM(png_get_color_type)
ZEROPNG_DECLARE_SYM(png_get_bit_depth)
ZEROPNG_DECLARE_SYM(png_set_palette_to_rgb)
ZEROPNG_DECLARE_SYM(png_set_strip_16)
ZEROPNG_DECLARE_SYM(png_set_expand_gray_1_2_4_to_8)
ZEROPNG_DECLARE_SYM(png_set_tRNS_to_alpha)
ZEROPNG_DECLARE_SYM(png_get_valid)
ZEROPNG_DECLARE_SYM(png_set_filler)
ZEROPNG_DECLARE_SYM(png_set_gray_to_rgb)
ZEROPNG_DECLARE_SYM(png_get_rowbytes)
ZEROPNG_DECLARE_SYM(png_read_image)
ZEROPNG_DECLARE_SYM(png_destroy_read_struct)
ZEROPNG_DECLARE_SYM(png_create_read_struct)
ZEROPNG_DECLARE_SYM(png_create_info_struct)
ZEROPNG_DECLARE_SYM(png_init_io)
ZEROPNG_DECLARE_SYM(png_read_update_info)
ZEROPNG_DECLARE_SYM(png_set_longjmp_fn)


#else
#define ZEROPNG(func) func
#endif

namespace olc::imload
{
    class ImageLoader_LibPNG : public ImageLoader
    {	
    public:
        ImageLoader_LibPNG();
        ~ImageLoader_LibPNG();

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
    private:
    #if OLC_HOST == OLC_HOST_LINUX_X11
        void* libpng_handle{nullptr};
    #endif
    };
}

#define PGE_IMAGELOADER_LIB_PNG_DECLARED 1
#endif
//! END DECLARATION
