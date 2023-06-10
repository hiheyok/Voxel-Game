#include "ImageLoader.h"
#include "../../../Utils/LogUtils.h"

RawTextureData GetImageData(const char* path) {
	RawTextureData Data;
	Data.data = stbi_load(path, &Data.width, &Data.height, &Data.format, 0);
	switch (Data.format) {
	case 1:
		Data.format = GL_RED;
		break;
	case 3:
		Data.format = GL_RGB;
		break;
	case 4:
		Data.format = GL_RGBA;
		break;
	default:
		getLogger()->LogWarn("Image Loader","Image invalid format");
	}
	return Data;
}