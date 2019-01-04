#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include "Board.hpp"
#include "evalation.hpp"
#include "Command.hpp"

#define MAX_DEPS 8
// 現在は150程度でひよこ得
#define WORSE 150

namespace {
using namespace teyo_shogi;
}

using Dynamic_Evals = std::pair<Board_p, int16_t>;
namespace teyo_shogi{
	struct Game_Node{
		int emerge;
		Move_t next;
		Dynamic_Evals eval;
		bool check;
	};
	using Game_Node_p = std::shared_ptr<Game_Node>;

};

// types 
using Game_Tree = std::unordered_map<size_t, Game_Node>;
// treeに入っていないものはフロンティア
// BLACK と WHITEで別々のツリーを持っているように見せる
// ただし参照先は互いとなる.
/* Game_Tree is map that connect Board Hash to Game_Node  */

struct DobutsuAI{

Game_Tree game_tree[2];
// method
DobutsuAI(std::string evalfile):
	eval(evalfile)
{}
DobutsuAI():
	eval()
{}

Evalation eval;
Board_p adventure(Board_p& board, int turn, int depth);
Dynamic_Evals negamax(Board_p& board, int turn, int depth, int a, int b);
Dynamic_Evals negamax_avoid(Board_p& board, int turn, int depth, int a, int b);
bool is_win(Board_p& x, int self);
int16_t evalate(Board_p p);

};

