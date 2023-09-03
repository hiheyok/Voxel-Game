#include "Client/Client.h"
#include "World/Chunk/Block/Blocks.h"
#include "World/World.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"
#include "Utils/LogUtils.h"

int main() {
	
	stbi_set_flip_vertically_on_load(true);

	getLogger()->Start();

	Client GameClient;
	GameClient.run();

	getLogger()->Stop();
	return 0;
}