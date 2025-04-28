#include "RenderEngine/OpenGL/Render/RenderObject.h"

class RenderDrawArrays : public RenderObject {
   public:
    // Triangles are the default draw type
    RenderDrawArrays(uint32_t draw_type = GL_TRIANGLES);
    ~RenderDrawArrays();

    void Render() override;

   private:
    uint32_t draw_type_ = 0;
};