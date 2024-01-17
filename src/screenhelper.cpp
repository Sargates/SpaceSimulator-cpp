// Include raylib deps
#include <string>
#include "raylib.hpp"
#include "rcamera.hpp"

#include "screenhelper.hpp"


#define GLSL_VERSION	330

Font sdfFont = { 0 };
Shader sdfFontShader;
Vector2 screenSize;

int Init(Vector2 size) {
	screenSize = size;

	//* Initialize font
	int fileSize = 0;
	unsigned char *fileData = LoadFileData("resources/Fonts/opensans-semibold.ttf", &fileSize);
	// SDF font generation from TTF font
	sdfFont.baseSize = 64;
	sdfFont.glyphCount = 95;
	// Parameters > font size: 64, no glyphs array provided (0), glyphs count: 0 (defaults to 95)
	sdfFont.glyphs = LoadFontData(fileData, fileSize, 64, 0, 0, FONT_SDF);
	// Parameters > glyphs count: 95, font size: 64, glyphs padding in image: 0 px, pack method: 1 (Skyline algorythm)
	Image atlas = GenImageFontAtlas(sdfFont.glyphs, &sdfFont.recs, 95, 64, 0, 1);
	sdfFont.texture = LoadTextureFromImage(atlas);
	// Get rid of unneeded data
	while (! IsImageReady(atlas)) {}
	UnloadImage(atlas);
	UnloadFileData(fileData);

	sdfFontShader = LoadShader(0, TextFormat("resources/Fonts/sdf.fs", GLSL_VERSION));
	SetTextureFilter(sdfFont.texture, TEXTURE_FILTER_BILINEAR);

	return 0;
}

Vector2 AlignmentToScreenPosition(Alignment alignment) {
	switch(alignment) {
		case Alignment::TOPLEFT: 		return (Vector2) {            0.f,            0.f }; break;
		case Alignment::TOPCENTER: 		return (Vector2) { screenSize.x/2,            0.f }; break;
		case Alignment::TOPRIGHT: 		return (Vector2) {   screenSize.x,            0.f }; break; 
		case Alignment::LEFT: 			return (Vector2) {            0.f, screenSize.y/2 }; break; 
		case Alignment::CENTER: 		return (Vector2) { screenSize.x/2, screenSize.y/2 }; break; 
		case Alignment::RIGHT: 			return (Vector2) {   screenSize.x, screenSize.y/2 }; break; 
		case Alignment::BOTTOMLEFT: 	return (Vector2) {            0.f,   screenSize.y }; break; 
		case Alignment::BOTTOMCENTER: 	return (Vector2) { screenSize.x/2,   screenSize.y }; break; 
		case Alignment::BOTTOMRIGHT: 	return (Vector2) {   screenSize.x,   screenSize.y }; break; 

		default: 						return (Vector2) {            0.f,            0.f }; break; // Todo: Raise exception here
	}
}

Vector2 ElementAlignmentToRelativePosition(ElementAlignment alignment, Vector2 size, bool negate) {
	Vector2 out;
	switch(alignment) {
		case ElementAlignment::TOPLEFT: 		out = (Vector2) {      0.f,      0.f }; break;
		case ElementAlignment::TOPCENTER: 		out = (Vector2) { size.x/2,      0.f }; break;
		case ElementAlignment::TOPRIGHT: 		out = (Vector2) {   size.x,      0.f }; break; 
		case ElementAlignment::LEFT: 			out = (Vector2) {      0.f, size.y/2 }; break; 
		case ElementAlignment::CENTER: 			out = (Vector2) { size.x/2, size.y/2 }; break; 
		case ElementAlignment::RIGHT: 			out = (Vector2) {   size.x, size.y/2 }; break; 
		case ElementAlignment::BOTTOMLEFT: 		out = (Vector2) {      0.f,   size.y }; break; 
		case ElementAlignment::BOTTOMCENTER: 	out = (Vector2) { size.x/2,   size.y }; break; 
		case ElementAlignment::BOTTOMRIGHT: 	out = (Vector2) {   size.x,   size.y }; break; 

		default: 								out = (Vector2) {      0.f,      0.f }; break; // Todo: Raise exception here
	}
	if (!negate) {
		return out;
	}
	return (Vector2) { -out.x, -out.y };
}

Rectangle AlignmentToRect(Alignment alignment, Vector2 size, Vector2 delta) {
	Vector2 pos = AlignmentToScreenPosition(alignment);
	return (Rectangle) { pos.x + delta.x, pos.y + delta.y, size.x, size.y };
}

bool Button(Rectangle rect, Color buttonColor, std::string text, int fontSize, float spacing) {
	Vector2 mousePos = GetMousePosition();
	Vector2 fontPosition = (Vector2){ rect.x + rect.width/2, rect.y + rect.height/2 };
	Vector2 fontSizeV = MeasureTextEx(sdfFont, text.c_str(), fontSize, spacing);
	fontPosition = (Vector2){ fontPosition.x - fontSizeV.x/2, fontPosition.y - fontSizeV.y/2 };

	if (! ((rect.x <= mousePos.x && mousePos.x <= rect.x + rect.width) && (rect.y <= mousePos.y && mousePos.y <= rect.y + rect.height))) { 
		DrawRectangleRec(rect, buttonColor);
		BeginShaderMode(sdfFontShader);
		DrawTextEx(sdfFont, text.c_str(), fontPosition, fontSize, spacing, WHITE);
		EndShaderMode();
		return false;
	}
	DrawRectangleRec(rect, (Color){ 3.f, 173.f, 252.f, 255.f });
	BeginShaderMode(sdfFontShader);
	DrawTextEx(sdfFont, text.c_str(), fontPosition, fontSize, spacing, WHITE);
	EndShaderMode();

	return IsMouseButtonPressed(MOUSE_BUTTON_LEFT);	
}

// // ~~This will ensure that the button is situated~~
bool Button(Alignment alignment, Vector2 size, ElementAlignment elemAlignment, Color buttonColor, std::string text, int fontSize, float spacing) {
	return Button(
		AlignmentToRect( // Convert passed arguments to a rectangle at the relevent position
			alignment, size, 
			ElementAlignmentToRelativePosition( // Get local position of element (determined by size) based on passed `ElementAlignment` and negate the value
				elemAlignment, size, true
		)), 
		buttonColor, text, fontSize, spacing
	);
}

bool ButtonHold(Rectangle rect, Color buttonColor, std::string text, int fontSize, float spacing) {
	Vector2 mousePos = GetMousePosition();
	Vector2 fontPosition = (Vector2){ rect.x + rect.width/2, rect.y + rect.height/2 };
	Vector2 fontSizeV = MeasureTextEx(sdfFont, text.c_str(), fontSize, spacing);
	fontPosition = (Vector2){ fontPosition.x - fontSizeV.x/2, fontPosition.y - fontSizeV.y/2 };

	if (! ((rect.x <= mousePos.x && mousePos.x <= rect.x + rect.width) && (rect.y <= mousePos.y && mousePos.y <= rect.y + rect.height))) { 
		DrawRectangleRec(rect, buttonColor);
		BeginShaderMode(sdfFontShader);
		DrawTextEx(sdfFont, text.c_str(), fontPosition, fontSize, spacing, WHITE);
		EndShaderMode();
		return false;
	}
	DrawRectangleRec(rect, (Color){ 3.f, 173.f, 252.f, 255.f });
	BeginShaderMode(sdfFontShader);
	DrawTextEx(sdfFont, text.c_str(), fontPosition, fontSize, spacing, WHITE);
	EndShaderMode();

	return IsMouseButtonDown(MOUSE_BUTTON_LEFT);	
}

int FreeFontStuff(void) {
	// Free memory to avoid memory leak
	UnloadFont(sdfFont);
	UnloadShader(sdfFontShader);
	return 0;
}