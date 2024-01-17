#define RAYMATH_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

// Include raylib deps
#include "config.hpp"
#include "raymath.hpp"
#include "raylib.hpp"
#include "rcamera.hpp"
#include "rlights.hpp"
#include "object_transform.hpp"


#include <iostream>
#include <string>
#include <vector>
#define print(x) std::cout << x << std::endl // print macro pog

#include "screenhelper.hpp"

typedef struct Player {
	Player() : transform(Vector3::Zero, Vector4::Zero, false) {
		camera.projection = CAMERA_PERSPECTIVE;
		this->UpdateCamera();
		camera.up = Vector3{0, 1, 0};
		camera.fovy = 60;
	}

	Player(Vector3 position, Quaternion rotation, float fovY, bool faceY) : transform(position, rotation, faceY) {
		camera.projection = CAMERA_PERSPECTIVE;
		this->UpdateCamera();
		camera.up = Vector3{0, 1, 0};
		camera.fovy = fovY;
	}

	ObjectTransform transform;
	Camera3D camera = {0};
	void UpdateCamera(void);
} Player;

namespace Game {
	float dt;
	Vector2 mouseDelta; 							// Mouse movement
	Vector3 movementVector; 						// WASD, etc.
	bool debugMode = false;							// Debug Mode
	void GeneralUpdate(void);						// Should be called once per frame
	void UpdatePlayer(bool yawLocked=true);			// Perform necessary updates on camera
	std::vector<ObjectTransform*> physicsObjects; 	// This vector should only contain `ObjectTransform`'s that do not have a parent
	Player player;
}

// This needs to be defined after `Game`
void Player::UpdateCamera(void) {
	camera.position = transform.GetPosition();
	// Set target to be inline with camera's position
	camera.target = Vector3Transform({ 0, 0, 1 }, transform.GetWorldMatrix());
	camera.up = transform.GetUp();
}

void Game::GeneralUpdate() {
	Game::dt = GetFrameTime();
	Game::mouseDelta = GetMouseDelta() * 0.05f;
	Game::movementVector = {
		IsKeyDown(KEY_D)	 - IsKeyDown(KEY_A),			// LEFT/RIGHT		/ X
		IsKeyDown(KEY_SPACE) - IsKeyDown(KEY_LEFT_SHIFT),	// UP/DOWN			/ Y
		IsKeyDown(KEY_W)	 - IsKeyDown(KEY_S)				// FORWARD/BACKWARD	/ Z
	};
}

void Game::UpdatePlayer(bool yawLocked) {
	
	float moveSpeed = 300.0f;
	float mouseSens = 1.0f;

	Vector2 rotationVector = DEG2RAD*Game::mouseDelta*mouseSens;
	
	//* Translation
	Vector3 translationDelta = Game::dt * moveSpeed * (Game::movementVector.x * Game::player.transform.GetRight() + Game::movementVector.y * Game::player.transform.GetUp() + Game::movementVector.z * Game::player.transform.GetForward());

	//* Rotational Axis
	Vector3 yawAxis = yawLocked ? Vector3{0,1,0} : Game::player.transform.GetUp(); // Axis of rotation for yaw

	//* Horizontal Movement of Mouse
    // Vector3 targetPosition = Game::player.transform.GetForward();
	Quaternion newRotation = QuaternionMultiply(
		QuaternionFromEulerV(Game::player.transform.GetRight() * rotationVector.y),	// Pitch
		QuaternionFromEulerV(yawAxis * rotationVector.x)							// Yaw
	); // The order we multiply seems to matter, otherwise we need to reset `camera.up` periodically

	Game::player.transform.AddPosition(translationDelta);	// Add the translation
	Game::player.transform.AddRotation(newRotation);		// Add the new rotation

	Game::player.UpdateCamera(); // Update player camera with the new transform
}

int main(void) {
	const int screenWidth = 1600;
	const int screenHeight = 900;

	Game::player = Player(Vector3(0.0f, 0.0f, -10.0f), Vector4(), 72.0f, true);

	// SetTraceLogLevel(LOG_FATAL);
	InitWindow(screenWidth, screenHeight, "Game");

	if (Init((Vector2){screenWidth, screenHeight}) != 0) { //* Idek why this is structured like this, I just want to be cool
		return 1;
	}

	// [x] Look into loading shaders with each sphere. 
	// [ ] Look into assembling the mesh manually
	// [ ] Look into vertex manipulation
	// [ ] Look into adding stary sky using shader or texture -- Research Depth buffer


	Model sphere1 = LoadModelFromMesh(GenMeshSphere(100, 16, 32));
	Model sphere2 = LoadModelFromMesh(GenMeshSphere(100, 4, 8));

	Shader shader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/lighting.fs");
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

	// Assign lighting shader to model
	sphere1.materials[0].shader = shader;
	sphere2.materials[0].shader = shader;

	
	// Create light
	Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ -2, 1, -2 }, Vector3Zero(), WHITE, shader);


	Camera3D overlayCamera = { 0 };
	overlayCamera.position = Vector3(0.0f, 0.0f, -5.0f);
	overlayCamera.target = Vector3(0.0f, 0.0f, 0.0f);
	overlayCamera.up = Vector3(0.0f, 1.0f, 0.0f);
	overlayCamera.fovy = 40.0f;
	overlayCamera.projection = CAMERA_ORTHOGRAPHIC;

	DisableCursor();
	SetMousePosition(screenWidth/2.0f, screenHeight/2.0f);

	SetTargetFPS(300);

	bool freeRotate = false;

	while (!WindowShouldClose()) {
		Game::GeneralUpdate();
		Game::UpdatePlayer(!freeRotate);

		//! If issue occurs: Change `Game::player.camera.position` to `Game::player.transform.GetPosition()`
		SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], Vector3ToFloat(Game::player.camera.position), SHADER_UNIFORM_VEC3);
		UpdateLightValues(shader, light);

		if (IsKeyPressed(KEY_GRAVE)) EnableCursor();
		if (IsKeyPressed(KEY_F3)) Game::debugMode = !Game::debugMode;
		if (IsKeyPressed(KEY_P)) {
			if (freeRotate) { // Free rotate is being disabled, reset the transforms up direction
				Game::player.transform.AddRotation(
					QuaternionFromVector3ToVector3(Game::player.transform.GetUp(), Vector3{0,1,0})
				);
				Game::player.UpdateCamera();

				std::cout << "Freecam Disabled" << std::endl;
			} else {
				std::cout << "Freecam Enabled" << std::endl;
			}
			freeRotate = !freeRotate;
		}





		BeginDrawing();

			ClearBackground(BLACK);

			BeginMode3D(Game::player.camera);

				DrawModel(sphere1, Vector3(-150.0f, 0.0f, 200.0f), 1.0f, LIME);
				DrawModel(sphere2, Vector3( 150.0f, 0.0f, 200.0f), 1.0f, BLUE);

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
			DrawText(TextFormat("Camera Target: <%02.02f, %02.02f, %02.02f>", Game::player.camera.target.x, Game::player.camera.target.y, Game::player.camera.target.z), 20, 60, 20, LIME);
			DrawText(TextFormat("Camera FovY: %02.02f", Game::player.camera.fovy), 20, 80, 20, LIME);
			DrawText(TextFormat("Camera Up: <%02.02f, %02.02f, %02.02f>", Game::player.camera.up.x, Game::player.camera.up.y, Game::player.camera.up.z), 20, 100, 20, LIME);
			DrawText((freeRotate ? "Freelook: Enabled" : "Freelook: Disabled"), screenWidth-450, 20, 20, RED);

		EndDrawing();
	}

	FreeFontStuff();
	UnloadModel(sphere1);
	UnloadModel(sphere2);
	UnloadShader(shader);
	CloseWindow();

	return 0;
}

