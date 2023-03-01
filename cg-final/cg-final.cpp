#include <iostream>
#include "my_obj_loader.hpp"
#include "my_scene_loader.hpp"
#include "scene/material.hpp"
#include "scene/triangle.hpp"
#include "scene/camera.hpp"

int main()
{
    Camera_Config camera;
    std::vector<Triangle> triangles;
    std::vector<Material *> materials;
    std::string filename = "runtime/scene/cornell-box/cornell-box.obj";
    std::string basepath = "runtime/scene/cornell-box/";
    my_load_obj(filename, basepath, triangles, materials);
    std::string xml_filename = "runtime/scene/cornell-box/cornell-box.xml";
    my_load_scene(xml_filename, materials, camera);

    std::cout << "Hello World!\n";
}
