#pragma once
#include <string>
#include <glm/glm.hpp>

struct Camera_Config 
{
	std::string type{"perspective"};
	int width;
	int height;
	float fovy;
	glm::vec3 eye;
	glm::vec3 lookat;
	glm::vec3 up;
};