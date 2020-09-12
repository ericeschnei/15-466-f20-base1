#include "Flashlight.hpp"
#include <iostream>
Flashlight::Flashlight() {};
Flashlight::~Flashlight() {};

glm::vec2 Flashlight::cast_ray(
		const glm::vec2            &from,
		const glm::vec2            &direction,
		const glm::ivec2           &lower_left,
		const std::vector<uint8_t> &map,
		size_t                     map_width) {

	// http://www.cse.yorku.ca/~amana/research/grid.pdf

	int x = (int)glm::floor(from.x);
	int y = (int)glm::floor(from.y);

	int step_x = glm::sign(direction.x);
	int step_y = glm::sign(direction.y);

	float t_delta_x = 1.0f/glm::abs(direction.x);
	float t_delta_y = 1.0f/glm::abs(direction.y);

	float t_max_x;
	if (step_x < 0) {
		t_max_x = glm::fract(glm::abs(from.x)) * t_delta_x;
	} else {
		t_max_x = (1.0f - glm::fract(glm::abs(from.x))) * t_delta_x;
	}

	float t_max_y;
	if (step_y < 0) {
		t_max_y = glm::fract(glm::abs(from.y)) * t_delta_y;
	} else {
		t_max_y = (1.0f - glm::fract(glm::abs(from.y))) * t_delta_y;
	}

	bool last_incremented_x = true;

	while (true) {

		if (x < 0 || x >= (int) lightmap_width  ||
			y < 0 || y >= (int) lightmap_height ||
			map[(y + lower_left.y) * map_width + (x + lower_left.x)]
		) {
			t_max_y -= t_delta_y;
			t_max_x -= t_delta_x;
			
			glm::vec2 ret = from + direction * (last_incremented_x ? t_max_x : t_max_y);
			if (last_incremented_x) {
				ret.x = glm::round(ret.x);
			} else {
				ret.y = glm::round(ret.y);
			}

			return glm::clamp(
				ret,
				glm::vec2(0, 0), 
				glm::vec2(lightmap_width, lightmap_height)
			);

		}
		// if t_delta is too high, avoid infinite loops
		// by just pretending it's an axis-aligned line
		if (t_delta_x > 1000.0f) {
			t_max_y += t_delta_y;
			y += step_y;
			last_incremented_x = false;
		} else if (t_delta_y > 1000.0f) {
			t_max_x += t_delta_x;
			x += step_x;
			last_incremented_x = true;
		}

		else if (t_max_x < t_max_y) {
			t_max_x += t_delta_x;
			x += step_x;
			last_incremented_x = true;
		} else {
			t_max_y += t_delta_y;
			y += step_y;
			last_incremented_x = false;
		}

	}

}

void Flashlight::update_lightmap(
		const glm::vec2 &player,
		const glm::vec2 &mouse,
		const glm::ivec2 &lower_left,
		const std::vector<uint8_t> &map,
		size_t map_width) {



}
