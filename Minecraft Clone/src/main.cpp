#include "Utils/LogUtils.h"

#include "Client/Client.h"
#include "World/Chunk/Block/Blocks.h"
#include "World/World.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"


int main() {
	
	stbi_set_flip_vertically_on_load(true);

	Client GameClient;
	GameClient.run();

	Logger.Stop();
	return 0;
}