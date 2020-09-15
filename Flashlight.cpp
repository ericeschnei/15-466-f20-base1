#include "Flashlight.hpp"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <utility>
#include "GL.hpp"
#include "gl_compile_program.hpp"
#include "glm/gtc/type_ptr.hpp"

Flashlight::Flashlight() {

	// this is from
	// https://learnopengl.com/Getting-started/Hello-Triangle
	// with help from gl_compile_program.hpp
	program = gl_compile_program(
		// vertex shader
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"in vec4 pos;\n"
		"void main() {\n"
		"  gl_Position = OBJECT_TO_CLIP * pos;\n"
		"}\n",

		// fragment shader (solid white)
		"#version 330\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"  fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
		"}\n"
	);

	GLint pos_vec2 = glGetAttribLocation(program, "pos");	
	otc_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(
			pos_vec2, //index
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(float) * 2, //stride
			(GLbyte *)0 //offset
	);
	glEnableVertexAttribArray(pos_vec2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// oh my god so much of this is from
	// https://learnopengl.com/Advanced-OpenGL/Framebuffers
	// i have no idea how to opengl
	// (okay, well i have *some* idea, but it's still a bit foggy)
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			GL_R8, 
			lightmap_width, 
			lightmap_height, 
			0, 
			GL_RED, 
			GL_UNSIGNED_BYTE, 
			nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error! Framebuffer not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_ERRORS(); // PARANOIA!! (the rpg! from the eighties)
};
Flashlight::~Flashlight() {
	// remove opengl stuff
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &tex);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
};

inline bool is_wall(uint8_t w) {
	return w == 7;
}

glm::vec2 Flashlight::cast_ray(
		const glm::vec2            &player,
		const glm::vec2            &direction,
		const glm::ivec2           &lower_left,
		const std::vector<uint8_t> &map,
		size_t                     map_width) {

	// http://www.cse.yorku.ca/~amana/research/grid.pdf

	glm::vec2 from = player / 8.0f;

	int x = (int)glm::floor(from.x);
	int y = (int)glm::floor(from.y);

	int step_x = (int)glm::sign(direction.x);
	int step_y = (int)glm::sign(direction.y);

	double t_delta_x = 1.0f/glm::abs(direction.x);
	double t_delta_y = 1.0f/glm::abs(direction.y);

	double t_max_x;
	if (step_x < 0) {
		t_max_x = glm::fract(glm::abs(from.x)) * t_delta_x;
	} else {
		t_max_x = (1.0f - glm::fract(glm::abs(from.x))) * t_delta_x;
	}

	double t_max_y;
	if (step_y < 0) {
		t_max_y = glm::fract(glm::abs(from.y)) * t_delta_y;
	} else {
		t_max_y = (1.0f - glm::fract(glm::abs(from.y))) * t_delta_y;
	}

	bool last_incremented_x = true;

	while (true) {

		if (x < 0 || x >= (int)(lightmap_width /8)  ||
			y < 0 || y >= (int)(lightmap_height/8) ||
			is_wall(map[(y + lower_left.y) * map_width + (x + lower_left.x)])
		) {
			t_max_y -= t_delta_y;
			t_max_x -= t_delta_x;
			
			glm::vec2 ret = from + direction * (float)(last_incremented_x ? t_max_x : t_max_y);
			if (last_incremented_x) {
				ret.x = glm::round(ret.x);
			} else {
				ret.y = glm::round(ret.y);
			}

			return glm::clamp(
				8.0f * ret,
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
		glm::vec2 player,
		const glm::ivec2 &lower_left,
		const std::vector<uint8_t> &map,
		size_t map_width,
		size_t map_height) {

	// really, TRULY awful fix for a stupid bug.
	if (glm::fract(player.x) >= 0.99f) player.x -= 0.05f;
	if (glm::fract(player.y) >= 0.99f) player.y -= 0.05f;
	if (glm::fract(player.x) <= 0.01f) player.x += 0.05f;
	if (glm::fract(player.y) <= 0.01f) player.y += 0.05f;


	constexpr float EPSILON = 0.001f;
	const float cos_eps = glm::cos(EPSILON);
	const float sin_eps = glm::sin(EPSILON);

	// rotation matrix, +0.0001 rad
	const glm::mat2x2 rot_eps_pos = glm::mat2x2(
		cos_eps, sin_eps,
		-sin_eps, cos_eps
	);

	// rotation matrix, -0.0001 rad
	const glm::mat2x2 rot_eps_neg = glm::mat2x2(
		cos_eps, -sin_eps,
		sin_eps, cos_eps
	);

	(void) rot_eps_pos;
	(void) rot_eps_neg;


	typedef std::pair<float, glm::vec2> pq_elem_t;
	const auto pq_lt = [](pq_elem_t a, pq_elem_t b){
		if (a.first == b.first) {
			return glm::length(a.second) > glm::length(b.second);
		}
		return a.first > b.first;
	};
	
	std::priority_queue<pq_elem_t, std::vector<pq_elem_t>, decltype(pq_lt)> pq(pq_lt);	

	auto add_pq = [&pq](const glm::vec2 &elem) {
		
		float angle = glm::atan(elem.y, elem.x);

		pq.emplace(angle, elem);
	}; 


	// lambda to index into the tilemap, dealing with OOB
	// by treating them like walls
	auto get_tilepos = [map, lower_left, map_width, map_height](int x_, int y_){
		
		if (x_ < 0 || x_ >= (int)lightmap_width/8) {
			return true;
		} else if (y_ < 0 || y_ >= (int)lightmap_height/8) {
			return true;
		}

		x_ += lower_left.x;
		y_ += lower_left.y;
		
		if (x_ < 0 || x_ >= (int)map_width) {
			return true;
		}
		else if (y_ < 0 || y_ >= (int)map_height) {
			return true;
		}
		else {
			return is_wall(map[y_ * map_width + x_]);
		}
	};

	// find corners
	for (int y = 0; y <= (int)(lightmap_height/8); y++) {
		for (int x = 0; x <= (int)(lightmap_width/8); x++) {

			bool tl = get_tilepos(x-1, y  );
			bool tr = get_tilepos(x  , y  );
			bool bl = get_tilepos(x-1, y-1);
			bool br = get_tilepos(x  , y-1);

			bool is_corner = !(
					(!tl && !tr && !bl && !br) ||
					( tl &&  tr &&  bl &&  br) ||
					(!tl && !tr &&  bl &&  br) ||
					( tl &&  tr && !bl && !br) ||
					( tl && !tr &&  bl && !br) ||
					(!tl &&  tr && !bl &&  br)
			);

			glm::vec2 corner = glm::vec2(x, y) * 8.0f;
			if (is_corner) {
				glm::vec2 player_to_corner = glm::normalize(corner - player);
				add_pq(player_to_corner);
				add_pq(rot_eps_neg * player_to_corner);
				add_pq(rot_eps_pos * player_to_corner);
			}

		}
	}

	// generate points
	std::vector<glm::vec2> verts;

	verts.emplace_back(player);

	glm::vec2 end = pq.top().second;

	while (pq.size() > 0) {
		glm::vec2 dir = pq.top().second;
		pq.pop();

		verts.push_back(cast_ray(player, dir, lower_left, map, map_width));
	}

	verts.push_back(cast_ray(player, end, lower_left, map, map_width));
	// actually update lightmap with OpenGL

	// from Jim McCann, PPU466.cpp
	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);

	// https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/5.1.framebuffers/framebuffers.cpp
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, lightmap_width, lightmap_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// MOST OF THE FOLLOWING CODE IS TAKEN FROM PPU466.CPP
	// ALL CREDIT TO JIM MCCANN :)

	//upload vertex data:
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * verts.size(), verts.data(), GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	//set up the pipeline:
	// set blending function for output fragments:
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set the shader programs:
	glUseProgram(program);

	// configure attribute streams:
	glBindVertexArray(vao);

	// set uniforms for shader programs:
	{ //set matrix to transform [0,ScreenWidth]x[0,ScreenHeight] -> [-1,1]x[-1,1]:
		//NOTE: glm uses column-major matrices:
		glm::mat4 OBJECT_TO_CLIP = glm::mat4(
			glm::vec4(2.0f / lightmap_width, 0.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 2.0f / lightmap_height, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec4(-1.0f,-1.0f, 0.0f, 1.0f)
		);
		glUniformMatrix4fv(otc_mat4, 1, GL_FALSE, glm::value_ptr(OBJECT_TO_CLIP));
	}

	//now that the pipeline is configured, trigger drawing of triangle strip:
	glDrawArrays(GL_TRIANGLE_FAN, 0, GLsizei(verts.size()));

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);	

	for (size_t i = 0; i < lightmap.size(); i++) {
		lightmap[i] = 200;
	}
	glBindTexture(GL_TEXTURE_2D, tex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, lightmap.data());
	
	glBindTexture(GL_TEXTURE_2D, 0);

	GL_ERRORS();	

}

void Flashlight::graft(
		const glm::ivec2 &              lower_left,
		const std::vector<uint8_t> &    map,
		size_t                          map_width,
		size_t                          map_height,
		const std::array<PPU466::Tile, 256> &tiles,
		std::array<uint8_t, lightmap_width*lightmap_height/64> &newmap,
		std::vector<PPU466::Tile> &     dyntiles) {

	dyntiles.clear();

	// https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
	auto hash_tile = [](PPU466::Tile t){
		size_t hash = 0;
		for (size_t i = 0; i < 8; i++) {
			hash ^= std::hash<uint8_t>{}(t.bit0[i]);
			hash ^= std::hash<uint8_t>{}(t.bit1[i]);
		}
		return hash;
	};

	auto tile_equal = [](PPU466::Tile a, PPU466::Tile b){
		return (a.bit0 == b.bit0) && (a.bit1 == b.bit1);
	};

	// hash table mapping tiles to indices
	std::unordered_map<PPU466::Tile, uint8_t, decltype(hash_tile), decltype(tile_equal)> newtiles(20, hash_tile, tile_equal);

	for (size_t y = 0; y < lightmap_height/8; y++) {
		for (size_t x = 0; x < lightmap_width/8; x++) {
			size_t newmap_index = y * lightmap_width/8 + x;

			int x_tilepos = lower_left.x + (int)x;
			int y_tilepos = lower_left.y + (int)y;

			if (x_tilepos < 0 || x_tilepos >= (int)map_width) {
				newmap[newmap_index] = (uint8_t)7;
				continue;
			} else if (y_tilepos < 0 || y_tilepos >= (int)map_height) {
				newmap[newmap_index] = (uint8_t)7;
				continue;
			}

			uint8_t tile_index = map[y_tilepos * map_width + x_tilepos];
			PPU466::Tile tile = tiles[tile_index];

			bool is_all_ones = true;
			bool is_all_zeros = true;
			for (size_t yy = 0; yy < 8; yy++) {
				uint8_t light_row = 0;
				for (int xx = 7; xx >= 0; xx--) {
					light_row <<= 1;
					if (lightmap[(y * 8 + yy) * lightmap_width + (x * 8 + xx)]) {
						light_row += 1;
						is_all_zeros = false;
					} else {
						is_all_ones = false;
					}
				}
				tile.bit0[yy] &= light_row;
				tile.bit1[yy] &= light_row;
			}

			if (is_all_ones) {
				// tilemap all ones, just use static tile
				newmap[newmap_index] = tile_index;
			} else if (is_all_zeros) {
				// tilemap all zeros, just use blank tile
				newmap[newmap_index] = (uint8_t)7;
			} else {
				// see if we've already constructed this tile
				auto search = newtiles.find(tile);
				if (search == newtiles.end()) {
					// we haven't, add it
					if (dyntiles.size() >= 128) {
						std::cout << "Oh no, tile overflow!" << std::endl;
					}
					newtiles[tile] = (uint8_t)(dyntiles.size() + DYNAMIC_TILE_START);
					newmap[newmap_index] = (uint8_t)(dyntiles.size() + DYNAMIC_TILE_START);
					dyntiles.push_back(tile);
				} else {
					// we have, reuse that index
					newmap[newmap_index] = search->second;
				}
			}

		}
	}

}




