#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <time.h>
#include <stdlib.h>
#include "ray.hpp"
#include "hit.hpp"
#include "../scene/scene_parser.hpp"


#define MAXnum 100000
#define epsilon 0.001f
#define M_PI 3.1415926f
#define posive(X) ((X>=0) ? (X) : (-1 * (X)))
#define DegreesToRadians(x) ((M_PI * x) / 180.0f)


class Ray_Tracer {
public:
	Ray_Tracer() {}
	Ray_Tracer(Scene_Parser *s, int m_bounces, float c_weight, bool shads, bool back) {
		scene = s;
		max_bounces = m_bounces;
		cutoff_weight = c_weight;
		shadows = shads;
		shade_back = back;

		srand((unsigned int)time(NULL));
	}
	~Ray_Tracer() {}

	glm::vec3 traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
		if (bounces > max_bounces || weight < cutoff_weight) {
			glm::vec3 zero(0, 0, 0);
			return zero;
		}
		//	cout << bounces << endl;

		glm::vec3 ldir, clit, pin;
		glm::vec3 color(0, 0, 0);
		float Dis2Lit;
		glm::vec3 n0(0, 0, 0), normal;

		bool intsec;
		intsec = scene->group_intersect(ray, hit, tmin);

		if (intsec) {


			normal = hit.getNormal();
			pin = hit.getIntersectionPoint();

			if (glm::dot(normal, ray.getDirection()) > 0 && shade_back == 1)
				hit.set(hit.getT(), hit.getMaterial(), -1.f * normal, ray);

			//color = scene->getAmbientLight() * hit.getMaterial()->diffuse;
			int num_lights = scene->getNumLights();
			// todo: change ray call

			auto light_weight = rand() / float(RAND_MAX);
			auto k = choose_light(light_weight);
			{
				// compute light
				scene->getIllumination(k, pin, ldir, clit, Dis2Lit);
				clit = clit / powf(Dis2Lit, 2) * scene->light_areas[k];

				auto new_pin = pin;
				if (shade_back) new_pin = new_pin + ldir * epsilon;//这里是epsilon
				Ray ray2(new_pin, ldir);
				Hit hit2(Dis2Lit, scene->bg_mat, n0);

				if (shade_back) {
					scene->group_intersect(ray2, hit2, 0);
				} else {
					scene->group_intersect(ray2, hit2, epsilon);
				}
				if (shadows == 0) {
					color += (hit.getMaterial())->shade(ray, hit, ldir, clit);
				} else  if (hit2.getT() == INFINITY || posive(hit2.getT() - Dis2Lit) < 0.0001) {
					color += (hit.getMaterial())->shade(ray, hit, ldir, clit);
				}
			}

			if (!hit.getMaterial()->is_light) {
				auto next_dir = glm::vec3();
				{
					auto r1 = rand() / float(RAND_MAX);
					auto r2 = rand() / float(RAND_MAX);
					next_dir = glm::vec3(sqrtf(1 - powf(r1, 2)) * cosf(2 * M_PI * r2), sqrtf(1 - powf(r1, 2)) * sinf(2 * M_PI * r2), r1);
					//next_dir = glm::normalize(next_dir);

					auto z = glm::vec3(0, 0, 1);
					auto new_z = hit.getNormal();
					if (posive(glm::dot(z, new_z)) >= 1 - epsilon) z = glm::vec3(0, 1, 0);
					auto new_x = glm::cross(z, new_z);
					new_x = glm::normalize(new_x);
					auto new_y = glm::cross(new_z, new_x);
					new_y = glm::normalize(new_y);
					auto rot_matrix = glm::mat3x3{ new_x,new_y,new_z };

					next_dir = rot_matrix * next_dir;
					//if (glm::dot(next_dir, new_z) < 0) std::cout << "nooo";
				}

				Ray next_ray(pin, next_dir);
				Hit next_hit(MAXnum, scene->bg_mat, n0);
				auto next_shade = traceRay(next_ray, epsilon, bounces + 1, weight, indexOfRefraction, next_hit);

				if (!next_hit.getMaterial()->is_light) {
					color += (hit.getMaterial())->shade(ray, hit, next_dir, next_shade) * (2.f * M_PI * M_PI);
				}
			}


			//glm::vec3 reflectColor = hit.getMaterial()->getReflectiveColor();
			//if (reflectColor.Length() > 0) {
			//	glm::vec3 rdir = mirrorDirection(normal, ray.getDirection());
			//	glm::vec3 rori = hit.getIntersectionPoint();
			//	rori = rori + rdir * epsilon;//先移动epsilon

			//	Ray reray(rori, rdir);
			//	Hit rhit;
			//	float rweight = weight * reflectColor.Length();
			//	rhit.set(MAXnum, scene->getMaterial(0), n0, reray);
			//	color += reflectColor *
			//		traceRay(reray, 0, bounces + 1, rweight, indexOfRefraction, rhit);
			//}

			//if (isvisgrid == 0) {
			//	glm::vec3 transColor = hit.getMaterial()->getTransparentColor();
			//	if (transColor.Length() > 0) {
			//		glm::vec3 tdir;
			//		glm::vec3 tori = hit.getIntersectionPoint();
			//		glm::vec3 income = -1 * ray.getDirection();
			//		float index_i, index_t;
			//		if (normal.Dot3(income) > 0) {
			//			index_i = 1;
			//			index_t = hit.getMaterial()->getIndexOfRefraction();
			//		}
			//		else {
			//			index_t = 1;
			//			index_i = indexOfRefraction;
			//			normal *= -1;
			//		}

			//		if (transmittedDirection(normal, income, index_i, index_t, tdir)) {
			//			tori = tori + tdir * epsilon;
			//			Ray tray(tori, tdir);
			//			Hit thit;
			//			float tweight = weight * transColor.Length();
			//			float ior = hit.getMaterial()->getIndexOfRefraction();
			//			thit.set(MAXnum, scene->getMaterial(0), n0, tray);
			//			color += transColor *
			//				traceRay(tray, 0, bounces + 1, tweight, ior, thit);
			//		}

			//	}
			//}
		}
		else {
			color = scene->bg_color;
		}
		return color;
	}
private:
	auto choose_light(float light_weight) const -> int
	{
		for (auto i = 0; i < scene->light_triangles.size(); i++) {
			if (light_weight <= scene->light_weight_prefix_sum[i])
				return i;
		}
		return 0;
	}

	Scene_Parser* scene;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
	bool shade_back;
};
