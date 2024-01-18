// //! This code doesnt work as is with my vscode config -- commented out
// #define RAYMATH_IMPLEMENTATION
// #define RLIGHTS_IMPLEMENTATION

// // Include raylib deps
// #include "config.hpp"
// #include "raymath.hpp"
// #include "raylib.hpp"
// #include "rcamera.hpp"
// #include "rlights.hpp"
// #include "object_transform.hpp"



// #include <iostream>
// #include <string>
// #include <vector>
// #define print(x) std::cout << x << std::endl // print macro pog

// #include "screenhelper.hpp"

// #include <vector>
// #include <list>

// ObjectTransform PlayerBase;
// ObjectTransform CameraNode;
// ObjectTransform GunNode;

// Model GunModel = { 0 };

// void GameInit()
// {
// 	// build up a Hierarchy of transform nodes in a parent child relationship
// 	// 
// 	// Camera
// 	//   |\
// 	//   | \Gun
// 	//	 |	  	
// 	//   + Base
// 	// 
// 	// player base is where the players 'feet' are
// 	// its child is the camera node at an offset for the 'head'
// 	// the gun is a child of the camera so it moves with it

// 	PlayerBase.AddChild(CameraNode);
// 	CameraNode.MoveUp(1);
// 	CameraNode.AddChild(GunNode);

// 	GunNode.MoveDown(0.5f);
// 	GunNode.MoveRight(0.5f);

// 	// load a gun model
// 	GunModel = LoadModel("resources/blaster.glb");
// }

// bool GameUpdate()
// {
// 	//rotate the player base left and right
// 	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
// 		PlayerBase.RotateYaw(GetMouseDelta().x * -0.1f);

// 	// rotate the camera up and down
// 	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
// 		CameraNode.RotatePitch(GetMouseDelta().y * -0.1f);

// 	// move the player based on it's local transform
// 	if (IsKeyDown(KEY_W))
// 		PlayerBase.MoveForward(GetFrameTime() * 10);

//     if (IsKeyDown(KEY_S))
//         PlayerBase.MoveForward(GetFrameTime() * -10);

//     if (IsKeyDown(KEY_D))
//         PlayerBase.MoveRight(GetFrameTime() * 10);

//     if (IsKeyDown(KEY_A))
//         PlayerBase.MoveRight(GetFrameTime() * -10);

// 	return true;
// }

// void GameDraw()
// {
// 	BeginDrawing();
// 	ClearBackground(DARKGRAY);

// 	// the camera is not our controller, so we can always recreate it from the camera node
// 	Camera3D camera = { 0 };
// 	camera.projection = CAMERA_PERSPECTIVE;
// 	camera.fovy = 45;
// 	camera.up.y = 1;

// 	// where the camera node is in the world
// 	camera.position = CameraNode.GetWorldPosition();

// 	// where the depth vector of the camera node is in world space
// 	camera.target = Vector3Transform({ 0,0,1 }, CameraNode.GetWorldMatrix());

// 	BeginMode3D(camera);

// 	DrawGrid(10, 10);
// 	DrawCube(Vector3{ 0,0.5f,10 }, 1, 1, 1, RED);

// 	// push the OpenGL matrix for the gun node in world space
// 	// so we can draw the gun there
// 	GunNode.PushMatrix();
// 	// we are now drawing local to the gun node, this model needs to stick out a little, so we shift it by 1.5 in z
// 	DrawModel(GunModel, Vector3{0,0,1.5f}, 1, WHITE);
// 	GunNode.PopMatrix();

// 	EndMode3D();
// 	EndDrawing();
// }

// int main()
// {
// 	SetConfigFlags(FLAG_VSYNC_HINT);
// 	InitWindow(1280, 800, "Transform Example");
// 	SetTargetFPS(144);

// 	GameInit();

// 	while (!WindowShouldClose())
// 	{
// 		if (!GameUpdate())
// 			break;
		
// 		GameDraw();
// 	}

// 	UnloadModel(GunModel);

// 	CloseWindow();
// 	return 0;
// }