#pragma once
#include <glm/glm.hpp>

struct Triangle {
	glm::vec3 position[3];
	glm::vec3 normal[3];
	glm::vec2 texcoord[3];
	int mat_idx;
};