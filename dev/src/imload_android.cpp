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
            AASSET_MODE_STREAMING
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

        status = AImageDecoder_decodeImage(
            decoder,
            image.GetPixels().data(),
            image.Size().x * 4,
            image.GetPixels().size()
        );

        AImageDecoder_delete(decoder);
        AAsset_close(asset);

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

        status = AImageDecoder_decodeImage(
            decoder,
            image.GetPixels().data(),
            image.Size().x * 4,
            image.GetPixels().size()
        );

        AImageDecoder_delete(decoder);

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
        return false;
    }
}
//! END IMPLEMENTATION
