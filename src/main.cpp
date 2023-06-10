#include "Client/Client.h"
#include "World/Chunk/Block/Blocks.h"
#include "World/World.h"


int main() {
	
	World newWorld;
	newWorld.Generate();

	Client GameClient;
	GameClient.run();

	return 0;
}