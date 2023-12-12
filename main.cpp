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
	
	if (init() != 0) { // Idek why this is structured like this, I just want to be cool
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

			if (Button((Rectangle){ 400.f, 420.f, 210.f, 50.f }, (Color) { 51.f, 51.f, 51.f, 255.f }, "I'm spaced out!", 32, spacing)) {
				print("Space is so cool!");
			}
			if (ButtonHold((Rectangle){ 400.f, 480.f, 210.f, 50.f }, (Color) { 51.f, 51.f, 51.f, 255.f }, "spacing += 0.1", 32, 1.f)) {
				spacing += 4.f*deltaTime;
			}
			if (ButtonHold((Rectangle){ 400.f, 540.f, 210.f, 50.f }, (Color) { 51.f, 51.f, 51.f, 255.f }, "spacing -= 0.1", 32, 1.f)) {
				spacing -= 4.f*deltaTime;
			}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
