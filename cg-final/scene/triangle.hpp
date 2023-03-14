#pragma once
#include <glm/glm.hpp>

class Grid;
class Hit;
struct Material;
struct Ray;

struct Triangle {
	glm::vec3 position[3];
	glm::vec3 normal[3];
	glm::vec2 texcoord[3];
	Material* mat;

	glm::vec3 get_normal(glm::vec3 point) const;
	glm::vec2 get_uv(glm::vec3 point) const;
	bool intersect(const Ray& r, Hit& h, float tmin);
	void insertIntoGrid(Grid* g);
};