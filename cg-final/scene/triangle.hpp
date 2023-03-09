#pragma once
#include <glm/glm.hpp>
#include "material.hpp"

class Grid;

struct Triangle {
	glm::vec3 position[3];
	glm::vec3 normal[3];
	glm::vec2 texcoord[3];
	Material* mat;

	glm::vec3 get_normal(glm::vec3 point) const;
	bool intersect(const Ray& r, Hit& h, float tmin);
	void insertIntoGrid(Grid* g);
};