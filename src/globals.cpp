

#include <iostream>
#include <string>
#include <vector>


#include "raymath.hpp"
#include "raylib.hpp"
#include "object_transform.hpp"
#include "planet.hpp"
#include "player.hpp"
#include "globals.hpp"

namespace Universe {
	int G = 200;
}


namespace Game {
	float dt = 0.0f;
	Vector2 mouseDelta;
	Vector2 mouseWheelDelta;
	Vector3 movementVector;
	bool debugMode = false;


	void GeneralUpdate() {
		dt = GetFrameTime();
		mouseDelta = IsCursorHidden() ? GetMouseDelta() * 0.05f : Vector2::Zero;
		mouseWheelDelta = GetMouseWheelMoveV();
		movementVector = { 0, 0, 0 };

		if (IsKeyDown(KEY_D)) 			 { movementVector.x += 1; }
		if (IsKeyDown(KEY_A)) 			 { movementVector.x -= 1; }
		if (IsKeyDown(KEY_SPACE)) 		 { movementVector.y += 1; }
		if (IsKeyDown(KEY_LEFT_CONTROL)) { movementVector.y -= 1; }
		if (IsKeyDown(KEY_W)) 			 { movementVector.z += 1; }
		if (IsKeyDown(KEY_S)) 			 { movementVector.z -= 1; }

		// Center origin around player to prevent float-precision issues when far away
		if (Vector3LengthSqr(player.transform.GetPosition()) > PLAYER_DISTANCE_RESET_SQUARED) {
			for (Planet& planet : planets)
				planet.transform.AddPosition(-player.transform.GetPosition());
			PseudoOrigin += player.transform.GetPosition();
			player.transform.SetPosition(Vector3::Zero);
		}

		// Update Acceleration
		for (Planet& planetI : planets) {
			Vector3 newAcc = Vector3::Zero;
			for (Planet& planetJ : planets) {
				if (planetI.id == planetJ.id) continue;
				float sqrDst = Vector3LengthSqr(planetI.transform.GetPosition() - planetJ.transform.GetPosition());
				Vector3 dir = Vector3Normalize(planetJ.transform.GetPosition() - planetI.transform.GetPosition());
				newAcc += dir*(planetJ.mass * Universe::G / std::max(0.000000001f, sqrDst)); // Use `max` to ensure `sqrDst` is never 0.
			}
			planetI.Acceleration = newAcc; // Set this, don't add it
		}
	}

	void UpdatePlayer(bool yawLocked) {
		
		// TODO: convert this to yaw/pitch based w/ clamping

		float moveSpeed = 300.0f;
		if (IsKeyDown(KEY_LEFT_SHIFT)) { moveSpeed *= 40; }
		float mouseSens = 1.0f;

		Vector2 rotationVector = DEG2RAD*mouseDelta*mouseSens;

		//* Rotational Axis
		Vector3 yawAxis = yawLocked ? Vector3{0,1,0} : player.transform.GetUp(); // Axis of rotation for yaw
		
		//* Translation -- Use `yawAxis` for up/down translation
		Vector3 translationDelta = dt * moveSpeed * (movementVector.x * player.transform.GetRight() + movementVector.y * yawAxis + movementVector.z * player.transform.GetForward());


		//* Horizontal Movement of Mouse
		// Vector3 targetPosition = player.transform.GetForward();
		Quaternion newRotation = QuaternionMultiply(
			QuaternionFromEulerV(player.transform.GetRight() * rotationVector.y),	// Pitch
			QuaternionFromEulerV(yawAxis * rotationVector.x)							// Yaw
		); // The order we multiply seems to matter, otherwise we need to reset `camera.up` periodically

		player.transform.AddPosition(translationDelta);	// Add the translation
		player.transform.AddRotation(newRotation);		// Add the new rotation

		player.UpdateCamera(); // Update player camera with the new transform
	}

}
std::vector<ObjectTransform*> Game::physicsObjects;
std::vector<Planet> Game::planets;
Player Game::player;
Vector3 Game::PseudoOrigin;