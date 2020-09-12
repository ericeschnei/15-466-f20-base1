#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include <array>
#include <unordered_map>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::vec2   player_tile_pos;
	glm::vec2   camera_pos;

	// convert from screen space (top-left origin)
	// to ppu space (bottom-left origin)
	glm::mat3x2 screen_to_ppu;
	//----- drawing handled by PPU466 -----

	// data structures for light raycasting
	size_t map_width;
	size_t map_height;

	std::vector<uint8_t> map_tiles;
	std::vector<glm::u64vec2> map_corners;

	// hash function to create unordered_map of pairs,
	// https://www.geeksforgeeks.org/how-to-create-an-unordered_map-of-pairs-in-c/
	struct hash_pair { 
		template <class T1, class T2> 
		size_t operator()(const std::pair<T1, T2>& p) const
		{ 
			auto hash1 = std::hash<T1>{}(p.first); 
			auto hash2 = std::hash<T2>{}(p.second); 
			return hash1 ^ hash2; 
		} 
	}; 

	std::array<
		size_t, 
		(PPU466::BackgroundWidth/2 + 1) * (PPU466::BackgroundHeight/2 + 1)
	> tile_buffer;
	std::unordered_map<std::pair<uint64_t, uint64_t>, size_t, hash_pair> tiledata_buffer;
	size_t tiledata_counter;

	PPU466 ppu;
};
