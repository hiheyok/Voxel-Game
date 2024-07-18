#include "ImageLoader.h"
#include "../../../Utils/LogUtils.h"

RawTextureData GetImageData(const char* path) {
	RawTextureData Data;
	Data.data = stbi_load(path, &Data.width, &Data.height, &Data.format, 0);
	switch (Data.format) {
	case 1:
		Data.format = GL_RED;
		Logger.LogDebug("Image Loader", "Loaded image: " + std::string(path));
		break;
	case 2:
		Data.format = GL_RG;
		Logger.LogDebug("Image Loader", "Loaded image: " + std::string(path));
		break;
	case 3:
		Data.format = GL_RGB;
		Logger.LogDebug("Image Loader", "Loaded image: " + std::string(path));
		break;
	case 4:
		Data.format = GL_RGBA;
		Logger.LogDebug("Image Loader", "Loaded image: " + std::string(path));
		break;
	default:
		Logger.LogWarn("Image Loader","Image invalid format");
	}
	
	return Data;
}