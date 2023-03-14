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

		if (hit.getMaterial()->is_refelect == false) {

			if (glm::dot(ray.getDirection(), hit.getNormal()) > 0) {
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
				auto sample_choice = rand() / float(RAND_MAX);
				float w1, w2;
				{
					// compute light
					glm::vec3 ldir, clit;
					float Dis2Lit;
					scene->getIllumination(k, pin, ldir, clit, Dis2Lit);

					Ray ray2(pin, ldir);
					Hit hit2(MAXnum, scene->bg_mat, glm::vec3(0, 0, 0));

					bool intersect2 = scene->group_intersect_grid(ray2, hit2, epsilon);

					if (intersect2 && glm::dot(hit2.getNormal(), ldir) < 0.f && posive(hit2.getT() - Dis2Lit) < 0.001) {
						auto cos_theta = glm::dot((-1.f) * ldir, hit2.getNormal());
						auto area = scene->light_areas[k];
						pdf_light = powf(Dis2Lit, 2) / cos_theta / scene->area_sum;

						pdf_scene = glm::dot(ldir, hit.getNormal()) / M_PI;
						auto shininess = hit.getMaterial()->shininess;
						auto next_h = ray.getDirection() * (-1.f) + ldir;
						next_h = glm::normalize(next_h);
						pdf_scene += (shininess + 1) / (2.f * M_PI) * powf(glm::dot(hit.getNormal(), next_h), shininess) / (4.0f * glm::dot(ldir, next_h));
						pdf_scene /= 2.f;
						w1 = powf(pdf_light, 2) / (powf(pdf_light, 2) + powf(pdf_scene, 2));

						color += w1 * (hit.getMaterial())->shade(ray, hit, ldir, clit) / pdf_light;
					}
				}

				auto next_dir = glm::vec3();
				{
					auto z = glm::vec3(0, 0, 1);
					auto new_z = hit.getNormal();
					if (posive(glm::dot(z, new_z)) >= 1 - epsilon) z = glm::vec3(0, 1, 0);
					auto new_x = glm::cross(z, new_z);
					new_x = glm::normalize(new_x);
					auto new_y = glm::cross(new_z, new_x);
					new_y = glm::normalize(new_y);
					auto rot_matrix = glm::mat3x3{ new_x,new_y,new_z };

					auto shininess = hit.getMaterial()->shininess;
					if (sample_choice < 0.5) { // diffuse
						auto r1 = rand() / float(RAND_MAX);
						auto theta = 0.5 * acosf(1 - 2 * r1);
						auto r2 = rand() / float(RAND_MAX);
						auto alpha = 2 * M_PI * r2;
						next_dir = glm::vec3(cosf(alpha) * sinf(theta), sinf(alpha) * sinf(theta), cosf(theta));
						next_dir = glm::normalize(next_dir);

						next_dir = rot_matrix * next_dir;
				
					} else { // specular
						auto r1 = rand() / float(RAND_MAX);
						auto r2 = rand() / float(RAND_MAX);
						auto theta = acosf(powf(r1, 1.0f / (shininess + 1)));
						auto alpha = 2 * M_PI * r2;
						auto next_h = glm::vec3(cosf(alpha) * sinf(theta), sinf(alpha) * sinf(theta), cosf(theta));
						next_h = glm::normalize(next_h);

						next_h = rot_matrix * next_h;

						auto in_ray = ray.getDirection();
						next_dir = in_ray - 2.f * glm::dot(in_ray, next_h) * next_h;
						next_dir = glm::normalize(next_dir);

					}

					pdf_scene = glm::dot(next_dir, hit.getNormal()) / M_PI;
					auto next_h = ray.getDirection() * (-1.f) + next_dir;
					next_h = glm::normalize(next_h);
					pdf_scene += (shininess + 1) / (2.f * M_PI) * powf(glm::dot(hit.getNormal(), next_h), shininess) / (4.0f * glm::dot(next_dir, next_h));
					pdf_scene /= 2.f;
				}

				if (glm::dot(next_dir, hit.getNormal()) >= 0) {
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
							pdf_light = powf(Dis2Lit, 2) / glm::dot((-1.f) * next_ray.getDirection(), next_hit.getNormal()) / scene->area_sum;
							w2 = powf(pdf_scene, 2) / (powf(pdf_light, 2) + powf(pdf_scene, 2));
							color += w2 * (hit.getMaterial())->shade(ray, hit, next_dir, next_shade) / pdf_scene;
						}
					} else {
						color += scene->bg_color;
					}
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

		} else { // reflect
			auto color = glm::vec3(0.f);

			auto normal = hit.getNormal();
			auto in_ray = ray.getDirection();
			glm::vec3 transColor = hit.getMaterial()->transmittance;
			auto n_io = 1.f;
			if (glm::dot(in_ray, normal) > 0) { //in_to_out
				normal = -1.f * normal;
				n_io = indexOfRefraction / 1.f;
			} else {
				n_io = 1.f / hit.getMaterial()->ior;
			}
			auto cos_in = -1.f * glm::dot(in_ray, normal);
			auto sin_in = sqrtf(1 - cos_in * cos_in);
			auto fr = 1.0f;
			if (n_io * sin_in < 1) { // refraction
				auto cos_out = sqrtf(1 - (n_io * sin_in) * (n_io * sin_in));
				auto r0 = (cos_in - n_io * cos_out) / (cos_in + n_io * cos_out);
				auto r1 = (n_io * cos_in - cos_out) / (n_io * cos_in + cos_out);
				fr = 0.5f * (r0 * r0 + r1 * r1);
				auto re_fr = 1 - fr;

				if (glm::length(transColor) > 0) {
					glm::vec3 tdir;
					glm::vec3 tori = hit.getIntersectionPoint();
					float index_i, index_t;

					tdir = -cos_out * normal + n_io * (in_ray - glm::dot(in_ray, normal) * normal);
					tori = tori + tdir * epsilon;
					Ray tray(tori, tdir);
					Hit thit;
					float ior = hit.getMaterial()->ior;
					thit.set(MAXnum, scene->bg_mat, glm::vec3(0.f), tray);

					auto intsec = scene->group_intersect_grid(tray, thit, 0);
					if (intsec) {
						color += re_fr * transColor * Shade(tray, thit, bounces + 1, weight, ior);
					} else {
						color += scene->bg_color;
					}
				}

			}


			glm::vec3 rdir = in_ray - 2.f * glm::dot(in_ray, normal) * normal;
			glm::vec3 rori = hit.getIntersectionPoint();
			rori = rori + rdir * epsilon;//ÏÈÒÆ¶¯epsilon

			Ray reray(rori, rdir);
			Hit rhit;
			rhit.set(MAXnum, scene->bg_mat, glm::vec3(0.f), reray);
			auto intsec = scene->group_intersect_grid(reray, rhit, 0);
			if (intsec) {
				color += fr * transColor * Shade(reray, rhit, bounces + 1, weight, indexOfRefraction);
			} else {
				color += scene->bg_color;
			}

			return color;
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
