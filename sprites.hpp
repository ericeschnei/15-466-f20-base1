#pragma once
#include <array>
#include <string>
struct SpriteIndices {

	// order: bl, br, tl, tr, (bl2, br2)
	static constexpr std::array<uint8_t, 4> tile {0, 1, 8, 9};

	static constexpr std::array<uint8_t, 64> palettes {
		0, 0, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 2,
		3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3
	};

};
