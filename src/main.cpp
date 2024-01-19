#define RAYMATH_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

// Include raylib deps
#include "raymath.hpp"
#include "raylib.hpp"
#include "rlgl.hpp"
#include "rcamera.hpp"
#include "rlights.hpp"


#include <iostream>
#include <string>
#include <vector>

#define print(x) std::cout << x << std::endl // print macro pog

#include "globals.hpp"
#include "screenhelper.hpp"
#include "planet.hpp"
#include "object_transform.hpp"

#define PLAYER_DISTANCE_RESET			1000
#define PLAYER_DISTANCE_RESET_SQUARED	PLAYER_DISTANCE_RESET*PLAYER_DISTANCE_RESET




int main(void) {
	const Vector2 screenSize(1600, 900);
	const int screenWidth = screenSize.x;
	const int screenHeight = screenSize.y;

	Game::player = Player(Vector3(0.0f, 0.0f, 10000.0f), Vector4::Zero, 72.0f, true); // Orient to face the origin
	Game::player.transform.AddRotation(QuaternionFromEuler(0, 180*DEG2RAD, 0));

	// SetTraceLogLevel(LOG_FATAL);
	InitWindow(screenWidth, screenHeight, "Game");

	if (Init((Vector2){screenWidth, screenHeight}) != 0) { //* Idek why this is structured like this, I just want to be cool
		return 1;
	}

	// [x] ~~Look into loading shaders with each sphere~~
	// [x] ~~Look into adding stary sky using shader or texture -- Research Depth buffer~~
	// [ ] Look into assembling the mesh manually
	// [ ] Look into vertex manipulation


	Shader lightingShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/lighting.fs");
	lightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightingShader, "viewPos");
    lightingShader.locs[SHADER_LOC_COLOR_AMBIENT] = GetShaderLocation(lightingShader, "ambient"); // Ambient light level (some basic lighting)

	// Create light
	Light light = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, Vector3Zero(), WHITE, lightingShader);

	// Sun
	Game::planets.push_back(
		Planet(Vector3(0, 0, 0), 500.0f, 100, YELLOW, nullptr, &light) // Sun gives off light
	);
	// Earth
	Game::planets.push_back(
		Planet(Vector3(100, 0, 9500), 50.0f, 9.81, SKYBLUE, &lightingShader)
	);
	// Mars
	Game::planets.push_back(
		Planet(Vector3(-9899.49, 0, 9899.49), 30.0f, 3.71, Color{156, 46, 53}, &lightingShader)
	);
	// // Temp
	// Game::planets.push_back(
	// 	Planet(Vector3(0, 0, -100000), 20000.0f, 10, ORANGE, &lightingShader)
	// );

	Shader starsShader = LoadShader(NULL, "resources/shaders/stars.fs");
	starsShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(starsShader, "viewPos");
	starsShader.locs[SHADER_LOC_FLOAT_FOVY] = GetShaderLocation(starsShader, "fovY");
	starsShader.locs[SHADER_LOC_VECTOR2_SCREENSIZE] = GetShaderLocation(starsShader, "screenSize");
	starsShader.locs[SHADER_LOC_MATRIX_VIEW] = GetShaderLocation(starsShader, "matView");

	

	Camera3D overlayCamera = Camera3D{
		.position = Vector3(0.0f, 0.0f, -5.0f),
		.target = Vector3(0.0f, 0.0f, 0.0f),
		.up = Vector3(0.0f, 1.0f, 0.0f),
		.fovy = 40.0f,
		.projection = CAMERA_ORTHOGRAPHIC
	};

	DisableCursor();
	SetMousePosition(screenWidth/2.0f, screenHeight/2.0f);

	SetTargetFPS(300);

	// Buffer texture used for executing shaders on to draw to another texture
	RenderTexture bufferTexture = LoadRenderTexture(screenWidth, screenHeight);
	RenderTexture starsTexture = LoadRenderTexture(screenWidth, screenHeight);

	bool freeRotate = false;
	float ambientStrength = 0.0f;
	


	while (!WindowShouldClose()) {
		Game::GeneralUpdate();
		Game::UpdatePlayer(!freeRotate);

		//! If issue occurs: Change `Game::player.camera.position` to `Game::player.transform.GetPosition()`
		// Update camera uniform in the shader
		SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_VECTOR_VIEW], Vector3ToFloat(Game::player.camera.position), SHADER_UNIFORM_VEC3);
		UpdateLightValues(lightingShader, light);
		// Update ambientColor uniform in the shader
    	SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_COLOR_AMBIENT], Vector4ToFloat(Vector4(ambientStrength,ambientStrength,ambientStrength,1.0f)), SHADER_UNIFORM_VEC4);

		// Update screensize uniform in the buffer
    	SetShaderValue(starsShader, starsShader.locs[SHADER_LOC_VECTOR2_SCREENSIZE], Vector2ToFloat(screenSize), SHADER_UNIFORM_VEC2);
		// Update fov uniform in the buffer
    	SetShaderValue(starsShader, starsShader.locs[SHADER_LOC_FLOAT_FOVY], &Game::player.camera.fovy, SHADER_UNIFORM_FLOAT);
		// Update camera uniform in the shader
		SetShaderValue(starsShader, starsShader.locs[SHADER_LOC_VECTOR_VIEW], Vector3ToFloat(Game::player.camera.position), SHADER_UNIFORM_VEC3);
		SetShaderValueMatrix(starsShader, starsShader.locs[SHADER_LOC_MATRIX_VIEW], MatrixInvert(QuaternionToMatrix(Game::player.transform.GetRotation())));


		if (IsKeyPressed(KEY_GRAVE)) { IsCursorHidden() ? EnableCursor() : DisableCursor(); };
		if (IsKeyPressed(KEY_F3)) Game::debugMode = !Game::debugMode;
		if (IsKeyPressed(KEY_P)) {
			if (freeRotate) { // Free rotate is being disabled, reset the transforms up direction
				Game::player.transform.AddRotation(QuaternionFromVector3ToVector3(Game::player.transform.GetUp(), Vector3{0,1,0}));
				Game::player.UpdateCamera();
			}
			freeRotate = !freeRotate;
		}
		if (IsKeyPressed(KEY_O)) {
			Matrix mat = MatrixInvert(QuaternionToMatrix(Game::player.transform.GetRotation()));
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m0, mat.m4, mat.m8, mat.m12) << std::endl;
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m1, mat.m5, mat.m9, mat.m13) << std::endl;
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m2, mat.m6, mat.m10, mat.m14) << std::endl;
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m3, mat.m7, mat.m11, mat.m15) << std::endl;
		}
		if (Game::mouseWheelDelta.y != 0) {
			ambientStrength += Game::mouseWheelDelta.y * 0.04f;
			ambientStrength = std::clamp(ambientStrength, 0.0f, 1.0f);
		}

		// Updating velocity, then position seems to prevent strange ordering issues
		for (Planet& planet : Game::planets)
			planet.UpdateVelocity(Game::dt);
		for (Planet& planet : Game::planets)
			planet.UpdatePosition(Game::dt);



		BeginTextureMode(starsTexture);

			ClearBackground(BLANK);

			BeginShaderMode(starsShader);
				DrawTexture(bufferTexture.texture, 0, 0, WHITE);
			EndShaderMode();


		EndTextureMode();


		BeginDrawing();
			ClearBackground(BLACK);

			DrawTextureRec(starsTexture.texture, (Rectangle){ 0, 0, screenWidth, -screenHeight }, Vector2::Zero, WHITE);

			BeginMode3D(Game::player.camera);

				for (Planet& planet : Game::planets)
					planet.Draw();

			EndMode3D();

			if (Game::debugMode) {
				BeginMode3D(overlayCamera);

					DrawLine3D(Vector3::Zero, Vector3Transform(Vector3{ 1, 0, 0 }, QuaternionToMatrix(Game::player.transform.GetRotation())), RED);
					DrawLine3D(Vector3::Zero, Vector3Transform(Vector3{ 0, 1, 0 }, QuaternionToMatrix(Game::player.transform.GetRotation())), LIME);
					DrawLine3D(Vector3::Zero, Vector3Transform(Vector3{ 0, 0, 1 }, QuaternionToMatrix(Game::player.transform.GetRotation())), BLUE);

				EndMode3D();
			}


			// My CS crosshair
			DrawRectangleV(Vector2(screenWidth, screenHeight)/2 - Vector2(2,2), Vector2(4,4), BLACK);
			DrawRectangleV(Vector2(screenWidth, screenHeight)/2 - Vector2(1,1), Vector2(2,2), RED);

			DrawFPS(20, 20);
			
			DrawText(TextFormat("Camera Postion: <%02.02f, %02.02f, %02.02f>", Game::player.camera.position.x, Game::player.camera.position.y, Game::player.camera.position.z), 20, 40, 20, LIME);
			DrawText(TextFormat("Camera Target: <%02.02f, %02.02f, %02.02f>", Game::player.camera.target.x-Game::player.camera.position.x, Game::player.camera.target.y-Game::player.camera.position.y, Game::player.camera.target.z-Game::player.camera.position.z), 20, 60, 20, LIME);
			DrawText(TextFormat("Camera FovY: %02.02f", Game::player.camera.fovy), 20, 80, 20, LIME);
			DrawText(TextFormat("Camera Up: <%02.02f, %02.02f, %02.02f>", Game::player.camera.up.x, Game::player.camera.up.y, Game::player.camera.up.z), 20, 100, 20, LIME);
			DrawText((freeRotate ? "Freelook: Enabled" : "Freelook: Disabled"), screenWidth-450, 20, 20, RED);

		EndDrawing();
	}

	FreeFontStuff();
	UnloadShader(lightingShader);
	UnloadShader(starsShader);
	UnloadRenderTexture(starsTexture);
	UnloadRenderTexture(bufferTexture);
	for (Planet& planet : Game::planets)
		planet.Free();

	CloseWindow();

	return 0;
}

