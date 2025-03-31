#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

namespace Model {
    struct ModelData {
        std::vector<float> vertices_;
        std::vector<unsigned int> indices_;

        int vertices_count_ = 0;
    } ;

    struct UV {
        glm::vec2 p0_ = glm::vec2(0.f, 0.f);
        glm::vec2 p1_ = glm::vec2(0.f, 0.f);
    } ;

    struct RectangularPrism {

        RectangularPrism(float x, float y, float z) {
            size_.x = x;
            size_.y = y;
            size_.z = z;
        }

        RectangularPrism(glm::vec3 size, glm::vec3 offset) {
            size_ = size;
            offset_ = offset;
        }

        ModelData GetVertices();

        glm::vec3 offset_ = glm::vec3(0.f, 0.f, 0.f);

        glm::vec3 size_ = glm::vec3(0.f, 0.f, 0.f);

        UV uv_map_[6]{};
    };
}

class EntityModel {
public:

    Model::RectangularPrism* AddRectangle(glm::vec3 size, glm::vec3 offset) {
        shapes_.emplace_back(size, offset);
        return &shapes_.back();
    }

    Model::ModelData GetVertices() {

        Model::ModelData model;

        for (Model::RectangularPrism& shape : shapes_) {
            Model::ModelData SubModel = shape.GetVertices();

            size_t CurrentIndex = model.vertices_count_;

            for (size_t i = 0; i < SubModel.vertices_.size(); i++) {
                if ((i % 5) < 3) {
                    model.vertices_.push_back(SubModel.vertices_[i] + shape.offset_[i % 5]);
                }
                else {
                    model.vertices_.push_back(SubModel.vertices_[i]);
                }
                
            }

            for (size_t i = 0; i < SubModel.indices_.size(); i++) {
                model.indices_.push_back(static_cast<uint32_t>(SubModel.indices_[i] + CurrentIndex));
            }

            model.vertices_count_ += SubModel.vertices_count_;

        }

        return model;
    }

    void Clear() {
        shapes_.clear();
    }

private:
    std::vector<Model::RectangularPrism> shapes_;
};