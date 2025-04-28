#include <vector>

/*
Vertices Format
- 2 floats for relative position in screen
- 2 float for UV
- 1 float for TextureID
*/

struct DisplayVertices {
    std::vector<float> vertices_;
    std::vector<unsigned int> indices_;
};