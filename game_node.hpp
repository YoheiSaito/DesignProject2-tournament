#include "Board.hpp"
#include <vector>

namespace teyo_shogi{
struct game_node{
	int8_t evaration;
	uint8_t emerge;
	std::string best_command;
	size_t best_hash;
	std::vector<size_t> next_hash;
};
};
