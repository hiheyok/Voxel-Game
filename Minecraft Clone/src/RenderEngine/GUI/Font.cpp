#include "Font.h"

glm::vec4 RenderableFont::GetCharUV(char c) {
    int index = (int)c;

    float width = GetCharWidth(c);

    float x0 = (float)(index & 0b1111);
    float y0 = (float)(index >> 4);

    float x1 = x0 + width;
    float y1 = y0 + 1.f;

    x0 = x0 / 16.f;
    y0 = y0 / 16.f;
    x1 = x1 / 16.f;
    y1 = y1 / 16.f;

    return glm::vec4{ x0, 1.f - y0, x1, 1.f - y1 };
}

float RenderableFont::GetCharWidth(char c) {
    return CharWidth[(int)c];
}


std::vector<float> RenderableFont::GetCharactorVertex(char c, float xOffset, float yOffset, float xOrigin, float yOrigin) {
    

    glm::vec4 uv_ = GetCharUV(c);

    float x0 = uv_.x;
    float y0 = uv_.y;
    float x1 = uv_.z;
    float y1 = uv_.w;

    float Width = GetCharWidth(c);

    std::vector<float> out = {
        0                    , 0                ,xOffset, yOffset, xOrigin, yOrigin, x0, y1, color_.x, color_.y, color_.z,
        0 + font_size_ * Width, 0                ,xOffset, yOffset, xOrigin, yOrigin, x1, y1, color_.x, color_.y, color_.z,
        0 + font_size_ * Width, 0 + font_size_    ,xOffset, yOffset, xOrigin, yOrigin, x1, y0, color_.x, color_.y, color_.z,
        0 + font_size_ * Width, 0 + font_size_    ,xOffset, yOffset, xOrigin, yOrigin, x1, y0, color_.x, color_.y, color_.z,
        0                    , 0 + font_size_    ,xOffset, yOffset, xOrigin, yOrigin, x0, y0, color_.x, color_.y, color_.z,
        0                    , 0                ,xOffset, yOffset, xOrigin, yOrigin, x0, y1, color_.x, color_.y, color_.z
    };

    return out;
}

std::vector<float> RenderableFont::GetVertices() {
    std::vector<float> out;

    float shift = font_size_;

    glm::vec2 coords = position_;

    float offsetValue = 0.f;

    for (int i = 0; i < render_text_.size(); i++) {
        char c = render_text_[i];
        float Width = GetCharWidth(c);
        float ShiftMag = shift * Width + spacing_ * shift;

        std::vector<float> charVert = GetCharactorVertex(render_text_[i], offsetValue, 0.f, coords.x, coords.y);

        offsetValue = offsetValue + ShiftMag;

        out.insert(out.end(), charVert.begin(), charVert.end());
        charVert.clear();
    }

    text_length_ = offsetValue;

    return out;
}

std::vector<float> RenderableFont::GetBackgroundVertices() {
    float r = background_color_.x;
    float g = background_color_.y;
    float b = background_color_.z;
    float a = background_alpha_;

    float xOrigin = position_.x;
    float yOrigin = position_.y;

    float W = background_width_padding_;
    float H = background_height_padding_;

    std::vector<float> out = {
        0 - W                , 0    - H                , xOrigin, yOrigin, r, g, b, a,
        0 + text_length_ + W    , 0 - H                , xOrigin, yOrigin, r, g, b, a,
        0 + text_length_ + W    , 0 + font_size_ + H    , xOrigin, yOrigin, r, g, b, a,
        0 + text_length_ + W    , 0 + font_size_ + H    , xOrigin, yOrigin, r, g, b, a,
        0 - W                , 0 + font_size_ + H    , xOrigin, yOrigin, r, g, b, a,
        0 - W                , 0 - H                , xOrigin, yOrigin, r, g, b, a,
    };

    return out;
}

void RenderableFont::setBackgroundPadding(float H, float W) {
    background_height_padding_ = H;
    background_width_padding_ = W;
}

void RenderableFont::SetText(std::string str) {
    render_text_ = str;
}

void RenderableFont::AddBackground(glm::vec3 color, float alpha) { //add background
    background_ = true;
    background_color_ = color;
    background_alpha_ = alpha;
}

void RenderableFont::FontSize(float size) {
    font_size_ = size;
}

void RenderableFont::SetColor(glm::vec3 clr) {
    color_ = clr;
}

void RenderableFont::SetPosition(glm::vec2 pos) {
    position_ = pos;
}