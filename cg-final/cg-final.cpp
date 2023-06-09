﻿#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "my_obj_loader.hpp"
#include "my_scene_loader.hpp"
#include "scene/material.hpp"
#include "scene/triangle.hpp"
#include "scene/camera.hpp"
#include "scene/scene_parser.hpp"
#include "render/ray_tracer.hpp"
#include "tool/svpng.h"

#define MAXnum 100000

int main(int argc, char* argv[]) {
    std::string model_name = "cornell-box-simple";
    int sample_num = 3;
    if (argc == 2) {
        model_name = argv[1];
    }
    if (argc == 3) {
        model_name = argv[1];
        sample_num = atoi(argv[2]);
    }
    if (sample_num <= 0) {
        std::cout << "invalid sample num!" << std::endl;
    }
    int max_bounce = 20;
    bool shadows = true;
    bool shade_back = false;

    Scene_Parser scene;
    auto& triangles = scene.triangles;
    auto& materials = scene.materials;
    auto& light_triangles = scene.light_triangles;
    auto& light_materials = scene.light_materials;

    // load scene
    Camera_Config camera_config;
    std::string xml_filename = "runtime/scene/" + model_name + "/" + model_name + ".xml";
    my_load_scene(xml_filename, light_materials, camera_config);
    std::string filename = "runtime/scene/" + model_name + "/" + model_name + ".obj";
    std::string basepath = "runtime/scene/" + model_name + "/";
    my_load_obj(filename, basepath, triangles, light_triangles, materials, light_materials);

    auto camera = Perspective_Camera(camera_config);
    auto width = camera_config.width;
    auto height = camera_config.height;
    std::vector<unsigned char> out_data(width * height * 3);

    auto bg_mat = scene.bg_mat;
    scene.init_bounding_box_and_grid(20, 20, 20);
    scene.init_light_weight();
    float cutoff_weight = 0.01;
    Ray_Tracer ray_tracer(&scene, max_bounce, cutoff_weight, shadows, shade_back);

    float tmin = camera.get_t_min();
    glm::vec3 n0(0, 0, 0);

    auto sample_offset = glm::vec2(1.f / sample_num, 1.f / sample_num);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
            // todo: filter & sample

			auto pixel = glm::vec2(i, j);
			auto position = pixel;

            auto color = glm::vec3(0.f);
            for (int offset_i = 0; offset_i < sample_num; offset_i++) {
                for (int offset_j = 0; offset_j < sample_num; offset_j++) {
                    auto sample_pos = position + glm::vec2(offset_i * sample_offset.x, offset_j * sample_offset.y);

			        auto r = camera.generate_ray(position);
                    auto h = Hit();
                    h.set(MAXnum, bg_mat, n0, r);
			        // todo:
                    color += (1.f / sample_num / sample_num) * ray_tracer.traceRay(r, tmin, 0, 1, 1, h);
                }
            }

            color = glm::clamp(color, glm::vec3(0.f), glm::vec3(1.f));
            auto to_srgb = [](float linear) -> float {
                float s;
                if (linear <= 0.0031308) {
                    s = linear * 12.92;
                } else {
                    s = 1.055 * powf(linear, 1.0 / 2.4) - 0.055;
                }
                return s;
            };
            out_data[3 * (width * (height - 1 - j) + i) + 0] = (unsigned char)((int)(255 * to_srgb(color.r)));
            out_data[3 * (width * (height - 1 - j) + i) + 1] = (unsigned char)((int)(255 * to_srgb(color.g)));
            out_data[3 * (width * (height - 1 - j) + i) + 2] = (unsigned char)((int)(255 * to_srgb(color.b)));
            
		}
	}


    // output to show result
    FILE* fp = fopen("runtime/output/rgba.png", "wb");
    svpng(fp, width, height, &(out_data[0]), 0);
    fclose(fp);


    std::cout << "Hello World!\n";
}
