#pragma once
#include <unordered_set>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "KEY_CODE.h"


struct MouseInputs {
	enum ButtonState {
		RELEASE, PRESS, HOLD
	};

	enum ScrollState {
		SCROLL_NONE, SCROLL_DOWN, SCROLL_UP
	};

	ButtonState LEFT;
	ButtonState MIDDLE;
	ButtonState RIGHT;

	ScrollState ScrollDirection = SCROLL_NONE; //-1 or 1

	glm::dvec2 Displacement = glm::dvec2(0.f, 0.f);
	glm::dvec2 Position = glm::dvec2(0.f, 0.f);
};


class UserInputs {

private:

	enum KeyStatus {
		PRESS, HOLD
	};

	std::unordered_map<int, KeyStatus> Keys;


public:

	void UpdateAllKey() {
		for (const auto& key : Keys) {
			Keys[key.first] = HOLD;
		}
	}

	void PressIndividualKey(int key) {
		if (Keys.count(key)) {
			Keys[key] = HOLD;
			return;
		}

		Keys[key] = PRESS;
		return;
	}

	void ReleaseIndividualKey(int key) {
		if (Keys.count(key)) {
			Keys.erase(key);
		}
	}
	
	bool CheckKey(int key) {
		if (Keys.count(key)) {
			return true;
		}
		return false;
	}

	bool CheckKeyPress(int key) {
		if (!CheckKey(key))
			return false;

		return Keys[key] == PRESS;
	}

	bool CheckKeyHold(int key) {
		if (!CheckKey(key))
			return false;

		return Keys[key] == HOLD;
	}

	MouseInputs Mouse;
	
	float delta = 0.f;
};