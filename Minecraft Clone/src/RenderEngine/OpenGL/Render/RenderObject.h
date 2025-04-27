#include <memory>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

/*
Top abstract class for OpenGL Rendeirng
Cannot be copied
*/

class RenderObject {
public:
    RenderObject() noexcept;
    RenderObject(RenderObject&&) noexcept;
    RenderObject& operator=(RenderObject&&) noexcept;
    virtual ~RenderObject() noexcept;

    RenderObject(const RenderObject&) noexcept = delete;

    RenderObject& operator=(const RenderObject&) noexcept = delete;

    virtual void Render() = 0;

    void SetShader(std::unique_ptr<ShaderInterface> shader);

    // Use to configure shader before rendering
    ShaderInterface& GetShader();
protected:
    static constexpr size_t GetDataTypeSize(uint32_t data_type) {
        switch (data_type) {
            case GL_BYTE:
                return 1;
            case GL_UNSIGNED_BYTE:
                return 1;
            case GL_SHORT:
                return 2;
            case GL_UNSIGNED_SHORT:
                return 2;
            case GL_INT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_FIXED:
                return 4;
            case GL_HALF_FLOAT:
                return 2;
            case GL_FLOAT:
                return 4;
            case GL_DOUBLE:
                return 8;
            default:
                throw std::runtime_error("Invalid datatype");
        }
    }
private:
    std::unique_ptr<ShaderInterface> shader_;

};