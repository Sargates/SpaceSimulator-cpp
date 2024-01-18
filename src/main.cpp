#define RAYMATH_IMPLEMENTATION
#define RLIGHTS_IMPLEMENTATION

// Include raylib deps
#include "config.hpp"
#include "raymath.hpp"
#include "raylib.hpp"
#include "rlgl.hpp"
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

// void Game::UpdatePlayer(bool yawLocked) {
	
	//! Broken pitch/yaw migration
	//! Notes: Yaw/pitch doesn't work with freecam mode (think about it) -- I cant get pitch clamping to work
	
// 	// TODO: convert this to yaw/pitch based w/ clamping
	
// 	float moveSpeed = 300.0f;
// 	float mouseSens = 1.0f;

// 	Vector2 rotationVector = DEG2RAD*Game::mouseDelta*mouseSens;

// 	Game::player.transform.SetYaw(Game::player.transform.GetYaw()+rotationVector.x);
// 	Game::player.transform.SetPitch(std::clamp(Game::player.transform.GetPitch()+rotationVector.y, -89.0f+EPSILON, 89.0f-EPSILON));
	
// 	//* Translation
// 	Vector3 translationDelta = Game::dt * moveSpeed * (Game::movementVector.x * Game::player.transform.GetRight() + Game::movementVector.y * Game::player.transform.GetUp() + Game::movementVector.z * Game::player.transform.GetForward());

// 	//* Rotational Axis
// 	Vector3 yawAxis = yawLocked ? Vector3{0,1,0} : Game::player.transform.GetUp(); // Axis of rotation for yaw

// 	//* Horizontal Movement of Mouse
//     // Vector3 targetPosition = Game::player.transform.GetForward();
// 	Quaternion newRotation = QuaternionMultiply( 
// 		QuaternionFromEulerV(yawAxis*Game::player.transform.GetYaw()),	// Yaw
// 		QuaternionFromEulerV(Game::player.transform.GetRight()*Game::player.transform.GetPitch())	// Pitch
// 	); // The order we multiply seems to matter, otherwise we need to reset `camera.up` periodically

// 	Game::player.transform.AddPosition(translationDelta);	// Add the translation
// 	Game::player.transform.SetRotation(newRotation);		// Add the new rotation

// 	Game::player.UpdateCamera(); // Update player camera with the new transform
// }

void Game::UpdatePlayer(bool yawLocked) {
	
	// TODO: convert this to yaw/pitch based w/ clamping
	
	float moveSpeed = 300.0f;
	float mouseSens = 1.0f;

	Vector2 rotationVector = DEG2RAD*Game::mouseDelta*mouseSens;

	//* Rotational Axis
	Vector3 yawAxis = yawLocked ? Vector3{0,1,0} : Game::player.transform.GetUp(); // Axis of rotation for yaw
	
	//* Translation -- Use `yawAxis` for up/down translation
	Vector3 translationDelta = Game::dt * moveSpeed * (Game::movementVector.x * Game::player.transform.GetRight() + Game::movementVector.y * yawAxis + Game::movementVector.z * Game::player.transform.GetForward());


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
	const Vector2 screenSize(1600, 900);
	const int screenWidth = screenSize.x;
	const int screenHeight = screenSize.y;

	Game::player = Player(Vector3(0.0f, 0.0f, 0.0f), Vector4(), 72.0f, true);

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

	Model sphere1 = LoadModelFromMesh(GenMeshSphere(100, 16, 32));
	Model sphere2 = LoadModelFromMesh(GenMeshSphere(100, 4, 8));
	sphere1.materials[0].shader = lightingShader; // Assign lighting shader to model
	sphere2.materials[0].shader = lightingShader;

	// Create light
	Light light = CreateLight(LIGHT_DIRECTIONAL, (Vector3){ -2, 1, -2 }, Vector3Zero(), WHITE, lightingShader);


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
	const float scrollModifier = 0.01f;
	


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
		if (IsKeyPressed(KEY_O)) {
			Matrix mat = MatrixInvert(QuaternionToMatrix(Game::player.transform.GetRotation()));
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m0, mat.m4, mat.m8, mat.m12) << std::endl;
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m1, mat.m5, mat.m9, mat.m13) << std::endl;
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m2, mat.m6, mat.m10, mat.m14) << std::endl;
			std::cout << TextFormat("[ %0f, %0f, %0f, %0f ]", mat.m3, mat.m7, mat.m11, mat.m15) << std::endl;
		}
		float mWheelDelta = GetMouseWheelMove();
		if (mWheelDelta != 0) {
			ambientStrength += mWheelDelta * scrollModifier;
			ambientStrength = std::clamp(ambientStrength, 0.0f, 1.0f);
		}


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

				// DrawGrid(20, 50);

				DrawModel(sphere1, Vector3(-250.0f, 0.0f, 250.0f), 1.0f, LIME);
				DrawModel(sphere2, Vector3( 250.0f, 0.0f, 250.0f), 1.0f, SKYBLUE);

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
	UnloadShader(lightingShader);
	UnloadShader(starsShader);
	UnloadRenderTexture(starsTexture);
	UnloadRenderTexture(bufferTexture);
	CloseWindow();

	return 0;
}

