#include "IO.h"

void UserInputs::UpdateAllKey() {
	for (const auto& key : Keys) {
		Keys[key.first] = HOLD;
	}

	Mouse.ScrollDirection = Mouse.SCROLL_NONE;

	Mouse.Displacement = glm::dvec2(0.0, 0.0);

	if (Mouse.LEFT == Mouse.PRESS) {
		Mouse.LEFT = Mouse.HOLD;
	}

	if (Mouse.RIGHT == Mouse.PRESS) {
		Mouse.RIGHT = Mouse.HOLD;
	}

	if (Mouse.MIDDLE == Mouse.PRESS) {
		Mouse.MIDDLE = Mouse.HOLD;
	}
}

void UserInputs::PressIndividualKey(int key) {
	if (Keys.count(key)) {
		Keys[key] = HOLD;
	} else {
		Keys[key] = PRESS;
	}
}

void UserInputs::ReleaseIndividualKey(int key) {
	if (Keys.count(key)) {
		Keys.erase(key);
	}
}

bool UserInputs::CheckKey(int key) {
	if (Keys.count(key)) {
		return true;
	}
	return false;
}

bool UserInputs::CheckKeyPress(int key) {
	if (!CheckKey(key)) {
		return false;
	}
	return Keys[key] == PRESS;
}

bool UserInputs::CheckKeyHold(int key) {
	if (!CheckKey(key)) {
		return false;
	}
	return Keys[key] == HOLD;
}