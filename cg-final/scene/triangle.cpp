#include "triangle.hpp"
#include "grid.hpp"
#include "../tool/determinant.hpp"
#define posive(X) ((X>=0) ? (X) : (-1 * (X)))

glm::vec3 Triangle::get_normal(glm::vec3 point) const
{
	// todo
	auto norm = normal[0] + normal[1] + normal[2];
	norm = glm::normalize(norm);
	return norm;
}

bool Triangle::intersect(const Ray &r, Hit &h, float tmin) {
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
			h.hit_triangle = this;
		}
		state = true;
	}
	else state = false;
	return state;
}

void Triangle::insertIntoGrid(Grid *g) {

	/////
	auto vec3_larger_and_equal = [](glm::vec3 a, glm::vec3 b) -> bool {
		if (a.x >= b.x && a.y >= b.y && a.z >= b.z) return true;
		else return false;
	};
	////

	glm::vec3 triv0, triv1;
	auto a = position[0]; auto b = position[1]; auto c = position[2];
	triv0 = glm::vec3(min3(a.x, b.x, c.x), min3(a.y, b.y, c.y), min3(a.z, b.z, c.z));
	triv1 = glm::vec3(max3(a.x, b.x, c.x), max3(a.y, b.y, c.y), max3(a.z, b.z, c.z));

	glm::vec3 v0, v1, pos, pos2;
	glm::vec3 x(g->lx, 0, 0), y(0, g->ly, 0), z(0, 0, g->lz), a0(g->lx, g->ly, g->lz);
	g->bb->get(v0, v1);
	pos = v0;
	int i, j, k;
	int nx = g->nx;
	int ny = g->ny;
	int nz = g->nz;
	triv0 -= glm::vec3(epsilon2, epsilon2, epsilon2);
	triv1 += glm::vec3(epsilon2, epsilon2, epsilon2);
	for (i = 0; i < nx; i++) {
		for (j = 0; j < ny; j++) {
			for (k = 0; k < nz; k++) {
				if (vec3_larger_and_equal(pos + a0 + epsilon2, triv0) && vec3_larger_and_equal(triv1 + epsilon2, pos)) {
					g->occ[i][j][k] = 1;
					g->objs[i][j][k].addObject(this);
				}
				pos += z;
			}
			pos = glm::vec3(pos.x, pos.y, v0.z);
			pos += y;
		}
		pos = glm::vec3(pos.x, v0.y, v0.z);
		pos += x;
	}
}
