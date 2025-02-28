#pragma once
#include <vector>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "../Shader/Shader.h"
#include "../../../Utils/LogUtils.h"

class TexturedFrameBuffer {
public:

    GLuint FBO, RBO, texture, ShaderID = 0;
    GLint sx = 0;
    GLint sy = 0;

    GLint glTextSizeX, glTextSizeY;
    unsigned int quadVAO, quadVBO = 0;
    GLfloat smuti = 0;

    Shader* screen = new Shader;

    void genBuffer(GLint x, GLint y, float muti, GLuint FORMAT = GL_RGBA) {

        screen->init("assets/shaders/screen/vert.glsl", "assets/shaders/screen/frag.glsl");
        screen->setVec2("Resolution", glm::vec2((x), (y)));

        sy = y;
        sx = x;

        glTextSizeX = sx;
        glTextSizeY = sy;

        smuti = muti;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // create a color attachment texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, FORMAT, (int)(x * muti), (int)(y * muti), 0, FORMAT, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
        // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)(x * muti), (int)(y * muti)); // use a single renderbuffer object for both a depth AND stencil buffer.
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // now actually attach it
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Logger.LogError("Framebuffer", "Failed to create framebuffer!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        screen->bindTexture2D(0, texture, "screenTexture");
        ShaderID = screen->ShaderID;
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates. NOTE that this plane is now much smaller and at the top of the screen
          // positions   // texCoords
          -1.f,  1.0f,  0.0f, 1.0f,
          -1.f,  -1.f,  0.0f, 0.0f,
           1.f,  -1.f,  1.0f, 0.0f,

          -1.f,  1.0f,  0.0f, 1.0f,
           1.f,  -1.0f,  1.0f, 0.0f,
           1.f,  1.0f,  1.0f, 1.0f
        };


        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        Logger.LogDebug("Framebuffer", "Created new frame buffer: " + std::to_string(FBO));
    }

    void UpdateResolution(GLint x, GLint y, float muti = 1.f) {
        sx = static_cast<GLint>(muti * x);
        sy = static_cast<GLint>(muti * y);
    }

    void bindRBO() {
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    }

    void unbindRBO() {
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    }

    void bindFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, (int)(sx * smuti), (int)(sy * smuti));
        if (sx != glTextSizeX || sy != glTextSizeY) {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(sx * smuti), (int)(sy * smuti), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)(sx * smuti), (int)(sy * smuti));
            glTextSizeY = sy;
            glTextSizeX = sx;
        }
    }

    void unbindFBO() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, sx, sy);
        if (sx != glTextSizeX || sy != glTextSizeY) {
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)(sx * smuti), (int)(sy * smuti), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glBindRenderbuffer(GL_RENDERBUFFER, RBO);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)(sx * smuti), (int)(sy * smuti));
            glTextSizeY = sy;
            glTextSizeX = sx;
        }
    }

    void clear() {
        glDeleteFramebuffers(1, &FBO);
        glDeleteTextures(1, &texture);
        glDeleteRenderbuffers(1, &RBO);
    }

    void getShaders() {


    }

    void updateTexture() {

    }


    void render() {
        // screen->bindTexture2D(0, texture, "screenTexture");

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glUseProgram(ShaderID);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texture);	// use the color attachment texture as the texture of the quad plane
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        
    }
};