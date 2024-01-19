
#include "raymath.hpp"
#include "raylib.hpp"
#include "rcamera.hpp"
#include "globals.hpp"

#include "player.hpp"


Player::Player() : transform(Vector3::Zero, Vector4::Zero) {
	camera.projection = CAMERA_PERSPECTIVE;
	this->UpdateCamera();
	camera.up = Vector3{0, 1, 0};
	camera.fovy = 60;
}

Player::Player(Vector3 position, Quaternion rotation, float fovY, bool faceY) : transform(position, rotation, faceY) {
	camera.projection = CAMERA_PERSPECTIVE;
	this->UpdateCamera();
	camera.up = Vector3{0, 1, 0};
	camera.fovy = fovY;
}


void Player::UpdateCamera(void) {
	camera.position = transform.GetPosition();
	// Set target to be inline with camera's position
	camera.target = Vector3Transform({ 0, 0, 1 }, transform.GetWorldMatrix());
	camera.up = transform.GetUp();
}