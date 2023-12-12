// Include raylib deps
#include <raylib/raylib.h>
#include <raylib/rcamera.h>
#include <iostream>

enum Alignment {
	TOPLEFT,
	TOPCENTER,
	TOPRIGHT,
	LEFT,
	CENTER,
	RIGHT,
	BOTTOMLEFT,
	BOTTOMCENTER,
	BOTTOMRIGHT
};

int init(); // Initializes things like fonts and runtime constants
bool Button(Rectangle rect, Color buttonColor, std::string text, int fontSize=32, float spacing=2.f);
bool ButtonHold(Rectangle rect, Color buttonColor, std::string text, int fontSize=32, float spacing=2.f);
