#include "ImageLoader.h"
#include "../../../Utils/LogUtils.h"

RawTextureData GetImageData(const char* path) {
    RawTextureData data;
    data.data_ = stbi_load(path, &data.width_, &data.height_, &data.format_, 0);
    switch (data.format_) {
    case 1:
        data.format_ = GL_RED;
        g_logger.LogDebug("GetImageData", "Loaded image: " + std::string(path));
        break;
    case 2:
        data.format_ = GL_RG;
        g_logger.LogDebug("GetImageData", "Loaded image: " + std::string(path));
        break;
    case 3:
        data.format_ = GL_RGB;
        g_logger.LogDebug("GetImageData", "Loaded image: " + std::string(path));
        break;
    case 4:
        data.format_ = GL_RGBA;
        g_logger.LogDebug("GetImageData", "Loaded image: " + std::string(path));
        break;
    default:
        g_logger.LogWarn("GetImageData","Image invalid format");
    }
    
    return data;
}