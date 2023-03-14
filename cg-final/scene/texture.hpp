#pragma once

#include "../tool/stb_image.h"
#include <glm/glm.hpp>
#include <iostream>

struct Texture
{
	Texture(std::string path) {
		data = stbi_load(path.data(), &width, &height, &nrComponents, 0);
		//if (nrComponents == 3) std::cout << "rgb!" << std::endl;
	}
	~Texture() {
		stbi_image_free(data);
	}

	glm::vec3 get_color(glm::vec2 uv) {
		auto fit = [](float uv) -> float {
			if (uv < 0) {
				uv += -1 * std::floorf(uv);
			}
			return uv - std::floorf(uv);
		};
		auto u = fit(uv.x);
		auto v = fit(uv.y);
		int i = width * u;
		int j = height * v;
		auto color = glm::vec3(data[nrComponents * width * j + nrComponents * i + 0],
							   data[nrComponents * width * j + nrComponents * i + 1],
							   data[nrComponents * width * j + nrComponents * i + 2]);
		return color;
	}

private:
	int width, height, nrComponents;
	unsigned char* data;
};
