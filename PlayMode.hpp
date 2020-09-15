#include "PPU466.hpp"
#include "Mode.hpp"
#include "Flashlight.hpp"

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

	void draw_player();
	//----- game state -----


	// light effects
	Flashlight flashlight;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//player position:
	glm::vec2   player_tile_pos;
	glm::vec2   camera_pos;

	static const int LEFT = 0;
	static const int RIGHT = 1;
	static const int UP = 2;
	static const int DOWN = 3;

	int last_pressed = UP;
	int score = 0;

	std::vector<glm::ivec2> coins;

	float frame_timer;
	float game_timer = 60.0f;
	static constexpr float time_per_frame = 0.1f;

	// convert from screen space (top-left origin)
	// to ppu space (bottom-left origin)
	glm::mat3x2 screen_to_ppu;
	//----- drawing handled by PPU466 -----

	// data structures for light raycasting
	size_t map_width;
	size_t map_height;

	std::vector<uint8_t> map_tiles;

	PPU466 ppu;
};
