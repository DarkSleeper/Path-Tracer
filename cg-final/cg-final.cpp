#include <iostream>
#include "my_obj_loader.hpp"
#include "scene/material.hpp"
#include "scene/triangle.hpp"

int main()
{
    std::vector<float> vertices; 
    std::vector<float> normals; 
    std::vector<float> texcoord;
    std::vector<int> mat_idx; 
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    std::string filename = "runtime/scene/cornell-box/cornell-box.obj";
    std::string basepath = "runtime/scene/cornell-box/";
    my_load_obj(filename, basepath, vertices, normals, texcoord, mat_idx, materials);

    std::cout << "Hello World!\n";
}
