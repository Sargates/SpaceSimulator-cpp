
#include "raymath.hpp"
#include "raylib.hpp"
#include "rlights.hpp"
#include "globals.hpp"
#include "object_transform.hpp"

#include "planet.hpp"

#include <iostream>
#include <cmath>


int Planet::numPlanets = 0;

Planet::Planet(Vector3 position, float radius, float surfaceGravity, Color color, Shader* shader, Light* light) : transform(position), radius(radius), surfaceGravity(surfaceGravity), color(color), shader(shader), light(light) {
	id = Planet::numPlanets++;
	float sizeRatio = powf(radius, 1/log2f(3)); // == 2^log_3(radius)
	model = LoadModelFromMesh(GenMeshSphere(
		radius/2.0,
		std::clamp(sizeRatio, 8.0f, 50.0f),
		2*std::clamp(sizeRatio, 8.0f, 50.0f)
	));
	if (shader != nullptr) model.materials[0].shader = *shader;
	UpdateLight();

	mass = surfaceGravity * radius * radius / Universe::G;
}

void Planet::UpdatePosition(float dt) {
	transform.AddPosition(Velocity * dt);
	UpdateLight();
}
void Planet::UpdateVelocity(float dt) { Velocity += Acceleration * dt; }

void Planet::UpdateLight() { if (light != nullptr) light->position = transform.GetPosition(); }

void Planet::Draw() {
	if (shader != nullptr) BeginShaderMode(*shader);
		DrawModel(model, transform.GetPosition(), 1.0f, color);
	if (shader != nullptr) EndShaderMode();
}

void Planet::Free() {
	std::cout << "Freeing Planet-ID: " << this->id << std::endl;
	UnloadModel(model);
}