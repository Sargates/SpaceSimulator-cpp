
#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <vector>

#include "raymath.hpp"
#include "object_transform.hpp"
#include "player.hpp"
#include "planet.hpp"


#define PLAYER_DISTANCE_RESET			1000
#define PLAYER_DISTANCE_RESET_SQUARED	PLAYER_DISTANCE_RESET*PLAYER_DISTANCE_RESET


namespace Universe {
	extern int G;
}

class ObjectTransform;
class Planet;
class Player;

namespace Game {
	extern float dt;
	extern Vector2 mouseDelta; 							// Mouse movement
	extern Vector2 mouseWheelDelta;						// Mouse wheel delta -- Normal scrollwheels are in the `y` component
	extern Vector3 movementVector; 						// WASD, etc.
	extern bool debugMode;									// Debug Mode

	void GeneralUpdate(void);						// Should be called once per frame
	void UpdatePlayer(bool yawLocked=true);			// Perform necessary updates on camera

	
	extern std::vector<ObjectTransform*> physicsObjects; 	// This vector should only contain `ObjectTransform`'s that do not have a parent
	extern std::vector<Planet> planets;					// This contains all the planets in the world
	extern Player player;									// The player
	extern Vector3 PseudoOrigin;							// This is the position of the origin after the player position has cause every transform to get shifted
}

#endif /* GLOBALS_HPP */
