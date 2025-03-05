#include "IO.h"

void UserInputs::UpdateAllKey() {
    for (const auto& key : keys_) {
        keys_[key.first] = HOLD;
    }

    mouse_.scroll_direction_ = mouse_.SCROLL_NONE;

    mouse_.displacement_ = glm::dvec2(0.0, 0.0);

    if (mouse_.left_ == mouse_.PRESS) {
        mouse_.left_ = mouse_.HOLD;
    }

    if (mouse_.right_ == mouse_.PRESS) {
        mouse_.right_ = mouse_.HOLD;
    }

    if (mouse_.middle_ == mouse_.PRESS) {
        mouse_.middle_ = mouse_.HOLD;
    }
}

void UserInputs::PressIndividualKey(int key) {
    if (keys_.count(key)) {
        keys_[key] = HOLD;
    } else {
        keys_[key] = PRESS;
    }
}

void UserInputs::ReleaseIndividualKey(int key) {
    if (keys_.count(key)) {
        keys_.erase(key);
    }
}

bool UserInputs::CheckKey(int key) const {
    if (keys_.count(key)) {
        return true;
    }
    return false;
}

bool UserInputs::CheckKeyPress(int key) const {
    if (!CheckKey(key)) {
        return false;
    }
    auto it = keys_.find(key);
    return it->second == PRESS;
}

bool UserInputs::CheckKeyHold(int key) const {
    if (!CheckKey(key)) {
        return false;
    }
    auto it = keys_.find(key);
    return it->second == HOLD;
}