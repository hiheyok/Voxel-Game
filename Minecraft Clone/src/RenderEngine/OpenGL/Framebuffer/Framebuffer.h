#pragma once
#include <vector>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../Shader/Shader.h"
#include "../../../Utils/LogUtils.h"

class TexturedFrameBuffer {
public:

    GLuint fbo_, rbo_, texture_, shader_id_ = 0;
    GLint sx = 0;
    GLint sy = 0;

    GLint text_size_x_, text_size_y_;
    unsigned int quad_vao_, quad_vbo_ = 0;
    GLfloat res_multiplier_ = 1;

    Shader* screen_ = new Shader;

    void GenBuffer(GLint x, GLint y, float muti, GLuint format = GL_RGBA) {

        screen_->Init("assets/shaders/screen/vert.glsl", "assets/shaders/screen/frag.glsl");
        screen_->SetVec2("Resolution", glm::vec2((x), (y)));

        sy = y;
        sx = x;

        text_size_x_ = sx;
        text_size_y_ = sy;

        res_multiplier_ = muti;
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        // create a color attachment texture
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, format, (int)(x * muti), (int)(y * muti), 0, format, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        glGenRenderbuffers(1, &rbo_);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)(x * muti), (int)(y * muti)); // use a single renderbuffer object for both a depth AND stencil buffer.
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            g_logger.LogError("Framebuffer", "Failed to create framebuffer!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        screen_->BindTexture2D(0, texture_, "screenTexture");
        shader_id_ = screen_->shader_id_;
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
          // positions   // texCoords
          -1.f,  1.0f,  0.0f, 1.0f,
          -1.f,  -1.f,  0.0f, 0.0f,
           1.f,  -1.f,  1.0f, 0.0f,

          -1.f,  1.0f,  0.0f, 1.0f,
           1.f,  -1.0f,  1.0f, 0.0f,
           1.f,  1.0f,  1.0f, 1.0f
        };


        glGenVertexArrays(1, &quad_vao_);
        glGenBuffers(1, &quad_vbo_);
        glBindVertexArray(quad_vao_);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        g_logger.LogDebug("Framebuffer", "Created new frame buffer: " + std::to_string(fbo_));
    }

    void UpdateResolution(GLint x, GLint y, float muti = 1.f) {
        sx = static_cast<GLint>(muti * x);
        sy = static_cast<GLint>(muti * y);
    }

    void BindRBO() {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    }

    void UnbindRBO() {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    }

    void BindFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, (int)(sx * res_multiplier_), (int)(sy * res_multiplier_));
        if (sx != text_size_x_ || sy != text_size_y_) {
            glBindTexture(GL_TEXTURE_2D, texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(sx * res_multiplier_), (int)(sy * res_multiplier_), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)(sx * res_multiplier_), (int)(sy * res_multiplier_));
            text_size_y_ = sy;
            text_size_x_ = sx;
        }
    }

    void UnbindFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, sx, sy);
        if (sx != text_size_x_ || sy != text_size_y_) {
            glBindTexture(GL_TEXTURE_2D, texture_);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(sx * res_multiplier_), (int)(sy * res_multiplier_), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)(sx * res_multiplier_), (int)(sy * res_multiplier_));
            text_size_y_ = sy;
            text_size_x_ = sx;
        }
    }

    void Clear() {
        glDeleteFramebuffers(1, &fbo_);
        glDeleteTextures(1, &texture_);
        glDeleteRenderbuffers(1, &rbo_);
    }

    void GetShaders() {
    
    }

    void UpdateTexture() {

    }


    void Render() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(shader_id_);
        glBindVertexArray(quad_vao_);
        glBindTexture(GL_TEXTURE_2D, texture_);    // use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }
};