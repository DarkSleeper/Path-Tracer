#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "material.hpp"
#include "triangle.hpp"
#include "grid.hpp"
#include "bounding_box.hpp"

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
        auto light_mat = light.mat;
        auto sample_point = rand_sample(light);

        dir = sample_point - p;
        // grab the length before the direction is normalized
        distanceToLight = glm::length(dir);
        dir = glm::normalize(dir);

        col = light_mat->radiance * glm::dot((-1.f) * dir, light.get_normal(sample_point));
    }

    bool group_intersect(const Ray& r, Hit& h, float tmin) {
        int i;
        bool state = false;
        for (auto& tri: triangles) {
            if (tri.intersect(r, h, tmin)) state = true;
        }
        return state;
    }

    bool group_intersect_grid(const Ray& r, Hit& h, float tmin) {
        return grid->intersect_2(r, h, tmin);
    }

    void init_bounding_box_and_grid(int nx, int ny, int nz)
    {
        auto get_traingle_bbox = [] (Triangle const& tri) -> Bounding_Box {
            auto& a = tri.position[0];
            auto& b = tri.position[1];
            auto& c = tri.position[2];
            glm::vec3 _min, _max;
            _min = glm::vec3(min3(a.x, b.x, c.x), min3(a.y, b.y, c.y), min3(a.z, b.z, c.z));
            _max = glm::vec3(max3(a.x, b.x, c.x), max3(a.y, b.y, c.y), max3(a.z, b.z, c.z));
            return Bounding_Box(_min, _max);
        };

        bbox = new Bounding_Box(get_traingle_bbox(triangles[0]));
        for (auto& tri : triangles) {
            bbox->extend(get_traingle_bbox(tri));
        }

        grid = new Grid(bbox, nx, ny, nz);
        for (auto& tri : triangles) {
            tri.insertIntoGrid(grid);
        }
    }

    void init_light_weight() 
    {
        float area_sum = 0.f;
        for (auto& lit : light_triangles) {
            auto lit_a = compute_area(lit);
            light_areas.push_back(lit_a);
            area_sum += lit_a;
        }
        float prefix_sum = 0.f;
        for (auto& lit_a : light_areas) {
            prefix_sum += lit_a;
            light_weight_prefix_sum.push_back(prefix_sum / area_sum);
        }
    }

    auto get_bounding_box() const -> Bounding_Box*
    {
        return bbox;
    }

    Grid* grid;
    std::vector<Triangle> triangles;
    std::vector<Material*> materials;
    std::vector<Triangle> light_triangles;
    std::vector<float> light_areas;
    std::vector<float> light_weight_prefix_sum;
    std::vector<Material*> light_materials;
    Background_Material* bg_mat;
    glm::vec3 bg_color;

private:
    Bounding_Box *bbox;

    auto rand_sample(Triangle const& tri) const -> glm::vec3
    {
        auto t = rand() / float(RAND_MAX);
        auto s = rand() / float(RAND_MAX);
        glm::vec3 res = (1 - sqrtf(t)) * tri.position[0] + (1 - s) * sqrtf(t) * tri.position[1] + s * sqrtf(t) * tri.position[2];
        return res;
    }
    auto compute_area(Triangle const& tri) const -> float
    {
        auto a = glm::distance(tri.position[0], tri.position[1]);
        auto b = glm::distance(tri.position[1], tri.position[2]);
        auto c = glm::distance(tri.position[2], tri.position[0]);
        auto p = (a + b + c) / 2.f;
        auto res = sqrtf(p * (p - a) * (p - b) * (p - c));
        return res;
    }
};