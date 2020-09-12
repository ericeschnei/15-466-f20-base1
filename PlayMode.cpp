#include "PlayMode.hpp"
#include "Flashlight.hpp"
//for the GL_ERRORS() macro:
#include "SDL_events.h"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

//for glm::value_ptr() :
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {

	//TODO
	//Load map from image instead of having it hard-coded.
	map_width = 32;
	map_tiles = {
	//  0 1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 A B C D E F
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
	};
	
	map_height = map_tiles.size() / map_width;

	player_tile_pos = glm::vec2(map_width/2.0f, map_height/2.0f);
	camera_pos = glm::vec2(map_width/2.0f, map_height/2.0f);
	// mark all corners. note that corners on the outside are never checked.
	for (size_t y = 1; y < map_height; y++) {
		for (size_t x = 1; x < map_width; x++) {
			
			// check tile map surrounding the potential corner.
			// the "== 1" is a placeholder here--if more tile types
			// get added, this will become a function to check if a
			// given tile id is a wall.
			bool top_left = map_tiles[(y-1)*map_width+(x-1)] == 1;
			bool top_rght = map_tiles[(y-1)*map_width+(x  )] == 1;
			bool bot_left = map_tiles[(y  )*map_width+(x-1)] == 1;
			bool bot_rght = map_tiles[(y  )*map_width+(x  )] == 1;
			
			// if the tile is part of a wall, continue.
			if ((!top_left && !top_rght && !bot_left && !bot_rght) || // empty
				(!top_left && !top_rght &&  bot_left &&  bot_rght) || // bottom
				( top_left && !top_rght &&  bot_left && !bot_rght) || // left
				( top_left &&  top_rght && !bot_left && !bot_rght) || // top
				(!top_left &&  top_rght && !bot_left &&  bot_rght) || // right
				( top_left &&  top_rght &&  bot_left &&  bot_rght)) { // full
				continue;
			}
			
			// otherwise, we're in a corner!
			map_corners.emplace_back(x, y);
		}
	}
	//TODO:
	// you *must* use an asset pipeline of some sort to generate tiles.
	// don't hardcode them like this!
	// or, at least, if you do hardcode them like this,
	//  make yourself a script that spits out the code that you paste in here
	//   and check that script into your repository.

	//Also, *don't* use these tiles in your game:

	ppu.tile_table[0].bit0 = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000
	};

	ppu.tile_table[0].bit1 = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000
	};

	ppu.tile_table[1].bit0 = {
		0b11111111,
		0b10000001,
		0b10111101,
		0b10111101,
		0b10111101,
		0b10111101,
		0b10000001,
		0b11111111
	};

	ppu.tile_table[1].bit1 = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000
	};

	ppu.tile_table[2].bit0 = {
		0b00000011,
		0b00000001,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
	};

	ppu.tile_table[2].bit1 = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
		0b00000000,
	};

	//use sprite 32 as a "player":
	ppu.tile_table[32].bit0 = {
		0b01111110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
	};
	ppu.tile_table[32].bit1 = {
		0b00000000,
		0b00000000,
		0b00011000,
		0b00100100,
		0b00000000,
		0b00100100,
		0b00000000,
		0b00000000,
	};

	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x80, 0x00, 0x00, 0xff),
		glm::u8vec4(0xff, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	ppu.palette_table[2] = {
		glm::u8vec4(0x00, 0x80, 0x00, 0xff),
		glm::u8vec4(0x00, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00)
	};

	ppu.palette_table[3] = {
		glm::u8vec4(0x00, 0x00, 0x80, 0xff),
		glm::u8vec4(0x00, 0x00, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00)
	};

	//used for the player:
	ppu.palette_table[7] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xff, 0xff, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
	};

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}


void PlayMode::update(float elapsed) {

	constexpr float PlayerSpeed = 3.75f;
	if (left.pressed) player_tile_pos.x -= PlayerSpeed * elapsed;
	if (right.pressed) player_tile_pos.x += PlayerSpeed * elapsed;
	if (down.pressed) player_tile_pos.y -= PlayerSpeed * elapsed;
	if (up.pressed) player_tile_pos.y += PlayerSpeed * elapsed;

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	glm::vec2 camera_dead_radius = glm::vec2(3.0f, 3.0f);

	if (player_tile_pos.x - camera_pos.x > camera_dead_radius.x) {
		camera_pos.x = player_tile_pos.x - camera_dead_radius.x;
	} else if (player_tile_pos.x - camera_pos.x < -camera_dead_radius.x) {
		camera_pos.x = player_tile_pos.x + camera_dead_radius.x;
	}

	if (player_tile_pos.y - camera_pos.y > camera_dead_radius.y) {
		camera_pos.y = player_tile_pos.y - camera_dead_radius.y;
	} else if (player_tile_pos.y - camera_pos.y < -camera_dead_radius.y) {
		camera_pos.y = player_tile_pos.y + camera_dead_radius.y;
	}


}

void PlayMode::draw(glm::uvec2 const &drawable_size) {

	//--- set ppu state based on game state ---


	// first, we regenerate the matrix that takes us from screen space to
	// PPU space (used for mouse handling), since we can really only do that
	// here.
	
	/////--- TAKEN (shamelessly) FROM PPU466.cpp, originally by cool guy Jim McCann ---/////

	//set up screen scaling:
	if (drawable_size.x < ppu.ScreenWidth || drawable_size.y < ppu.ScreenHeight) {
		//if screen is too small, just do some inglorious pixel-mushing:
		//TODO: handle this awful, rare case
	} else {
		//otherwise, do careful integer-multiple upscaling:
		//largest size that will fit in the drawable:
		const uint32_t scale = std::max( 1U, std::min(drawable_size.x / ppu.ScreenWidth, drawable_size.y / ppu.ScreenHeight) );

		//compute lower left so that screen is centered:
		const glm::ivec2 lower_left = glm::ivec2(
			(int32_t(drawable_size.x) - scale * int32_t(ppu.ScreenWidth)) / 2,
			(int32_t(drawable_size.y) - scale * int32_t(ppu.ScreenHeight)) / 2
		);

	/////--- END TAKEN ---/////

		// scale           flip origin                 translate
		//           +-                     -+   +-                 -+ 
		//           | 1  0  0               |   | 1 0 -lower_left.x |
		// 1/scale * | 0 -1  drawable_size.y | * | 0 1 -lower_left.y |
		//           | 0  0  1               |   | 0 0 1             |
		//           +-                     -+   +-                 -+

		screen_to_ppu = (1.0f / scale) * glm::mat3x2(
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, -1.0f),
			glm::vec2(0.0f, drawable_size.y) - glm::vec2(lower_left)
		);
	}

	const glm::ivec2 lower_left = glm::ivec2(
			(int32_t(glm::floor(camera_pos.x) - ppu.BackgroundWidth/4)),
			(int32_t(glm::floor(camera_pos.y) - ppu.BackgroundHeight/4))
		);

	for (int y = 0; y < (int)(ppu.BackgroundHeight/2+1); y++) {
		int map_y = y + lower_left.y;
		for (int x = 0; x < (int)(ppu.BackgroundWidth/2+1); x++) {
			int map_x = x + lower_left.x;
			size_t ppu_index = y * ppu.BackgroundWidth + x;
			if (map_y < 0 || map_y >= (int)map_height ||
				map_x < 0 || map_x >= (int)map_width) {
				ppu.background[ppu_index] = 0;
			} else {
				ppu.background[ppu_index] = map_tiles[map_y * map_width + map_x];
			}
		}
	}

	ppu.background_color = glm::u8vec4(0x00, 0x00, 0x00, 0xff);


	//background scroll:
	ppu.background_position.x = int32_t(-glm::fract(camera_pos.x) * 8); 
	ppu.background_position.y = int32_t(-glm::fract(camera_pos.y) * 8);


	//player sprite:
	glm::vec2 player_pos = glm::vec2(
			(player_tile_pos.x - camera_pos.x) * 8 + PPU466::ScreenWidth / 2 - 4,
			(player_tile_pos.y - camera_pos.y) * 8 + PPU466::ScreenHeight / 2 - 4
	);
	ppu.sprites[0].x = int32_t(player_pos.x);
	ppu.sprites[0].y = int32_t(player_pos.y);
	ppu.sprites[0].index = 32;
	ppu.sprites[0].attributes = 7;

	glm::ivec2 mouse_raw;
	SDL_GetMouseState(&(mouse_raw.x), &(mouse_raw.y));
	glm::vec2 mouse = screen_to_ppu * glm::vec3(mouse_raw, 1.0f);

	ppu.sprites[1].x = int32_t(mouse.x);
	ppu.sprites[1].y = int32_t(mouse.y);
	ppu.sprites[1].index = 2;
	ppu.sprites[1].attributes = 7;


	Flashlight flashlight;
	glm::vec2 cast = flashlight.cast_ray((player_pos - glm::vec2(ppu.background_position))/8.0f, glm::normalize(mouse - player_pos), lower_left, map_tiles, map_width);
	ppu.sprites[2].x = int32_t(cast.x * 8) + ppu.background_position.x;
	ppu.sprites[2].y = int32_t(cast.y * 8) + ppu.background_position.y;
	ppu.sprites[2].index = 2;
	ppu.sprites[2].attributes = 1;

	//--- actually draw ---
	ppu.draw(drawable_size);
}
