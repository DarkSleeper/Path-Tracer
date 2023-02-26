#pragma once
#include <glm/glm.hpp>

struct Material {
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 transmittance;
	float shininess;
	float ior;       // index of refraction
};