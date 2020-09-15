#pragma once
#include <array>
#include <string>
struct SpriteIndices {

	// order: bl, br, tl, tr, (bl2, br2)
	static constexpr std::array<uint8_t, 4> tile {0, 1, 8, 9};

	// player[direction][frame][part]
	// LEFT RIGHT UP DOWN
	// STILL LEFT RIGHT
	// BL BR TL TR
	static constexpr std::array<std::array<std::array<uint8_t, 4>, 3>, 4> player {
		48, 49, 56, 57, 32, 33, 40, 41, 16, 17, 24, 25,
		54, 55, 62, 63, 38, 39, 46, 47, 22, 23, 30, 31,
		52, 53, 60, 61, 36, 37, 44, 45, 20, 21, 28, 29,
		50, 51, 58, 59, 34, 35, 42, 43, 18, 19, 26, 27,
	};

	static constexpr uint8_t coin = 15;

	static constexpr std::array<uint8_t, 10> numerals {
		10, 11, 12, 13, 14, 2, 3, 4, 5, 6
	};

};
