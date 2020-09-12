#include "PPU466.hpp"
#include <vector>
/**
 * Flashlight
 * ----------
 * This file defines the post-processing routine that draws 
 * the flashlight effect.
 *
 * The algorithm is, at its core, inspired by this write-up by Nicky Case:
 * https://ncase.me/sight-and-light/
 *
 * It is, however, adapted to work with the pixelized nature of this game.
 * The workflow is as follows:
 *
 * 1) Using the camera position, generate the 2D array of tiles that
 *    would be drawn on the screen if the entire scene was lit.
 * 2) Get the mouse and player positions relative to the origin of the tilemap.
 * 3) Cast rays and generate the lightmap as a 256x240 image, split into tiles.
 * 4) Split the tilemap into three categories, as follows:
 *   a) Any tile that is fully in light can use its original tile
 *   b) Any tile that is fully in shade can use the background color
 *   c) Edge tiles
 * 5) Create a std::unordered_map to try to reduce the number of unique
 *    edge tile sprites we need to use.
 * 6) Write all the tiles back to an array and allow indexing.
 */

struct Flashlight {

	Flashlight();
	~Flashlight();

	typedef std::array<uint8_t, 8> tilelight_t;

	static constexpr size_t lightmap_width = PPU466::BackgroundWidth/2 + 1;
	static constexpr size_t lightmap_height = PPU466::BackgroundHeight/2 + 1;
	std::array<tilelight_t, lightmap_height*lightmap_width> lightmap;

	// the position in the tilemap where dynamic tiles begin to generate.
	static constexpr size_t DYNAMIC_TILE_START = 128;

	/**
	 * Updates the values of "lightmap".
	 * Inputs:
	 * - player:     The position of the player, in pixel space
	 * - mouse:      The position of the mouse, in pixel space
	 * - lower_left: The lower_left tile in the map. Can be outside of map bounds.
	 * - map:        The map.
	 */
	void update_lightmap(
			const glm::vec2 &               player,
			const glm::vec2 &               mouse,
			const glm::ivec2 &              lower_left,
			const std::vector<uint8_t> &    map,
			size_t                          map_width);

	/**
	 * Casts a ray, returns its intersection point. 
	 * Inputs:
	 * - from:       The point to cast the ray from. In pixel space
	 * - angle:      The angle to cast the ray at, CCW from +x direction.
	 * - lower_left: The lower_left tile in the map. Can be outside of map bounds.
	 * - map:        The map.
	 */
	glm::vec2 cast_ray(
			const glm::vec2 &               from,
			const glm::vec2 &               direction,
			const glm::ivec2 &              lower_left,
			const std::vector<uint8_t> &    map,
			size_t                          map_width);	

};
