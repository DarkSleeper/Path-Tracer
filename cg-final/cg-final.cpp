#include <iostream>
#include "my_obj_loader.hpp"
#include "my_scene_loader.hpp"
#include "scene/material.hpp"
#include "scene/triangle.hpp"
#include "scene/camera.hpp"

#define MAXnum 100000

int main()
{
    std::vector<Triangle> triangles;
    std::vector<Material *> materials;
    std::vector<Triangle> light_triangles;
    std::vector<Material *> light_materials;

    // load scene
    Camera_Config camera_config;
    std::string xml_filename = "runtime/scene/cornell-box/cornell-box.xml";
    my_load_scene(xml_filename, light_materials, camera_config);
    std::string filename = "runtime/scene/cornell-box/cornell-box.obj";
    std::string basepath = "runtime/scene/cornell-box/";
    my_load_obj(filename, basepath, triangles, light_triangles, materials, light_materials);

    auto camera = Perspective_Camera(camera_config);
    auto width = camera_config.width;
    auto height = camera_config.height;

    Background_Material *bg_mat = new Background_Material();
    bg_mat->diffuse = glm::vec3(0.f, 0.f, 0.f);

    float tmin = camera.get_t_min();
    glm::vec3 n0(0, 0, 0);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
            // todo: filter & sample

			auto pixel = glm::vec2(1.f * i / width, 1.f * j / width);
			auto position = pixel;
			auto r = camera.generate_ray(position);
            auto h = Hit();
            h.set(MAXnum, bg_mat, n0, r);
			// todo: 
            //color = raytracer->traceRay(r, tmin, 0, 1, 1, h);

		}
	}

    std::cout << "Hello World!\n";
}
