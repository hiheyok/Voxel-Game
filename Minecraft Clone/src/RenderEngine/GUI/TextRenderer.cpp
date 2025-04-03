#include <exception>

#include "RenderEngine/GUI/TextRenderer.h"
#include "RenderEngine/GUI/Font.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Texture/Types/Texture2D.h"
#include "Utils/LogUtils.h"

TextRenderer::TextRenderer() :
    font_shader_{ std::make_unique<Shader>(
        "assets/shaders/Font/FontVert.glsl", 
        "assets/shaders/Font/FontFrag.glsl") },
    background_shader_{ std::make_unique<Shader>(
        "assets/shaders/Font/FontBackgroundVert.glsl", 
        "assets/shaders/Font/FontBackgroundFrag.glsl") },
    vbo_{ std::make_unique<Buffer>() },
    background_vbo_{ std::make_unique<Buffer>() },
    vao_{ std::make_unique<VertexArray>() },
    background_vao_{ std::make_unique<VertexArray>() },
    font_texture_{ std::make_unique<Texture2D>() } {}

TextRenderer::~TextRenderer() = default;


void TextRenderer::InitializeTextRenderer(GLFWwindow* w) {
    //Setup buffer for text rendering
    vbo_->GenBuffer();
    vao_->GenArray();

    vbo_->SetType(GL_ARRAY_BUFFER);
    vbo_->SetUsage(GL_STATIC_DRAW);

    vao_->Bind();
    vbo_->Bind();
    vao_->EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 11, 0);
    vao_->EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 11, 2);
    vao_->EnableAttriPTR(2, 2, GL_FLOAT, GL_FALSE, 11, 4);
    vao_->EnableAttriPTR(3, 2, GL_FLOAT, GL_FALSE, 11, 6);
    vao_->EnableAttriPTR(4, 3, GL_FLOAT, GL_FALSE, 11, 8);
    vao_->Unbind();
    vbo_->Unbind();

    //Setup buffer for background rendering
    background_vbo_->GenBuffer();
    background_vao_->GenArray();

    background_vbo_->SetType(GL_ARRAY_BUFFER);
    background_vbo_->SetUsage(GL_STATIC_DRAW);

    background_vao_->Bind();
    background_vbo_->Bind();
    background_vao_->EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 8, 0);
    background_vao_->EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 8, 2);
    background_vao_->EnableAttriPTR(2, 4, GL_FLOAT, GL_FALSE, 8, 4);
    background_vao_->Unbind();
    background_vbo_->Unbind();

    //Load textures
    RawTextureData RawTexture;
    RawTexture.Load("assets/minecraft/textures/font/ascii.png");
    font_texture_->Load(RawTexture);
    font_shader_->BindTexture2D(0, font_texture_->get(), "FontTexture");
    window_ = w;
    g_logger.LogDebug("TextRenderer::InitializeTextRenderer", "Initialized font renderer");
}

void TextRenderer::InsertFontObject(std::string name, RenderableFont font) {
    if (font_map_.count(name)) {
        g_logger.LogError("TextRenderer::InsertFontObject", std::string("Font with the name " + name + " already exist!"));
    }

    font_map_[name] = font;
}

void TextRenderer::RemoveFontObject(std::string name) {
    if (!font_map_.count(name)) {
        g_logger.LogError("TextRenderer::RemoveFontObject", std::string("Font with the name " + name + " doesn't exist! Cannot remove."));
    }

    font_map_.erase(name);
}

void TextRenderer::EditFontText(std::string name, std::string text) {
    font_map_[name].SetText(text);
}

void TextRenderer::ConstructBuffer() {
    //Fill text buffer
    vertices_.clear();

    for (auto& Font : font_map_) {
        std::vector<float> fontVertices = Font.second.GetVertices();

        vertices_.insert(vertices_.end(), fontVertices.begin(), fontVertices.end());
    }

    vbo_->InsertData(vertices_.size() * sizeof(vertices_[0]), vertices_.data(), GL_STATIC_DRAW);
    vertices_count_ = vertices_.size() / 11;

    //Fill background buffer
    vertices_background_.clear();

    for (auto& Font : font_map_) {
        if (!Font.second.background_) continue;

        std::vector<float> backgroundVertices = Font.second.GetBackgroundVertices();

        vertices_background_.insert(vertices_background_.end(), backgroundVertices.begin(), backgroundVertices.end());
    }

    background_vbo_->InsertData(vertices_background_.size() * sizeof(vertices_background_[0]), vertices_background_.data(), GL_STATIC_DRAW);
    background_vert_count_ = vertices_background_.size() / 8;
}

void TextRenderer::Prepare() {
    font_shader_->Use();
    font_shader_->BindTexture2D(0, font_texture_->get(), "FontTexture");

    int Height, Width;
    glfwGetWindowSize(window_, &Width, &Height);

    font_shader_->SetFloat("AspectRatio", ((float)Height) / ((float)Width));

    background_shader_->Use();
    background_shader_->SetFloat("AspectRatio", ((float)Height) / ((float)Width));
}

void TextRenderer::RenderFont() {
    Prepare();
    background_shader_->Use();
    background_vao_->Bind();
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(background_vert_count_));

    font_shader_->Use();
    vao_->Bind();
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices_count_));
}