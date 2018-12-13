#pragma once
#include <vector>
#include <memory>

namespace teyo_shogi{
struct Game_Node{
	int16_t evalation;
	uint8_t emerge;
};
using Game_Node_p = std::shared_ptr<Game_Node>;

};
