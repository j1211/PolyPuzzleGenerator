#pragma once
#include "DxLib.h"

class Mouse {
	int x, y;
	bool bstate, state;

public:
	Mouse() {
		x = 0, y = 0;
		state = false;
	}

	void update() {
		bstate = state;
		state = (GetMouseInput() & MOUSE_INPUT_LEFT) > 0;
		GetMousePoint(&x, &y);
	}

	int X() {
		return x;
	}

	int Y() {
		return y;
	}

	bool is_click() {
		return !bstate && state;
	}

	bool is_release() {
		return bstate && !state;
	}

	bool is_push() {
		return state;
	}
};