#pragma once
#include "DxLib.h"

class Keyboard {
	char bkey[256];
	char key[256];

public:
	Keyboard() {
		for (int i = 0; i < 256; i++) key[i] = 0;
	}

	void update() {
		memcpy(bkey, key, 256);
		GetHitKeyStateAll(key);
	}

	bool is_click(int key_input) {
		return !bkey[key_input] && key[key_input];
	}

	bool is_push(int key_input) {
		return key[key_input];
	}
};