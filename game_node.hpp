#pragma once
#include <vector>
#include <memory>

namespace teyo_shogi{
struct Game_Node{
	int16_t evalation[2];
	int emerge[2];
	Board_p best[2];
	Move_t next[2];
};
using Game_Node_p = std::shared_ptr<Game_Node>;

};
