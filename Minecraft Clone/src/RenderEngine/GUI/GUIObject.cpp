#include "RenderEngine/GUI/GUIObject.h"

GUIElement::GUIElement() = default;

GUIElement::GUIElement(std::string text, glm::vec2 size, glm::vec2 location) : text_(text), size_(size), location_(location) {

}

GUIElement::GUIVertices GUIElement::GetVertices() {
    GUIVertices data;
    uv_p0_.y = 1.f - uv_p0_.y;
    uv_p1_.y = 1.f - uv_p1_.y;

    data.vertices_.insert(data.vertices_.end(),

        {
            location_.x - (size_.x / 2.f), location_.y - (size_.y / 2.f), uv_p0_.x, uv_p1_.y,
            location_.x + (size_.x / 2.f), location_.y - (size_.y / 2.f), uv_p1_.x, uv_p1_.y,
            location_.x + (size_.x / 2.f), location_.y + (size_.y / 2.f), uv_p1_.x, uv_p0_.y,
            location_.x - (size_.x / 2.f), location_.y + (size_.y / 2.f), uv_p0_.x, uv_p0_.y,
        });

    data.indices_.insert(data.indices_.end(),
        {
            0, 1, 2,
            2, 3, 0
        }
    );


    uv_p0_.y = 1.f - uv_p0_.y;
    uv_p1_.y = 1.f - uv_p1_.y;

    return data;
}