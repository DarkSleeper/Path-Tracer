#pragma once
#include <iostream>
#include <vector>
#include "tool/tiny_xml/tinyxml.h"
#include "scene/material.hpp"
#include "scene/camera.hpp"

void my_load_scene(std::string const& filename, std::vector<Material*>& light_materials, Camera_Config& camera)
{
	TiXmlDocument file;
	if (!file.LoadFile(filename.data())) {
		std::cout << "xml load failed: " << filename << std::endl;
		return;
	}
	auto root = file.RootElement();
	auto container = root;
	while (container != nullptr) {
		if (container->ValueTStr() == "camera") 
		{
			camera.width = atoi(container->Attribute("width"));
			camera.height = atoi(container->Attribute("height"));
			camera.fovy = atof(container->Attribute("fovy"));
			auto child = container->FirstChildElement();
			while (child != nullptr) {
				if (child->ValueTStr() == "eye") {
					camera.eye.x = atof(child->Attribute("x"));
					camera.eye.y = atof(child->Attribute("y"));
					camera.eye.z = atof(child->Attribute("z"));
				}
				else if (child->ValueTStr() == "lookat") {
					camera.lookat.x = atof(child->Attribute("x"));
					camera.lookat.y = atof(child->Attribute("y"));
					camera.lookat.z = atof(child->Attribute("z"));
				}
				else if (child->ValueTStr() == "up") {
					camera.up.x = atof(child->Attribute("x"));
					camera.up.y = atof(child->Attribute("y"));
					camera.up.z = atof(child->Attribute("z"));
				}
				child = child->NextSiblingElement();
			}
		} 
		else if (container ->ValueTStr()== "light") 
		{
			auto mat_name = container->Attribute("mtlname");
			Light_Material* mat = new Light_Material();
			mat->name = mat_name;
			mat->is_light = true;
			std::string rad_str = container->Attribute("radiance");
			auto x_pos = rad_str.find_first_of(',');
			mat->radiance.x = atof(rad_str.substr(0, x_pos).data());
			rad_str = rad_str.substr(x_pos + 1);
			auto y_pos = rad_str.find_first_of(',');
			mat->radiance.y = atof(rad_str.substr(0, y_pos).data());
			mat->radiance.z = atof(rad_str.substr(y_pos + 1).data());
			light_materials.push_back(mat);
		}
		container = container->NextSiblingElement();
	}
}