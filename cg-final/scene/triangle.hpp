#pragma once
#include <glm/glm.hpp>
#include "material.hpp"
#include "../tool/determinant.hpp"

struct Triangle {
	glm::vec3 position[3];
	glm::vec3 normal[3];
	glm::vec2 texcoord[3];
	Material* mat;

	glm::vec3 get_normal(glm::vec3 point) const 
	{
		// todo
		auto norm = normal[0] + normal[1] + normal[2];
		norm = glm::normalize(norm);
		return norm;
	}

	bool intersect(const Ray &r, Hit &h, float tmin) {
		bool state = true;
		glm::vec3 Ro, Rd;
		Ro = r.getOrigin();
		Rd = r.getDirection();

		auto A = position[0];
		auto B = position[1];
		auto C = position[2];

		float d0 = det3x3(A.x - B.x, A.x - C.x, Rd.x,
			A.y - B.y, A.y - C.y, Rd.y,
			A.z - B.z, A.z - C.z, Rd.z);
		float d1 = det3x3(A.x - Ro.x, A.x - C.x, Rd.x,
			A.y - Ro.y, A.y - C.y, Rd.y,
			A.z - Ro.z, A.z - C.z, Rd.z);
		float d2 = det3x3(A.x - B.x, A.x - Ro.x, Rd.x,
			A.y - B.y, A.y - Ro.y, Rd.y,
			A.z - B.z, A.z - Ro.z, Rd.z);
		float d3 = det3x3(A.x - B.x, A.x - C.x, A.x - Ro.x,
			A.y - B.y, A.y - C.y, A.y - Ro.y,
			A.z - B.z, A.z - C.z, A.z - Ro.z);
		float p1, p2, p3, t;
		p2 = d1 / d0;
		p3 = d2 / d0;
		t = d3 / d0;
		p1 = 1 - p2 - p3;
		if (p2 + p3 <= 1 && p2 >= 0 && p3 >= 0 && t >= tmin) {
			if (t < h.getT()) {
				auto pin = r.pointAtParameter(t);
				auto norm = get_normal(pin);
				h.set(t, mat, norm, r);
			}
			state = true;
		}
		else state = false;
		return state;
	}

};