// Include raylib deps
#include <string>
#include <raylib/raylib.h>
#include <raylib/rcamera.h>

// #include "screenhelper.hpp"

#define GLSL_VERSION	330

Font sdfFont = { 0 };
Shader sdfFontShader;

int init(void) {
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

bool Button(Rectangle rect, Color buttonColor, std::string text, int fontSize=32, float spacing=0.f) {
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

bool ButtonHold(Rectangle rect, Color buttonColor, std::string text, int fontSize=32, float spacing=0.f) {
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