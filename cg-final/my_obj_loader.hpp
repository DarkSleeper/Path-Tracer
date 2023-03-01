#pragma once
#include <iostream>
#include "scene/triangle.hpp"
#include "scene/material.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tool/tiny_obj_loader.h"

void my_load_obj(std::string const& filename, std::string const& basepath, std::vector<Triangle>& triangles, std::vector<Material*>& materials) {
	auto load_model = [](tinyobj::attrib_t &attrib, std::vector<tinyobj::shape_t> &shapes, std::vector<tinyobj::material_t> &materials,
						 const char* filename, const char* basepath = NULL, bool triangulate = true) -> bool {
		std::string warn;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);
		if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
		if (!err.empty()) std::cerr << "ERR: " << err << std::endl;
		if (!ret) {
			printf("Failed to load/parse .obj.\n");
			return false;
		}
		return true;
	};
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::material_t> inner_materials;
	std::vector<tinyobj::shape_t> shapes;
	bool triangulate = true;
	load_model(attrib, shapes, inner_materials, filename.data(), basepath.data(), triangulate);

	materials.clear();
	for (auto& inner_mat: inner_materials) {
		Phong_Material* mat = new Phong_Material();
		mat->name = inner_mat.name;
		mat->diffuse = glm::vec3(inner_mat.diffuse[0], inner_mat.diffuse[1], inner_mat.diffuse[2]);
		mat->specular = glm::vec3(inner_mat.specular[0], inner_mat.specular[1], inner_mat.specular[2]);
		mat->transmittance = glm::vec3(inner_mat.transmittance[0], inner_mat.transmittance[1], inner_mat.transmittance[2]);
		mat->shininess = inner_mat.shininess;
		mat->ior = inner_mat.ior;
		materials.push_back(mat);
	}

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++) {
		size_t index_offset = 0;

		// For each face
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];
			if (fnum != 3) printf("  face[%ld] is not triangle!\n", static_cast<long>(f));

			Triangle tri;
			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				//indices.push_back(idx.vertex_index);
				tri.position[v] = glm::vec3(attrib.vertices[3 * idx.vertex_index + 0],
											attrib.vertices[3 * idx.vertex_index + 1],
											attrib.vertices[3 * idx.vertex_index + 2]);
				tri.normal[v]   = glm::vec3(attrib.normals[3 * idx.normal_index + 0],
											attrib.normals[3 * idx.normal_index + 1],
											attrib.normals[3 * idx.normal_index + 2]);
				tri.texcoord[v] = glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0],
											attrib.texcoords[2 * idx.texcoord_index + 1]);
			}
			tri.mat = materials[shapes[i].mesh.material_ids[f]];
			triangles.push_back(tri);

			index_offset += fnum;
		}
	}
}