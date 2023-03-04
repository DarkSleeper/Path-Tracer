#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "material.hpp"
#include "triangle.hpp"
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

struct Scene_Parser
{
    Scene_Parser()
    {
        bg_color = glm::vec3(0.f, 0.f, 0.f);
        bg_mat = new Background_Material();
        bg_mat->diffuse = bg_color;

        srand((unsigned int)time(NULL));
    }

    auto getAmbientLight() -> glm::vec3 {
        // todo: erase
        return glm::vec3(0.1, 0.1, 0.1);
    }
    auto getNumLights() -> int {
        return light_triangles.size();
    }
    auto getIllumination(int idx, const glm::vec3 &p, glm::vec3&dir, glm::vec3&col, float &distanceToLight) const -> void
    {
        auto& light = light_triangles[idx];
        auto light_mat = dynamic_cast<Light_Material*>(light.mat);
        auto sample_point = rand_sample(light);

        dir = sample_point - p;
        // grab the length before the direction is normalized
        distanceToLight = glm::length(dir);
        dir = glm::normalize(dir);

        col = light_mat->radiance / powf(distanceToLight * glm::dot(dir, light.get_normal(sample_point)), 2);
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

private:
    auto rand_sample(Triangle const& tri) const -> glm::vec3
    {
        auto t = rand() / float(RAND_MAX);
        auto s = rand() / float(RAND_MAX);
        glm::vec3 res = (1 - sqrtf(t)) * tri.position[0] + (1 - s) * sqrtf(t) * tri.position[1] + s * sqrtf(t) * tri.position[2];
        return res;
    }
};