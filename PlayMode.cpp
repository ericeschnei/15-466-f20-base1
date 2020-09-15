#include "PlayMode.hpp"
//for the GL_ERRORS() macro:
#include "SDL_events.h"
#include "gl_errors.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "load_save_png.hpp"
#include "data_path.hpp"
#include "sprites.hpp"
//for glm::value_ptr() :
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include <random>

inline void load_map(std::vector<uint8_t> &map_tiles, size_t &map_width, size_t &map_height) {
	glm::uvec2 size;
	
	std::vector<glm::u8vec4> data;
	auto path = data_path("maze.png");
	load_png(path, &size, &data, OriginLocation::LowerLeftOrigin);

	map_width = size.x;
	map_height = size.y;

	for (size_t y = 0; y < size.y; y++) {
		for (size_t x = 0; x < size.x; x++) {
			glm::u8vec4 d = data[y * size.x + x];
			if (d.x == 0) {
				// reserved wall tile
				map_tiles.push_back(7);
			} else {
				map_tiles.push_back(SpriteIndices::tile[2 * (y % 2) + (x % 2)]);
			}

		}
	}

}

inline void load_tiles(PPU466 &ppu) {
	glm::uvec2 size;
	std::vector<glm::u8vec4> data;
	auto path = data_path("sprites.png");
	
	load_png(path, &size, &data, OriginLocation::LowerLeftOrigin);
	
	for (size_t y = 0; y < size.y/8; y++) {
		for(size_t x = 0; x < size.x/8; x++) {	
			size_t tile_index = y * size.x/8 + x;
			for (size_t yy = 0; yy < 8; yy++) {
				uint8_t bit0 = 0;
				uint8_t bit1 = 0;
				for (int xx = 7; xx >= 0; xx--) {
					size_t image_index = (y * 8 + yy) * size.x + (x * 8 + xx);
					uint8_t brightness = data[image_index].x;
					
					bit0 <<= 1;
					bit1 <<= 1;
					
					if (data[image_index].w) {
						if (brightness == 0) {
							bit0 += 1;
						} else if (brightness < 255) {
							bit1 += 1;
						} else {
							bit0 += 1;
							bit1 += 1;
						}
					}
				}
				ppu.tile_table[tile_index].bit0[yy] = bit0;
				ppu.tile_table[tile_index].bit1[yy] = bit1;
			}

		}

	}

}
PlayMode::PlayMode() {

	load_map(map_tiles, map_width, map_height);
	load_tiles(ppu);
	player_tile_pos = glm::vec2(map_width/2.0f + 2.0f, map_height/2.0f);
	camera_pos = glm::vec2(map_width/2.0f + 2.0f, map_height/2.0f);


	//makes the outside of tiles 0-16 solid:
	ppu.palette_table[0] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0x99, 0xff),
		glm::u8vec4(0xaa, 0xaa, 0xbb, 0xff),
		glm::u8vec4(0xcc, 0xcc, 0xdd, 0xff),
	};

	//makes the center of tiles 0-16 solid:
	ppu.palette_table[1] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x0d, 0xa3, 0x2e, 0xff),
		glm::u8vec4(0x2d, 0xba, 0x2b, 0xff),
		glm::u8vec4(0x83, 0xf0, 0x5b, 0xff),
	};

	ppu.palette_table[2] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0xdd, 0xdd, 0xff, 0xff),
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
		glm::u8vec4(0x20, 0x00, 0x80, 0xff),
		glm::u8vec4(0xbb, 0x55, 0x00, 0xff),
		glm::u8vec4(0xf2, 0xd7, 0xa0, 0xff),
	};

	//used for the misc other sprites:
	ppu.palette_table[6] = {
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
		glm::u8vec4(0x88, 0x88, 0xff, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		glm::u8vec4(0x00, 0x00, 0x00, 0x00),
	};

	static std::mt19937 mt;

	for (int i = 0; i < 40; i++) {
		coins.emplace_back(mt() % map_width, mt() % map_height);
		while (map_tiles[coins.back().y * map_width + coins.back().x]) {
			coins.back() = glm::ivec2(mt() % map_width, mt() % map_height);
		}
	}

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (game_timer == 0.0f) {
		return false;
	}

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

	if (game_timer == 0) {
		return;
	}

	frame_timer += elapsed;
	if (frame_timer >= (4.0f * time_per_frame)) frame_timer -= 4.0f * time_per_frame;

	game_timer = glm::max(0.0f, game_timer - elapsed);

	auto is_wall = [this](const glm::vec2 &v){
		return this->map_tiles[((uint32_t)v.y) * this->map_width + ((uint32_t)v.x)] == 7;
	};

	constexpr float PlayerSpeed = 6.0f;
	if (left.pressed) {
		player_tile_pos.x -= PlayerSpeed * elapsed;
		last_pressed = LEFT;
		
		if (is_wall(player_tile_pos - glm::vec2(1.0f, 0.0f)) ||
			is_wall(player_tile_pos - glm::vec2(1.0f, 1.0f)) ||
			is_wall(player_tile_pos - glm::vec2(1.0f, -1.0f))) {
			
			player_tile_pos.x = glm::ceil(player_tile_pos.x) + 0.01f;

		}
		
	}
	if (right.pressed) {
		player_tile_pos.x += PlayerSpeed * elapsed;
		last_pressed = RIGHT;

		if (is_wall(player_tile_pos + glm::vec2(1.0f, 0.0f)) ||
			is_wall(player_tile_pos + glm::vec2(1.0f, 1.0f)) ||
			is_wall(player_tile_pos + glm::vec2(1.0f, -1.0f))) {
			
			player_tile_pos.x = glm::floor(player_tile_pos.x) - 0.01f;

		}
	}
	if (down.pressed) {
		player_tile_pos.y -= PlayerSpeed * elapsed;
		last_pressed = DOWN;

		if (is_wall(player_tile_pos - glm::vec2(0.0f, 1.0f)) ||
			is_wall(player_tile_pos - glm::vec2(1.0f, 1.0f)) ||
			is_wall(player_tile_pos - glm::vec2(-1.0f, 1.0f))) {
			
			player_tile_pos.y = glm::ceil(player_tile_pos.y) + 0.01f;

		}
	}
	if (up.pressed) {
		player_tile_pos.y += PlayerSpeed * elapsed;
		last_pressed = UP;

		if (is_wall(player_tile_pos + glm::vec2(0.0f, 1.0f)) ||
			is_wall(player_tile_pos + glm::vec2(1.0f, 1.0f)) ||
			is_wall(player_tile_pos + glm::vec2(-1.0f, 1.0f))) {
			
			player_tile_pos.y = glm::floor(player_tile_pos.y) - 0.01f;

		}
		
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	glm::vec2 camera_dead_radius = glm::vec2(1.0f, 1.0f);

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

	// collect emeralds
	for (size_t i = 0; i < coins.size(); i++) {
		if (glm::distance(glm::vec2(coins[i]), player_tile_pos) < 1.4) {
			score++;
			coins[i] = glm::ivec2(-1000, -1000);
		}

	}


}


void PlayMode::draw_player() {

	int frame = (int)(frame_timer / time_per_frame);
	int pose;
	if (!(left.pressed || right.pressed || up.pressed || down.pressed)) {
		pose = 0;
	} else {
		pose = (frame == 0 || frame == 2) ? 0 : (frame == 1 ? 1 : 2);
	}

	//player sprite:
	glm::vec2 player_pos = glm::vec2(
			(player_tile_pos.x - camera_pos.x) * 8 + PPU466::ScreenWidth / 2,
			(player_tile_pos.y - camera_pos.y) * 8 + PPU466::ScreenHeight / 2
	);

	auto sprites = SpriteIndices::player[last_pressed][pose];
	auto x_offsets = std::array<float, 4> {8.0f, 0.0f, 8.0f, 0.0f};
	auto y_offsets = std::array<float, 4> {8.0f, 8.0f, 0.0f, 0.0f};
	for (int i = 0; i < 4; i++) {
		ppu.sprites[i+40].x = int32_t(player_pos.x - x_offsets[i]);
		ppu.sprites[i+40].y = int32_t(player_pos.y - y_offsets[i]);
		ppu.sprites[i+40].index = sprites[i];
		ppu.sprites[i+40].attributes = 7;
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

	ppu.background_color = glm::u8vec4(0x00, 0x00, 0x00, 0xff);


	//background scroll:
	ppu.background_position.x = int32_t(-glm::fract(camera_pos.x) * 8); 
	ppu.background_position.y = int32_t(-glm::fract(camera_pos.y) * 8);


	//player sprite:
	glm::vec2 player_pos = glm::vec2(
			(player_tile_pos.x - camera_pos.x) * 8 + PPU466::ScreenWidth / 2,
			(player_tile_pos.y - camera_pos.y) * 8 + PPU466::ScreenHeight / 2
	);

	draw_player();

	flashlight.update_lightmap((player_pos - glm::vec2(ppu.background_position)), lower_left, map_tiles, map_width, map_height);
	std::array<uint8_t, Flashlight::lightmap_width * Flashlight::lightmap_height / 64> newmap;
	std::vector<PPU466::Tile> dyntiles;
	flashlight.graft(lower_left, map_tiles, map_width, map_height, ppu.tile_table, newmap, dyntiles);

	for (size_t y = 0; y < Flashlight::lightmap_height / 8; y++) {
		for (size_t x = 0; x < Flashlight::lightmap_width / 8; x++) {
			ppu.background[PPU466::BackgroundWidth * y + x] = newmap[Flashlight::lightmap_width/8 * y + x];
		}
	}

	for (size_t i = 0; i < dyntiles.size(); i++) {
		ppu.tile_table[i + Flashlight::DYNAMIC_TILE_START] = dyntiles[i];
	}

	for (size_t i = 0; i < coins.size(); i++) {
		glm::vec2 coin_pos = glm::vec2(
			(coins[i].x - camera_pos.x) * 8 + PPU466::ScreenWidth / 2,
			(coins[i].y - camera_pos.y) * 8 + PPU466::ScreenHeight / 2
		);

		if (coin_pos.x < 0 || coin_pos.x >= PPU466::ScreenWidth) {
			ppu.sprites[i].y = 248;
			continue;
		}
		if (coin_pos.y < 0 || coin_pos.y >= PPU466::ScreenHeight) {
			ppu.sprites[i].y = 248;
			continue;
		}


		PPU466::Tile t = ppu.tile_table[SpriteIndices::coin];
		
		glm::vec2 lower_left = coin_pos - glm::vec2(ppu.background_position);
		for (int y = 0; y < 8; y++) {
			uint8_t row = 0;
			for (int x = 7; x >= 0; x--) {
				bool light = flashlight.lightmap[(int)(lower_left.y + y) * Flashlight::lightmap_width + (int)(lower_left.x + x)];
				row <<= 1;
				if (light) row += 1;
			}
			t.bit0[y] &= row;
			t.bit1[y] &= row;
		}

		ppu.tile_table[64 + i] = t;

		ppu.sprites[i].attributes = 1;
		ppu.sprites[i].index = 64 + i;
		ppu.sprites[i].x = coin_pos.x;
		ppu.sprites[i].y = coin_pos.y;
	}

	// timer
	ppu.sprites[44].attributes = 2;
	ppu.sprites[44].index = SpriteIndices::numerals[((int)game_timer) / 10];
	ppu.sprites[44].x = 0;
	ppu.sprites[44].y = PPU466::ScreenHeight - 8;
	ppu.sprites[45].attributes = 2;
	ppu.sprites[45].index = SpriteIndices::numerals[((int)game_timer) % 10];
	ppu.sprites[45].x = 8;
	ppu.sprites[45].y = PPU466::ScreenHeight - 8;

	// scoreboard
	ppu.sprites[46].attributes = 2;
	ppu.sprites[46].index = SpriteIndices::numerals[score / 10];
	ppu.sprites[46].x = PPU466::ScreenWidth - 16;
	ppu.sprites[46].y = PPU466::ScreenHeight - 8;
	ppu.sprites[47].attributes = 2;
	ppu.sprites[47].index = SpriteIndices::numerals[score % 10];
	ppu.sprites[47].x = PPU466::ScreenWidth - 8;
	ppu.sprites[47].y = PPU466::ScreenHeight - 8;

	//--- actually draw ---
	ppu.draw(drawable_size);
}
