#define GLEW_STATIC
#include <gl/glew.h>
#include <GLFW/glfw3.h>


#include "Utils/LogUtils.h"
#include "Client/Client.h"
#include "Level/Chunk/Block/Blocks.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"
#include "Level/Chunk/Block/Blocks.h"
#include "Core/Initializer.h"
#include <cstdlib> 
#include <time.h>
#include "Core/Options/Option.h"

int main() {
    
    stbi_set_flip_vertically_on_load(true);
    srand(static_cast<unsigned int>(time(0)));//Set rng seed

    Initialize();

    Client GameClient;
    GameClient.run();

    Options options;

    g_logger.Stop();
    return 0;
}