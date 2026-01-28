#include "imload_android.h"

//! START IMPLEMENTATION
#include <android/imagedecoder.h>
#include <android/log.h>
#include <vector>

namespace olc::imload
{
    bool ImageLoader_NDKImageDecoder::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        AAsset* asset = AAssetManager_open(
            assetManager,
            sFileName.c_str(),
            AASSET_MODE_BUFFER
        );
        if (!asset) {
            return false;
        }

        AImageDecoder* decoder = nullptr;
        int status = AImageDecoder_createFromAAsset(
            asset,
            &decoder
        );

        if (status != ANDROID_IMAGE_DECODER_SUCCESS || !decoder) {
            AAsset_close(asset);
            return false;
        }

        const AImageDecoderHeaderInfo* info =
            AImageDecoder_getHeaderInfo(decoder);
        if (!info) {
            AImageDecoder_delete(decoder);
            AAsset_close(asset);
            return false;
        }

        image.Create({
            AImageDecoderHeaderInfo_getWidth(info),
            AImageDecoderHeaderInfo_getHeight(info)
        });

        AImageDecoder_setAndroidBitmapFormat(
            decoder,
            ANDROID_BITMAP_FORMAT_RGBA_8888
        );
        AImageDecoder_setUnpremultipliedRequired(decoder, true);

        std::vector<uint8_t> pixels(image.Size().x * image.Size().y * 4);

        status = AImageDecoder_decodeImage(
            decoder,
            pixels.data(),
            image.Size().x * 4,
            pixels.size()
        );

        AImageDecoder_delete(decoder);
        AAsset_close(asset);

        for (size_t i = 0; i < pixels.size(); i += 4) {
            int32_t x = static_cast<int32_t>(i / 4) % image.Size().x;
            int32_t y = static_cast<int32_t>(i / 4) / image.Size().x;
            uint8_t r = pixels[i];
            uint8_t g = pixels[i + 1];
            uint8_t b = pixels[i + 2];
            uint8_t a = pixels[i + 3];
            image.Pixel({x, y}) = olc::Pixel(r, g, b, a);
        }

        return status == ANDROID_IMAGE_DECODER_SUCCESS;
    }

    bool ImageLoader_NDKImageDecoder::CreateImageFromMemory(Image &image, const uint8_t *data, const size_t bytes)
    {
        AImageDecoder* decoder = nullptr;
        int status = AImageDecoder_createFromBuffer(
            data,
            bytes,
            &decoder
        );

        if (status != ANDROID_IMAGE_DECODER_SUCCESS || !decoder) {
            return false;
        }

        const AImageDecoderHeaderInfo* info =
            AImageDecoder_getHeaderInfo(decoder);
        if (!info) {
            AImageDecoder_delete(decoder);
            return false;
        }

        image.Create({
         AImageDecoderHeaderInfo_getWidth(info),
         AImageDecoderHeaderInfo_getHeight(info)
        });

        AImageDecoder_setAndroidBitmapFormat(
            decoder,
            ANDROID_BITMAP_FORMAT_RGBA_8888
        );
        AImageDecoder_setUnpremultipliedRequired(decoder, true);

        std::vector<uint8_t> pixels(image.Size().x * image.Size().y * 4);

        status = AImageDecoder_decodeImage(
            decoder,
            pixels.data(),
            image.Size().x * 4,
            pixels.size()
        );

        AImageDecoder_delete(decoder);

        for (size_t i = 0; i < pixels.size(); i += 4) {
            int32_t x = static_cast<int32_t>(i / 4) % image.Size().x;
            int32_t y = static_cast<int32_t>(i / 4) / image.Size().x;
            uint8_t r = pixels[i];
            uint8_t g = pixels[i + 1];
            uint8_t b = pixels[i + 2];
            uint8_t a = pixels[i + 3];
            image.Pixel({x, y}) = olc::Pixel(r, g, b, a);
        }

        return status == ANDROID_IMAGE_DECODER_SUCCESS;
    }

    bool ImageLoader_NDKImageDecoder::CreateImageFromMemory(Image &image, const std::vector<uint8_t> &data)
    {
        return CreateImageFromMemory(image, data.data(), data.size());
    }

    bool ImageLoader_NDKImageDecoder::WriteImageToFile(const Image &image, const std::string &sFileName)
    {
        // No solution for now
        return false;
    }

    bool ImageLoader_NDKImageDecoder::WriteImageToMemoryFile(Image &image, const std::vector<uint8_t> &data)
    {
        // No solution for now
        return false;
    }
}
//! END IMPLEMENTATION
