#pragma once
#include <unordered_map>
#include "Board.hpp"
#include "Command.hpp"
#include <vector>
#include <memory>

namespace {
using namespace teyo_shogi;
}
namespace teyo_shogi{
	struct Game_Node{
		int16_t evalation[2];
		int emerge[2];
		Board_p best[2];
		Move_t next[2];
	};
	using Game_Node_p = std::shared_ptr<Game_Node>;

};
// types 
typedef std::unordered_map<size_t, Game_Node> Game_Tree;
// treeに入っていないものはフロンティア
// BLACK と WHITEで別々のツリーを持っているように見せる
// ただし参照先は互いとなる.
/* Game_Tree is map that connect Board Hash to Game_Node  */

// method
Board_p adventure(Board_p& board, int turn, int depth);
bool winable_check(Board_p& x, int self);
int16_t evalate(Board_p p, int turn);

