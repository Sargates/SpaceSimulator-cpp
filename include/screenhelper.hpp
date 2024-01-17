#pragma once

// Include raylib deps
#include "raylib.hpp"
#include "rcamera.hpp"
#include <iostream>


extern Font sdfFont;
extern Shader sdfFontShader;
extern Vector2 screenSize;

//* These need to have the `class` keyword. See here: https://stackoverflow.com/a/23288968
enum class Alignment {
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

//* These need to have the `class` keyword. See here: https://stackoverflow.com/a/23288968
enum class ElementAlignment {
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

int Init(Vector2 size);
void DrawTextOnScreen(std::string text, Vector2 position, float fontSize, float spacing = 2.f, Color tint = WHITE, bool centered=true);
Vector2 AlignmentToScreenPosition(Alignment alignment);
Vector2 ElementAlignmentToRelativePosition(ElementAlignment alignment, Vector2 size, bool negate=false);
Rectangle AlignmentToRect(Alignment alignment, Vector2 size, Vector2 delta = (Vector2) { 0.f, 0.f });
bool Button(Rectangle rect, Color buttonColor, std::string text, int fontSize=24, float spacing=2.f);
bool Button(Alignment alignment, Vector2 size, ElementAlignment elemAlignment, Color buttonColor, std::string text, int fontSize=24, float spacing=2.f);
bool ButtonHold(Rectangle rect, Color buttonColor, std::string text, int fontSize=24, float spacing=2.f);
int FreeFontStuff(void);