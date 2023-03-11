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

	glm::vec3 Shade(Ray const& ray, Hit const& hit, int bounces, float weight, float indexOfRefraction) const
	{
		if (bounces > max_bounces) {
			glm::vec3 zero(0, 0, 0);
			return zero;
		}

		if (!hit.getMaterial()->is_light) {

			glm::vec3 color(0, 0, 0);
			
			auto normal = hit.getNormal();
			auto pin = hit.getIntersectionPoint();

			int num_lights = scene->getNumLights();

			auto light_weight = rand() / float(RAND_MAX);
			auto k = choose_light(light_weight);
			auto pdf_light = (float)0.f;
			auto pdf_scene = 0.5f / M_PI;
			float w1, w2;
			{
				// compute light
				glm::vec3 ldir, clit;
				float Dis2Lit;
				scene->getIllumination(k, pin, ldir, clit, Dis2Lit);

				Ray ray2(pin, ldir);
				Hit hit2(MAXnum, scene->bg_mat, glm::vec3(0, 0, 0));

				bool intersect2 = scene->group_intersect_grid(ray2, hit2, epsilon);

				if (intersect2 && glm::dot(hit2.getNormal(), ldir) < 0.f && posive(hit2.getT() - Dis2Lit) < 0.0001) {
					auto cos_theta = glm::dot((-1.f) * ldir, hit2.getNormal());
					auto area = scene->light_areas[k];
					pdf_light = powf(Dis2Lit, 2) / cos_theta / area;
					pdf_scene = glm::dot(ldir, hit.getNormal()) / M_PI;
					w1 = powf(pdf_light, 2) / (powf(pdf_light, 2) + powf(pdf_scene, 2));

					color += w1 * (hit.getMaterial())->shade(ray, hit, ldir, clit) / pdf_light;
				}
			}

			auto next_dir = glm::vec3();
			{
				auto r1 = rand() / float(RAND_MAX);
				auto alpha = 0.5 * acosf(1 - 2 * r1);
				auto theta = rand() / float(RAND_MAX);
				next_dir = glm::vec3(cosf(alpha) * cosf(2 * M_PI * theta), cosf(alpha) * sinf(2 * M_PI * theta), sinf(alpha));
				next_dir = glm::normalize(next_dir);

				auto z = glm::vec3(0, 0, 1);
				auto new_z = hit.getNormal();
				if (posive(glm::dot(z, new_z)) >= 1 - epsilon) z = glm::vec3(0, 1, 0);
				auto new_x = glm::cross(z, new_z);
				new_x = glm::normalize(new_x);
				auto new_y = glm::cross(new_z, new_x);
				new_y = glm::normalize(new_y);
				auto rot_matrix = glm::mat3x3{ new_x,new_y,new_z };

				next_dir = rot_matrix * next_dir;
				
				pdf_scene = glm::dot(next_dir, hit.getNormal()) / M_PI;
			}

			Ray next_ray(pin, next_dir);
			Hit next_hit(MAXnum, scene->bg_mat, glm::vec3(0, 0, 0));
			bool intsec = false;
			intsec = scene->group_intersect_grid(next_ray, next_hit, epsilon);

			if (intsec) {
				if (!next_hit.getMaterial()->is_light) {
					auto next_shade = Shade(next_ray, next_hit, bounces + 1, weight, indexOfRefraction);
					color += (hit.getMaterial())->shade(ray, hit, next_dir, next_shade) / pdf_scene;
				}
				else if (glm::dot(next_ray.getDirection(), next_hit.getNormal()) < 0) {
					auto next_shade = next_hit.getMaterial()->radiance;
					auto Dis2Lit = glm::length(next_hit.getIntersectionPoint() - hit.getIntersectionPoint());
					pdf_light = powf(Dis2Lit, 2) / glm::dot((-1.f) * next_ray.getDirection(), next_hit.getNormal()) / scene->compute_area(*next_hit.hit_triangle);
					w2 = powf(pdf_scene, 2) / (powf(pdf_light, 2) + powf(pdf_scene, 2));
					color += w2 * (hit.getMaterial())->shade(ray, hit, next_dir, next_shade) / pdf_scene;
				}
			} else {
				color += scene->bg_color;
			}
			return color;
		} 
		else {
			if (glm::dot(ray.getDirection(), hit.getNormal()) <= 0) {
				return hit.getMaterial()->radiance;
			}
			else {
				return glm::vec3(0.f);
			}
		}
	}

	glm::vec3 traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
		if (bounces > max_bounces || weight < cutoff_weight) {
			glm::vec3 zero(0, 0, 0);
			return zero;
		}
		//	cout << bounces << endl;

		glm::vec3 color(0, 0, 0);

		bool intsec;
		intsec = scene->group_intersect_grid(ray, hit, tmin);

		if (intsec) {

			color = Shade(ray, hit, bounces, weight, indexOfRefraction);


			//glm::vec3 reflectColor = hit.getMaterial()->getReflectiveColor();
			//if (reflectColor.Length() > 0) {
			//	glm::vec3 rdir = mirrorDirection(normal, ray.getDirection());
			//	glm::vec3 rori = hit.getIntersectionPoint();
			//	rori = rori + rdir * epsilon;//���ƶ�epsilon

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
		light_weight *= scene->area_sum;
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
