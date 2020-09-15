#pragma once
#include <array>
#include <string>
struct SpriteIndices {

	// order: bl, br, tl, tr, (bl2, br2)
	static constexpr std::array<uint8_t, 4> hole {96, 97, 104, 105};
	static constexpr std::array<uint8_t, 4> fill {98, 99, 106, 107};
	static constexpr std::array<uint8_t, 6> fplayer {118, 119, 126, 127, 108, 109};
	static constexpr std::array<uint8_t, 6> bplayer {102, 103, 110, 111, 100, 101};
	static constexpr std::array<uint8_t, 4> rskull {114, 115, 122, 123};
	static constexpr std::array<uint8_t, 4> lskull {116, 117, 124, 125};
	static constexpr std::array<uint8_t, 4> tile {120, 121, 112, 113};

	static constexpr std::array<uint8_t, 30> alph {
		90, 80, 81, 82, 83, 84, 85, 86, 87, 72, 73, 74, 75, 76, 77, 78, 79,
		64, 65, 66, 67, 68, 69, 70, 71, 56, 57, 58, 59, 60
	};


};
