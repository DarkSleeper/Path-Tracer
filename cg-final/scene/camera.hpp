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

class Perspective_Camera
{
public:
	Perspective_Camera(Camera_Config config) 
	{
		inner_config(config.eye, config.lookat - config.eye, config.up, config.fovy);
	}

	void inner_config(glm::vec3 cer, glm::vec3 dir, glm::vec3 upp, float ang) {
		center = cer;
		direction = dir;
		direction = glm::normalize(direction);

		glm::vec3 tmp;
		tmp = direction * glm::dot(upp, direction);
		up = upp - tmp;
		up = glm::normalize(up);

		horizontal = glm::cross(direction, up);
		angle = ang;
	}

	Ray generate_ray(glm::vec2 point) {
		direction = glm::normalize(direction);
		horizontal = glm::cross(direction, up);
		horizontal = glm::normalize(horizontal);
		screenup = glm::cross(horizontal, direction);

		glm::vec3 position, xray, yray;
		float x, y;
		x = point.x;
		y = point.y;
		assert(x >= 0 && x < 1);
		assert(y >= 0 && y < 1);
		xray = horizontal * x;
		yray = screenup * y;

		float a, c;
		c = 0.5 / tanf(angle / 2);
		a = sqrtf(c*c - 0.5*0.5);
		position = center + direction * a;
		position = position - 0.5f * screenup - 0.5f * horizontal;
		position += xray + yray;
		glm::vec3 raydir = position - center;
		raydir = glm::normalize(raydir);
		Ray tmp(center, raydir);
		return tmp;
	}

	float get_t_min() const {
		return 0;
	}

private:
	glm::vec3 center;
	glm::vec3 direction;
	glm::vec3 up;
	glm::vec3 screenup;
	glm::vec3 horizontal;
	float angle;
};
