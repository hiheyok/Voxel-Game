#pragma once
#include <glm/vec2.hpp>

#include "Core/Typenames.h"

struct MouseInputs {
    enum ButtonState {
        RELEASE, PRESS, HOLD
    };

    enum ScrollState {
        SCROLL_NONE, SCROLL_DOWN, SCROLL_UP
    };

    ButtonState left_ = RELEASE;
    ButtonState middle_ = RELEASE;
    ButtonState right_ = RELEASE;

    ScrollState scroll_direction_ = SCROLL_NONE; //-1 or 1

    glm::dvec2 displacement_ = glm::dvec2(0.f, 0.f);
    glm::dvec2 position_ = glm::dvec2(0.f, 0.f);
};


class UserInputs {
private:

    enum KeyStatus {
        PRESS, HOLD
    };

    FastHashMap<int, KeyStatus> keys_;

public:

    void UpdateAllKey();

    void PressIndividualKey(int key);

    void ReleaseIndividualKey(int key);
    
    bool CheckKey(int key) const;

    bool CheckKeyPress(int key) const;

    bool CheckKeyHold(int key) const;

    MouseInputs mouse_;
    
    float delta_ = 0.f;
};