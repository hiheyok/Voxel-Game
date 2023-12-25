#include "Utils/LogUtils.h"

#include "Client/Client.h"
#include "World/Chunk/Block/Blocks.h"
#include "World/World.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"
#include "World/Chunk/Block/Blocks.h"
#include <cstdlib> 
#include <time.h>

int main() {
	
	stbi_set_flip_vertically_on_load(true);
	srand(time(NULL));//Set rng seed

	Client GameClient;
	GameClient.run();

	

	Logger.Stop();
	return 0;
}