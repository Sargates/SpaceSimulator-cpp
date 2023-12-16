// Include raylib deps
#include <raylib/raylib.h>
#include <raylib/rcamera.h>


#include <iostream>
#define print(x) std::cout << x << std::endl // print macro pog

// Personalized helper files
#include "screenhelper.hpp"




int main(void) {
	const int screenWidth = 1600;
	const int screenHeight = 900;

	SetTraceLogLevel(LOG_FATAL);
	InitWindow(screenWidth, screenHeight, "Game");

	if (Init((Vector2){screenWidth, screenHeight}) != 0) { //* Idek why this is structured like this, I just want to be cool
		return 1;
	}


	Camera2D camera = { 0 };
	camera.offset = (Vector2){ screenWidth/2.f, screenHeight/2.f }; // Translate the camera to be looking at the origin initially, this way `camera.target` will always be at the center of the screen.
	camera.target = (Vector2){ 0.0f, 0.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.f;


	SetTargetFPS(300);                   // Set our game to run at 60 frames-per-second

	float deltaTime = 0.0f;
	float spacing = 2.f;
	while (!WindowShouldClose()) {
		deltaTime = GetFrameTime();


		BeginDrawing();

			ClearBackground(GRAY);

			BeginMode2D(camera);

				// Draw things here

			EndMode2D();
			DrawFPS(20, 20);

			if (Button(Alignment::TOPLEFT, 		(Vector2){ 210.f, 50.f }, ElementAlignment::TOPLEFT, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the TOPLEFT", 18, spacing)) {
				print("My alignment is TOPLEFT");
			}
			if (Button(Alignment::TOPCENTER, 	(Vector2){ 210.f, 50.f }, ElementAlignment::TOPCENTER, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the TOPCENTER", 18, spacing)) {
				print("My alignment is TOPCENTER");
			}
			if (Button(Alignment::TOPRIGHT, 	(Vector2){ 210.f, 50.f }, ElementAlignment::TOPRIGHT, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the TOPRIGHT", 18, spacing)) {
				print("My alignment is TOPRIGHT");
			}
			if (Button(Alignment::LEFT, 		(Vector2){ 210.f, 50.f }, ElementAlignment::LEFT, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the LEFT", 18, spacing)) {
				print("My alignment is LEFT");
			}
			if (Button(Alignment::CENTER, 		(Vector2){ 210.f, 50.f }, ElementAlignment::CENTER, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the CENTER", 18, spacing)) {
				print("My alignment is CENTER");
			}
			if (Button(Alignment::RIGHT, 		(Vector2){ 210.f, 50.f }, ElementAlignment::RIGHT, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the RIGHT", 18, spacing)) {
				print("My alignment is RIGHT");
			}
			if (Button(Alignment::BOTTOMLEFT, 	(Vector2){ 210.f, 50.f }, ElementAlignment::BOTTOMLEFT, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the BOTTOMLEFT", 18, spacing)) {
				print("My alignment is BOTTOMLEFT");
			}
			if (Button(Alignment::BOTTOMCENTER, (Vector2){ 210.f, 50.f }, ElementAlignment::BOTTOMCENTER, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the BOTTOMCENTER", 18, spacing)) {
				print("My alignment is BOTTOMCENTER");
			}
			if (Button(Alignment::BOTTOMRIGHT, 	(Vector2){ 210.f, 50.f }, ElementAlignment::BOTTOMRIGHT, (Color) { 51.f, 51.f, 51.f, 255.f }, "I should be in the BOTTOMRIGHT", 18, spacing)) {
				print("My alignment is BOTTOMRIGHT");
			}

		EndDrawing();
	}

	FreeFontStuff();
	CloseWindow();

	return 0;
}
