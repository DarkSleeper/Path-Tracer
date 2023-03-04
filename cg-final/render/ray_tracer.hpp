#pragma once

#include <glm/glm.hpp>
#include "ray.hpp"
#include "hit.hpp"
#include "../scene/scene_parser.hpp"


#define MAXnum 100000
#define epsilon 0.01f
#define M_PI 3.1415926
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
	}
	~Ray_Tracer() {}

	glm::vec3 traceRay(Ray &ray, float tmin, int bounces, float weight, float indexOfRefraction, Hit &hit) const {
		if (bounces > max_bounces || weight < cutoff_weight) {
			glm::vec3 zero(0, 0, 0);
			return zero;
		}
		//	cout << bounces << endl;

		int k;
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

			color = scene->getAmbientLight() * hit.getMaterial()->diffuse;
			int num_lights = scene->getNumLights();
			// todo: change ray call
			for (k = 0; k < num_lights; k++) {
				scene->getIllumination(k, pin, ldir, clit, Dis2Lit);
				if (shade_back) pin = pin + ldir * epsilon;//这里是epsilon
				Ray ray2(pin, ldir);
				Hit hit2(Dis2Lit, scene->bg_mat, n0);

				if (shade_back) {
					scene->group_intersect(ray2, hit2, 0);
				}
				else {
					scene->group_intersect(ray2, hit2, epsilon);
				}
				if (shadows == 0) {
					color += (hit.getMaterial())->shade(ray, hit, ldir, clit);
					//		} else if (hit2.getT() == Dis2Lit) {
				}
				else  if (hit2.getT() == INFINITY || posive(hit2.getT() - Dis2Lit) < 0.0001) {//
					color += (hit.getMaterial())->shade(ray, hit, ldir, clit);
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
	Scene_Parser* scene;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
	bool shade_back;
};
