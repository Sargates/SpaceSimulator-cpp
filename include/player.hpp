#ifndef PLAYER_HPP
#define PLAYER_HPP


#include "raymath.hpp"
#include "object_transform.hpp"

struct Player {
	ObjectTransform transform;
	Camera3D camera = {0};
	void UpdateCamera(void);

	Player();
	Player(Vector3 position, Quaternion rotation, float fovY, bool faceY);
};
#endif /* PLAYER_HPP */
