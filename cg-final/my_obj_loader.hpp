#pragma once
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tool/tiny_obj_loader.h"

void my_load_obj(std::string const& filename, std::string const& basepath, 
					std::vector<float>& vertices, std::vector<float>& normals, std::vector<float>& texcoord,
					std::vector<int>& mat_idx, std::vector<tinyobj::material_t>& materials) {
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
	std::vector<tinyobj::shape_t> shapes;
	bool triangulate = true;
	load_model(attrib, shapes, materials, filename.data(), basepath.data(), triangulate);

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++) {
		size_t index_offset = 0;

		// For each face
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++) {
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				//indices.push_back(idx.vertex_index);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
				vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
				normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
				normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
				normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
				texcoord.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
				texcoord.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
			}
			mat_idx.push_back(shapes[i].mesh.material_ids[f]);

			if (fnum != 3) printf("  face[%ld] is not triangle!\n", static_cast<long>(f));

			index_offset += fnum;
		}
	}
}