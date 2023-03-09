#pragma once

#include <glm/glm.hpp>
#include "../render/ray.hpp"
#include "../render/hit.hpp"
#include "material.hpp"
#include "bounding_box.hpp"
#include "triangle.hpp"
#include "object3d_vector.hpp"

#define posive(X) ((X>=0) ? (X) : (-1 * (X)))
#define epsilon2 0.00001


class MarchingInfo {
public:
	MarchingInfo() {}
	~MarchingInfo() {}
	void SetTmin(float& t) {
		tmin = t;
		return;
	}
	float GetTmin() {
		return tmin;
	}
	void SetIndice(int& _i, int& _j, int& _k) {
		i = _i;
		j = _j;
		k = _k;
		return;
	}
	void GetIndice(int& _i, int& _j, int& _k) {
		_i = i;
		_j = j;
		_k = k;
		return;
	}
	void SetNextT(float& tnx, float& tny, float& tnz) {
		t_next_x = tnx;
		t_next_y = tny;
		t_next_z = tnz;
		return;
	}
	void GetNextT(float& tnx, float& tny, float& tnz) {
		tnx = t_next_x;
		tny = t_next_y;
		tnz = t_next_z;
		return;
	}
	void SetIncrement(float dtx, float dty, float dtz) {
		d_tx = dtx;
		d_ty = dty;
		d_tz = dtz;
		return;
	}
	void GetIncrement(float& dtx, float& dty, float& dtz) {
		dtx = d_tx;
		dty = d_ty;
		dtz = d_tz;
		return;
	}
	void SetSign(int& sx, int& sy, int& sz) {
		sign_x = sx;
		sign_y = sy;
		sign_z = sz;
		return;
	}
	void GetSign(int& sx, int& sy, int& sz) {
		sx = sign_x;
		sy = sign_y;
		sz = sign_z;
		return;
	}
	void SetNormal(glm::vec3& _n) {
		n = _n;
		return;
	}
	glm::vec3 GetNormal() { return n; }
	void SetRay(Ray _r) {
		r = _r;
		return;
	}
	Ray GetRay() { return r; }
	void SetV(glm::vec3& vv0, glm::vec3& vv1) {
		v0 = vv0;
		v1 = vv1;
		return;
	}
	void nextCell() {
		//cout << tmin << endl;
		glm::vec3 dir;
		dir = r.getDirection();
		float tn;
		tn = t_next_x;
		if (t_next_y < tn) {
			tn = t_next_y;
		}
		if (t_next_z < tn) {
			tn = t_next_z;
		}
		tmin = tn;
		if (posive(tmin - t_next_x) < epsilon2) {
			i = i + sign_x;
			n = glm::vec3(-sign_x, 0, 0);
		}
		else if (posive(tmin - t_next_y) < epsilon2) {
			j = j + sign_y;
			n = glm::vec3(0, -sign_y, 0);
		}
		else if (posive(tmin - t_next_z) < epsilon2) {
			k = k + sign_z;
			n = glm::vec3(0, 0, -sign_z);
		}
		//计算到下个面的t值（x, y, z)
		glm::vec3 x(d_tx, 0, 0), y(0, d_ty, 0), z(0, 0, d_tz);
		glm::vec3 vv0 = v0 + x * (float)i + y * (float)j + z * (float)k;
		glm::vec3 vv1 = vv0 + x + y + z;
		glm::vec3 ori = r.getOrigin();
		float t[7];
		if (dir.x * sign_x > epsilon2) {
			t[1] = (vv0.x - ori.x) / dir.x;
			t[4] = (vv1.x - ori.x) / dir.x;
		}
		else {
			t[1] = INFINITY; t[4] = INFINITY;
		}
		if (dir.y * sign_y > epsilon2) {
			t[2] = (vv0.y - ori.y) / dir.y;
			t[5] = (vv1.y - ori.y) / dir.y;
		}
		else {
			t[2] = INFINITY; t[5] = INFINITY;
		}
		if (dir.z * sign_z > epsilon2) {
			t[3] = (vv0.z - ori.z) / dir.z;
			t[6] = (vv1.z - ori.z) / dir.z;
		}
		else {
			t[3] = INFINITY; t[6] = INFINITY;
		}
		if (sign_x == 1) t_next_x = t[4];
		else t_next_x = t[1];
		if (sign_y == 1) t_next_y = t[5];
		else t_next_y = t[2];
		if (sign_z == 1) t_next_z = t[6];
		else t_next_z = t[3];

	}
private:
	float tmin;//current T
	int i;
	int j;
	int k;//indice
	float t_next_x;
	float t_next_y;
	float t_next_z;//NextT
	float d_tx;
	float d_ty;
	float d_tz;//Increment
	int sign_x;
	int sign_y;
	int sign_z;//Sign
	glm::vec3 n;
	glm::vec3 v0;
	glm::vec3 v1;
	Ray r;
};

class Grid
{
public:
	Grid() {}
	Grid(Bounding_Box* b, int _nx, int _ny, int _nz) {
		nx = _nx;
		ny = _ny;
		nz = _nz;
		occ = new bool** [_nx];
		objs = new Object3DVector * *[_nx];
		int i, j, k;
		for (i = 0; i < _nx; i++) {
			occ[i] = new bool* [_ny];
			objs[i] = new Object3DVector * [_ny];
			for (j = 0; j < _ny; j++) {
				occ[i][j] = new bool[_nz];
				objs[i][j] = new Object3DVector[_nz];
				for (k = 0; k < _nz; k++) {
					occ[i][j][k] = 0;
					//	objs[i][j][k];
				}
			}
		}
		bb = b;
		glm::vec3 v0, v1;
		b->get(v0, v1);
		lx = 1.0 * (v1.x - v0.x) / _nx;
		ly = 1.0 * (v1.y - v0.y) / _ny;
		lz = 1.0 * (v1.z - v0.z) / _nz;
	}
	~Grid() {
		int i, j, k;
		for (i = 0; i < nx; i++) {
			for (j = 0; j < ny; j++) {
				delete[]occ[i][j];
				delete[]objs[i][j];
			}
			delete[]occ[i];
			delete[]objs[i];
		}
		delete[]occ;
		delete[]objs;/**/
	}
	virtual Bounding_Box* getBoundingBox() { return bb; }
	virtual bool intersect_2(const Ray& r, Hit& h, float tmin) {
		int i, j, k;
		Material* m;
		glm::vec3 diffuseColor;
		diffuseColor = glm::vec3(1, 1, 1);
		glm::vec3 specularColor(0, 0, 0);
		float exponent = 1;
		glm::vec3 reflectiveColor(0, 0, 0);
		glm::vec3 transparentColor(0, 0, 0);
		float indexOfRefraction = 1;
		//m = new PhongMaterial(diffuseColor, specularColor, exponent, reflectiveColor, transparentColor, indexOfRefraction);

		MarchingInfo mi;
		initializeRayMarch(mi, r, tmin);
		if (mi.GetTmin() == INFINITY) {
			int obnum = inf_obj.getNumObjects();
			int state = 0;
			for (k = 0; k < obnum; k++) {
				if (inf_obj.getObject(k)->intersect(r, h, tmin)) state = 1;
			}
			return state;
		}
		int indx, indy, indz;
		mi.GetIndice(indx, indy, indz);
		while (indx >= 0 && indx < nx && indy >= 0 && indy < ny && indz >= 0 && indz < nz) {

			if (occ[indx][indy][indz] == 1) {
				int obnum = objs[indx][indy][indz].getNumObjects();
				int state = 0;
				for (k = 0; k < obnum; k++) {
					if (objs[indx][indy][indz].getObject(k) == NULL) continue;
					if (objs[indx][indy][indz].getObject(k)->intersect(r, h, tmin)) state = 1;
				}
				if (state) return 1;
			}
			mi.nextCell();
			mi.GetIndice(indx, indy, indz);

		}
		int obnum = inf_obj.getNumObjects();
		int state = 0;
		for (k = 0; k < obnum; k++) {
			if (inf_obj.getObject(k)->intersect(r, h, tmin)) state = 1;
		}
		return state;
	}

	void initializeRayMarch(MarchingInfo& mi, const Ray& r, float tmin) const {
		bool miss = 0;
		glm::vec3 ori, dir;
		ori = r.getOrigin();
		dir = r.getDirection();
		float sx = dir.x, sy = dir.y, sz = dir.z;
		int _sx, _sy, _sz;
		if (sx < 0)_sx = -1;
		else _sx = 1;//==0时值为1
		if (sy < 0)_sy = -1;
		else _sy = 1;//==0时值为1
		if (sz < 0)_sz = -1;
		else _sz = 1;//==0时值为1
		mi.SetSign(_sx, _sy, _sz);
		mi.SetRay(r);
		mi.SetIncrement(lx, ly, lz);

		glm::vec3 v0, v1;
		bb->get(v0, v1);
		mi.SetV(v0, v1);
		glm::vec3 x(lx, 0, 0), y(0, ly, 0), z(0, 0, lz), a0(lx / 2, ly / 2, lz / 2);
		ori = ori + dir * tmin;
		float detx = (ori - v0).x;
		float dety = (ori - v0).y;
		float detz = (ori - v0).z;
		int indx, indy, indz;
		indx = detx / lx;
		indy = dety / ly;
		indz = detz / lz;
		if ((indx >= 0 && indx < nx) && (indy >= 0 && indy < ny) && (indz >= 0 && indz < nz)) {//原点在盒子里
			v0 = v0 + x * (float)indx + y * (float)indy + z * (float)indz;
			v1 = v0 + x + y + z;
			ori = r.getOrigin();
			float t[7];
			if (dir.x * _sx > 0) {
				t[1] = (v0.x - ori.x) / dir.x;
				t[4] = (v1.x - ori.x) / dir.x;
			}
			else {
				t[1] = INFINITY; t[4] = INFINITY;
			}
			if (dir.y * _sy > 0) {
				t[2] = (v0.y - ori.y) / dir.y;
				t[5] = (v1.y - ori.y) / dir.y;
			}
			else {
				t[2] = INFINITY; t[5] = INFINITY;
			}
			if (dir.z * _sz > 0) {
				t[3] = (v0.z - ori.z) / dir.z;
				t[6] = (v1.z - ori.z) / dir.z;
			}
			else {
				t[3] = INFINITY; t[6] = INFINITY;
			}

			float tnx, tny, tnz;
			if (_sx == 1) tnx = t[4];
			else tnx = t[1];
			if (_sy == 1) tny = t[5];
			else tny = t[2];
			if (_sz == 1) tnz = t[6];
			else tnz = t[3];
			mi.SetNextT(tnx, tny, tnz);

			mi.SetIndice(indx, indy, indz);
			mi.SetTmin(tmin);
			glm::vec3 fn(0, 0, 0);
			mi.SetNormal(fn);
		}
		else {
			ori = r.getOrigin();
			float t[7];
			if (dir.x * _sx > 0) {
				t[1] = (v0.x - ori.x) / dir.x;
				t[4] = (v1.x - ori.x) / dir.x;
			}
			else {
				t[1] = INFINITY; t[4] = INFINITY;
			}
			if (dir.y * _sy > 0) {
				t[2] = (v0.y - ori.y) / dir.y;
				t[5] = (v1.y - ori.y) / dir.y;
			}
			else {
				t[2] = INFINITY; t[5] = INFINITY;
			}
			if (dir.z * _sz > 0) {
				t[3] = (v0.z - ori.z) / dir.z;
				t[6] = (v1.z - ori.z) / dir.z;
			}
			else {
				t[3] = INFINITY; t[6] = INFINITY;
			}
			glm::vec3 p[7];
			int i;
			for (i = 1; i <= 6; i++) {
				p[i] = r.pointAtParameter(t[i]);
			}
			bool pbox[7];
			if (p[1].y >= v0.y && p[1].y <= v1.y && p[1].z >= v0.z && p[1].z <= v1.z) pbox[1] = 1;
			else pbox[1] = 0;
			if (p[2].x >= v0.x && p[2].x <= v1.x && p[2].z >= v0.z && p[2].z <= v1.z) pbox[2] = 1;
			else pbox[2] = 0;
			if (p[3].x >= v0.x && p[3].x <= v1.x && p[3].y >= v0.y && p[3].y <= v1.y) pbox[3] = 1;
			else pbox[3] = 0;
			if (p[4].y >= v0.y && p[4].y <= v1.y && p[4].z >= v0.z && p[4].z <= v1.z) pbox[4] = 1;
			else pbox[4] = 0;
			if (p[5].x >= v0.x && p[5].x <= v1.x && p[5].z >= v0.z && p[5].z <= v1.z) pbox[5] = 1;
			else pbox[5] = 0;
			if (p[6].x >= v0.x && p[6].x <= v1.x && p[6].y >= v0.y && p[6].y <= v1.y) pbox[6] = 1;
			else pbox[6] = 0;

			float tf;
			glm::vec3 pf;
			int fi;//判断从哪个面进入
			tf = INFINITY;
			for (i = 1; i <= 6; i++) {
				if (pbox[i] == 1) {
					if (t[i] < tf && t[i] >= tmin) {
						tf = t[i];
						pf = p[i];
						fi = i;
					}
				}
			}
			if (tf == INFINITY) {
				miss = 1;
				mi.SetTmin(tf);
			}
			else {
				miss = 0;
				float detx = (pf - v0).x;
				float dety = (pf - v0).y;
				float detz = (pf - v0).z;
				int indx, indy, indz;
				indx = detx / lx;
				indy = dety / ly;
				indz = detz / lz;
				if (posive(pf.x - v1.x) < epsilon2) indx = nx - 1;
				if (posive(pf.y - v1.y) < epsilon2) indy = ny - 1;
				if (posive(pf.z - v1.z) < epsilon2) indz = nz - 1;
				mi.SetIndice(indx, indy, indz);
				mi.SetTmin(tf);
				/*	glm::vec3 p;
					p = v0 + a0 + x * indx + y * indy + z * indz;
					float tnx, tny, tnz;
					if (sx != 0) tnx = sy / sx * (p.x - ori.x) + ori.y - p.y;//这里的ori带tmin，但是不影响
					else tnx = INFINITY;
					if (sy != 0) tny = sz / sy * (p.y - ori.y) + ori.z - p.z;
					else tny = INFINITY;
					if (sz != 0) tnz = sx / sz * (p.z - ori.z) + ori.x - p.x;
					else tnz = INFINITY;
					mi.SetNextT(tnx, tny, tnz);//算出e的初始值*/
				v0 = v0 + x * (float)indx + y * (float)indy + z * (float)indz;
				v1 = v0 + x + y + z;
				ori = r.getOrigin();
				float t[7];
				if (dir.x * _sx > 0) {
					t[1] = (v0.x - ori.x) / dir.x;
					t[4] = (v1.x - ori.x) / dir.x;
				}
				else {
					t[1] = INFINITY; t[4] = INFINITY;
				}
				if (dir.y * _sy > 0) {
					t[2] = (v0.y - ori.y) / dir.y;
					t[5] = (v1.y - ori.y) / dir.y;
				}
				else {
					t[2] = INFINITY; t[5] = INFINITY;
				}
				if (dir.z * _sz > 0) {
					t[3] = (v0.z - ori.z) / dir.z;
					t[6] = (v1.z - ori.z) / dir.z;
				}
				else {
					t[3] = INFINITY; t[6] = INFINITY;
				}

				float tnx, tny, tnz;
				if (_sx == 1) tnx = t[4];
				else tnx = t[1];
				if (_sy == 1) tny = t[5];
				else tny = t[2];
				if (_sz == 1) tnz = t[6];
				else tnz = t[3];
				mi.SetNextT(tnx, tny, tnz);
				glm::vec3 fn;
				switch (fi) {
				case 1: fn = glm::vec3(-1, 0, 0); break;
				case 2: fn = glm::vec3(0, -1, 0); break;
				case 3: fn = glm::vec3(0, 0, -1); break;
				case 4: fn = glm::vec3(1, 0, 0); break;
				case 5: fn = glm::vec3(0, 1, 0); break;
				case 6: fn = glm::vec3(0, 0, 1); break;
				default: fn = glm::vec3(0, 0, 0); break;
				}
				mi.SetNormal(fn);
			}
		}
	}
	Bounding_Box* bb;
	bool*** occ;
	Object3DVector*** objs;
	Object3DVector inf_obj;
	float lx;
	float ly;
	float lz;
	int nx;
	int ny;
	int nz;
};
