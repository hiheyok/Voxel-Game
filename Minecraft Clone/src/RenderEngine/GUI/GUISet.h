#pragma once
#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Buffers/Buffer.h"
#include <vector>
#include "GUIObject.h"
#include "../OpenGL/Texture/Types/Texture2D.h"
#include "../../Level/Typenames.h"

class GUISet {
private:
    std::vector<GUIElement> elements_;

    size_t AddRenderingObj() {
        vaos_.emplace_back();
        ebos_.emplace_back();
        vbos_.emplace_back();
        vaos_.back().GenArray();
        vbos_.back().GenBuffer();
        ebos_.back().GenBuffer();

        vbos_.back().SetType(GL_ARRAY_BUFFER);
        ebos_.back().SetType(GL_ELEMENT_ARRAY_BUFFER);

        vbos_.back().SetUsage(GL_STATIC_DRAW);
        ebos_.back().SetUsage(GL_STATIC_DRAW);

        vaos_.back().Bind();
        vbos_.back().Bind();
        vaos_.back().EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 4, 0);
        vaos_.back().EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 4, 2);
        vbos_.back().Unbind();
        vaos_.back().Unbind();

        vbo_size_.emplace_back(0);
        return vaos_.size() - 1;
    }

public:

    GUISet() {}

    void Initialize() {
        
    }

    void Clean() {
        for (auto& vao : vaos_)
            vao.Delete();
        for (auto& vbo : vbos_)
            vbo.Delete();
        for (auto& ebo : ebos_)
            ebo.Delete();
    }

    void AddGUIElementNorm(std::string Name, std::string Text, glm::vec2 Size, glm::vec2 Position, glm::vec2 UV_P0, glm::vec2 UV_P1) {

        if (!is_initialized_) {
            Initialize();
            is_initialized_ = true;
        }

        if (gui_element_index_.find(Name) == gui_element_index_.end()) {
            elements_.emplace_back(Text, Size, Position);
            elements_.back().buffer_index_ = AddRenderingObj();
            elements_.back().uv_p0_ = UV_P0;
            elements_.back().uv_p1_ = UV_P1;
            gui_element_index_.emplace(Name, static_cast<int>(elements_.size() - 1));
            num_of_renderable_objects_++;
            is_dirty_ = true;
        }
        else {
            g_logger.LogError("GUISet::AddGUIElementNorm", "Element " + Name + " already exist!");
        }
    }

    void AddGUIElement(std::string name, std::string text, glm::vec2 size, glm::vec2 position, glm::vec2 UV_P0, glm::vec2 UV_P1) {

        if (!is_initialized_) {
            Initialize();
            is_initialized_ = true;
        }

        UV_P0.x = UV_P0.x / (float)gui_texture_.width_;
        UV_P1.x = UV_P1.x / (float)gui_texture_.width_;

        UV_P0.y = UV_P0.y / (float)gui_texture_.height_;
        UV_P1.y = UV_P1.y / (float)gui_texture_.height_;

        if (gui_element_index_.find(name) == gui_element_index_.end()) {
            elements_.emplace_back(text, size, position);
            elements_.back().buffer_index_ = AddRenderingObj();
            elements_.back().uv_p0_ = UV_P0;
            elements_.back().uv_p1_ = UV_P1;
            gui_element_index_.emplace(name, static_cast<int>(elements_.size() - 1));
            num_of_renderable_objects_++;
            is_dirty_ = true;
        }
        else {
            g_logger.LogError("GUISet::AddGUIElement", "Element " + name + " already exist!");
        }
    }

    void EditElementPosition(std::string Name, glm::vec2 Position) {
        if (gui_element_index_.find(Name) != gui_element_index_.end()) {
            int Index = gui_element_index_[Name];
            elements_[Index].location_ = Position;
            is_dirty_ = true;
        }
        else {
            g_logger.LogError("GUISet::EditElementPosition", "Element " + Name + " doesn't exist!");
        }
    }

    void EditElementUVNorm(std::string Name, glm::vec2 UV0, glm::vec2 UV1) {
        if (gui_element_index_.find(Name) != gui_element_index_.end()) {
            int Index = gui_element_index_[Name];
            elements_[Index].uv_p0_ = UV0;
            elements_[Index].uv_p1_ = UV1;
            is_dirty_ = true;
        }
        else {
            g_logger.LogError("GUISet::EditElementUVNorm", "Element " + Name + " doesn't exist!");
        }
    }

    void SetGUITexture(std::string file) {
        gui_texture_ = Texture2D(RawTextureData(file.c_str()));
    }

    void SetGUITexture(GLuint texture_id_, size_t x, size_t y) {
        gui_texture_.texture_id_ = texture_id_;
        gui_texture_.width_ = x;
        gui_texture_.height_ = y;
    }

    void PrepareRenderer() {
        if (!is_dirty_) {
            return;
        }
        is_dirty_ = false;

        for (auto& e : elements_) {
            
            GUIElement::GUIVertices GUIData = e.GetVertices();
            size_t BufferIndex = e.buffer_index_;
            vbos_[BufferIndex].InsertData(GUIData.vertices_.size() * sizeof(float), GUIData.vertices_.data(), GL_STATIC_DRAW);
            ebos_[BufferIndex].InsertData(GUIData.indices_.size() * sizeof(unsigned int), GUIData.indices_.data(), GL_STATIC_DRAW);
            vbo_size_[BufferIndex] = GUIData.indices_.size();
        }

        
    }

    std::vector<VertexArray> vaos_;
    std::vector<Buffer> vbos_;
    std::vector<Buffer> ebos_;
    std::vector<size_t> vbo_size_;

    int num_of_renderable_objects_ = 0;

    
    FastHashMap<std::string, int> gui_element_index_;
    bool is_dirty_ = true;

    Texture2D gui_texture_;

    bool is_initialized_ = false;
};