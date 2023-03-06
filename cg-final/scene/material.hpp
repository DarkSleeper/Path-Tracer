#pragma once
#include <glm/glm.hpp>
#include "../render/hit.hpp"
#include "../render/ray.hpp"

struct Material 
{
	std::string name;

	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 transmittance;
	float shininess;
	float ior;       // index of refraction

	bool is_light{false};
	glm::vec3 radiance; // light_color

	virtual glm::vec3 shade(const Ray& ray, const Hit& hit, const glm::vec3& dir_to_light, const glm::vec3& light_color) const { return glm::vec3(0.f); }
};

struct Phong_Material: Material 
{
	virtual glm::vec3 shade(const Ray& ray, const Hit& hit, const glm::vec3& dir_to_light, const glm::vec3& light_color) const 
	{
		glm::vec3 v = glm::vec3(-1, -1, -1) * ray.getDirection();
		glm::vec3 n = hit.getNormal();
		glm::vec3 h = v + dir_to_light;
		h = glm::normalize(h);
		float an = glm::dot(n, h);
		if (glm::dot(dir_to_light, n) < 0) an = 0;
		an = pow(an, shininess);
		float ad = glm::dot(dir_to_light, n);
		if (ad < 0) ad = 0;
		return diffuse * light_color * ad / 3.1415926f + specular * light_color * an * (shininess + 1) / 2.f / 3.1415926f;
	}
};

struct Light_Material: Material 
{
	virtual glm::vec3 shade(const Ray& ray, const Hit& hit, const glm::vec3& dir_to_light, const glm::vec3& light_color) const 
	{
		// set void intentionally
		return radiance;
	}
};

struct Background_Material: Material 
{
	virtual glm::vec3 shade(const Ray& ray, const Hit& hit, const glm::vec3& dir_to_light, const glm::vec3& light_color) const 
	{
		return diffuse;
	}
};