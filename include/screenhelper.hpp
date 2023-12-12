// Include raylib deps
#include <raylib/raylib.h>
#include <raylib/rcamera.h>
#include <iostream>

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
Vector2 AlignmentToScreenPosition(Alignment alignment);
Vector2 ElementAlignmentToRelativePosition(ElementAlignment alignment, Vector2 size, bool negate=false);
Rectangle AlignmentToRect(Alignment alignment, Vector2 size, Vector2 delta = (Vector2) { 0.f, 0.f });
bool Button(Rectangle rect, Color buttonColor, std::string text, int fontSize=24, float spacing=2.f);
bool Button(Alignment alignment, Vector2 size, ElementAlignment elemAlignment, Color buttonColor, std::string text, int fontSize=24, float spacing=2.f);
bool ButtonHold(Rectangle rect, Color buttonColor, std::string text, int fontSize=24, float spacing=2.f);
int FreeFontStuff(void);