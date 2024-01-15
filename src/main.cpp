#define RAYMATH_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

// Include raylib deps
#include "raylib/config.hpp"
#include "raylib/raymath.hpp"
#include "raylib/raylib.hpp"
#include "raylib/rcamera.hpp"
#include "raylib/rlights.hpp"


#include <iostream>
#include <string>
#define print(x) std::cout << x << std::endl // print macro pog

#include "screenhelper.hpp"

namespace Game {
	float dt;
	Vector2 mouseDelta; 										// Mouse movement
	Vector3 movementVector; 									// WASD, etc.
	void GeneralUpdate();										// Should be called once per frame
	void UpdateCamera(Camera3D* cam, bool yawLocked=true);		// Perform necessary updates on camera
}

void Game::GeneralUpdate() {
	Game::dt = GetFrameTime();
	Game::mouseDelta = GetMouseDelta() * 0.05f;
	Game::movementVector = {
		IsKeyDown(KEY_D)-IsKeyDown(KEY_A),					// LEFT/RIGHT
		IsKeyDown(KEY_SPACE)-IsKeyDown(KEY_LEFT_SHIFT),		// UP/DOWN
		IsKeyDown(KEY_W)-IsKeyDown(KEY_S)					// FORWARD/BACKWARD
	};
}

void Game::UpdateCamera(Camera3D* cam, bool yawLocked) {
	
	bool lockView = true;
	float moveSpeed = 5.0f;

	Vector2 rotationVector = -DEG2RAD*Game::mouseDelta; // I don't know why this needs a negative sign
	
	//* Translation
	Vector3 scaledDelta = Game::dt * moveSpeed * (Game::movementVector.x * GetCameraRight(cam) + Game::movementVector.y * GetCameraUp(cam) + Game::movementVector.z * GetCameraForward(cam));
	cam->position += scaledDelta;
	cam->target += scaledDelta;



	//* Rotation
	Vector3 yawAxis = yawLocked ? Vector3(0, 1, 0) : GetCameraUp(cam); // Axis of rotation for yaw

	// Horizontal Movement of Mouse
    Vector3 targetPosition = cam->target - cam->position;

    if (lockView) {
        float maxAngleUp = Vector3Angle(yawAxis, targetPosition);
        maxAngleUp -= 0.001f; // avoid numerical errors
        if (rotationVector.y > maxAngleUp) rotationVector.y = maxAngleUp;

        float maxAngleDown = Vector3Angle(-yawAxis, targetPosition);
        maxAngleDown *= -1.0f; // downwards angle is negative
        maxAngleDown += 0.001f; // avoid numerical errors
        if (rotationVector.y < maxAngleDown) rotationVector.y = maxAngleDown;
    }

    // Rotation axis
    Vector3 right = GetCameraRight(cam);

    // Rotate view vector around right axis
    targetPosition = Vector3RotateByAxisAngle(targetPosition, right, rotationVector.y);

	cam->target = Vector3Add(cam->position, targetPosition);
	cam->up = Vector3RotateByAxisAngle(cam->up, right, rotationVector.y);

	// Vertical Movement of Mouse
	// Both `target` and `up` need to be rotated around the axis so that they stay orthogonal
	cam->target = cam->position + Vector3RotateByAxisAngle(cam->target-cam->position, yawAxis, -mouseDelta.x*DEG2RAD);
	cam->up = Vector3RotateByAxisAngle(cam->up, yawAxis, -mouseDelta.x*DEG2RAD);
}

int main(void) {
	const int screenWidth = 1600;
	const int screenHeight = 900;

	// SetTraceLogLevel(LOG_FATAL);
	InitWindow(screenWidth, screenHeight, "Game");

	if (Init((Vector2){screenWidth, screenHeight}) != 0) { //* Idek why this is structured like this, I just want to be cool
		return 1;
	}

	// Look into loading shaders with each sphere.
	// Look into assembling the mesh manually
	// Look into vertex manipulation

	Model sphere1 = LoadModelFromMesh(GenMeshSphere(1, 16, 32));
	Model sphere2 = LoadModelFromMesh(GenMeshSphere(1, 4, 8));

	Shader shader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/lighting.fs");
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

	// // Ambient light level (some basic lighting)
	// int ambientLoc = GetShaderLocation(shader, "ambient");
	// SetShaderValue(shader, ambientLoc, (float[4]){ 0.0f, 0.0f, 0.0f, 1.0f }, SHADER_UNIFORM_VEC4);

	// Assign out lighting shader to model
	sphere1.materials[0].shader = shader;
	sphere2.materials[0].shader = shader;

	
	// Create light
	Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ -2, 1, -2 }, Vector3Zero(), WHITE, shader);


	Camera3D camera = { 0 };
	camera.position = Vector3(0.0f, 2.0f, -10.0f);
	camera.target = Vector3(0.0f, 0.0f, 0.0f);
	camera.fovy = 72.0f;
	camera.up = Vector3(0.0f, 1.0f, 0.0f);
	camera.projection = CAMERA_PERSPECTIVE;

	DisableCursor();


	SetTargetFPS(300);

	while (!WindowShouldClose()) {
		Game::GeneralUpdate();
		Game::UpdateCamera(&camera);
		
		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], Vector3ToFloat(camera.position), SHADER_UNIFORM_VEC3);
		UpdateLightValues(shader, light);

		if (IsKeyPressed(KEY_P)) EnableCursor();


		BeginDrawing();

			ClearBackground(BLACK);

			BeginMode3D(camera);

				DrawGrid(10, 1);


				DrawModel(sphere1, { -4.0f, 0.0f, 0.0f }, 1.0f, LIME);
				DrawModel(sphere2, {  4.0f, 0.0f, 0.0f }, 1.0f, BLUE);

			EndMode3D();

			// My CS crosshair
			DrawRectangleV(Vector2(screenWidth, screenHeight)/2 - Vector2(2,2), Vector2(4,4), BLACK);
			DrawRectangleV(Vector2(screenWidth, screenHeight)/2 - Vector2(1,1), Vector2(2,2), RED);

			DrawFPS(20, 20);
			
			DrawText(TextFormat("Camera Postion: <%02.02f, %02.02f, %02.02f>", camera.position.x, camera.position.y, camera.position.z), 20, 40, 20, LIME);
			DrawText(TextFormat("Camera Target: <%02.02f, %02.02f, %02.02f>", camera.target.x, camera.target.y, camera.target.z), 20, 60, 20, LIME);
			DrawText(TextFormat("Camera FovY: %02.02f", camera.fovy), 20, 80, 20, LIME);
			DrawText(TextFormat("Camera Up: <%02.02f, %02.02f, %02.02f>", camera.up.x, camera.up.y, camera.up.z), 20, 100, 20, LIME);

		EndDrawing();
	}

	FreeFontStuff();
	UnloadModel(sphere1);
	UnloadModel(sphere2);
	UnloadShader(shader);
	CloseWindow();

	return 0;
}
