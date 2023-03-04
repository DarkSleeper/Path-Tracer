#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "material.hpp"
#include "triangle.hpp"

struct Scene_Parser
{
    Scene_Parser()
    {
        bg_color = glm::vec3(0.f, 0.f, 0.f);
        bg_mat = new Background_Material();
        bg_mat->diffuse = bg_color;
    }

    auto getAmbientLight() -> glm::vec3 {
        // todo
        return glm::vec3(0.1, 0.1, 0.1);
    }
    auto getNumLights() -> int {
        return light_triangles.size();
    }
    auto getIllumination(int idx, const glm::vec3 &p, glm::vec3&dir, glm::vec3&col, float &distanceToLight) const -> void
    {

    }

    bool group_intersect(const Ray& r, Hit& h, float tmin) {
        int i, state = 0;
        for (auto& tri: triangles) {
            if (tri.intersect(r, h, tmin)) state = 1;
        }
        return state;
    }

    std::vector<Triangle> triangles;
    std::vector<Material*> materials;
    std::vector<Triangle> light_triangles;
    std::vector<Material*> light_materials;
    Background_Material* bg_mat;
    glm::vec3 bg_color;
};