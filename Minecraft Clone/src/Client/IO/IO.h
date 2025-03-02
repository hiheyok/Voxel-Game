#pragma once
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "KEY_CODE.h"
#include "../../Level/Typenames.h"

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
	glm::dvec2 position_ = glm::dvec2(0.f, 0.f);
};


class UserInputs {

private:

	enum KeyStatus {
		PRESS, HOLD
	};

	FastHashMap<int, KeyStatus> Keys;

public:

	void UpdateAllKey();

	void PressIndividualKey(int key);

	void ReleaseIndividualKey(int key);
	
	bool CheckKey(int key);

	bool CheckKeyPress(int key);

	bool CheckKeyHold(int key);

	MouseInputs Mouse;
	
	float delta = 0.f;
};