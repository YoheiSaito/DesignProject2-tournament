#include <unordered_map>
#include "Board.hpp"
#include "Command.hpp"
#include "ai_core.hpp"
#define DEBUG 1
using namespace teyo_shogi;
/* typedef std::unordered_map<Board_p, Game_Node> Game_Tree; */

Game_Tree game_tree;

bool is_win(Board_p& x, int self){
	int trh = (self==BLACK)?0:BOARD_HEIGHT-1;
	int enm = (self==BLACK)?WHITE:BLACK;

	Koma self_lion;
	self_lion.player = self;
	self_lion.type = LION;


	if(x->caps[self][0].type == LION){
		return true;
	}else{
		for(int i = 0; i < BOARD_WIDTH; i++){
			if( x->field[i][trh].data == self_lion.data){
				if( !x->is_check(i, trh, enm))
					return true;
			}
		}
	}
	return false;
}

int16_t evalate(Board_p p){
	//0, 175, 185, 115, 100, 10000
	static int16_t Mochigoma_point[] = {
		0, 165, 175, 150, 90, 10000
	};
	static int16_t Koma_point[2][4][2][6] = {
		{
			{
				{0, 130, 150, 105, -20, 10000},
				{0, 130, 150, 115, 100, 10000},
			},
			//中二行
			{
				{0, 140, 160, 120, 100, 10000},
				{0, 140, 160, 120, 100, 10000}
			},
			{
				{0, 140, 160, 120, 100, 10000},
				{0, 140, 160, 120, 100, 10000}
			},
			{
				{0, 130, 150, 115, 100, 10000},
				{0, 130, 150, 105, -20, 10000},
			}
		},
		//中央
		{
			{
				{0, 130, 160, 105, -20, 10000},
				{0, 130, 160, 125, 100, 10000},
			},
			{
				{0, 155, 185, 130, 100, 10000},
				{0, 155, 185, 130, 100, 10000}
			},
			{
				{0, 155, 185, 130, 100, 10000},
				{0, 155, 185, 130, 100, 10000}
			},
			{
				{0, 130, 160, 125, 100, 10000},
				{0, 130, 160, 105, -20, 10000},
			}
		}
	};
	int16_t r = 0;
	for(int i = 0; i < BOARD_WIDTH; i++){
		for(int j = 0; j < BOARD_HEIGHT; j++){
			int kt = p->field[i][j].type;
			if( p->field[i][j].player == BLACK){
				r += Koma_point[i%2][j][BLACK][kt];
			}else{
				r -= Koma_point[i%2][j][WHITE][kt];
			}
		}
	}
	for( int i = 0; i < MAX_CAPS; i++){
		r += Mochigoma_point[p->caps[BLACK][i].type];
		r -= Mochigoma_point[p->caps[WHITE][i].type];
	}
	return r;
}
// TODO : implements a-b algorithm
Dynamic_Evals negamax( Board_p& board, int turn, int depth,
		int a = INT16_MIN + 1, int b = INT16_MAX) {

	Dynamic_Evals ret;
	// mvs : move result 
	// from this vector, we choose the highest evalation game node;
	auto mvs = board->generate_move(turn);
	int sgn =  (turn==BLACK)? 1: -1;
	int ntrn = (turn==BLACK)? WHITE: BLACK;
	if( depth == 0){
		ret.second= sgn*evalate(board);
		ret.first = nullptr;
		return ret;
	}
	if( mvs.size() == 0){
		// lose 
		ret.second= -sgn*30000;
		ret.first = nullptr;
		return ret;
	}
	ret.second = INT16_MIN;
	for(auto&& i: mvs){
		// if generated board is ended board, return win
		if( is_win( i, turn) ){
			ret.second = 30000;
			ret.first = i;
			break;
		}
		Dynamic_Evals next_eval = negamax( i, ntrn, depth-1, -b, -a);
		if( -next_eval.second > ret.second){
			ret.second = -next_eval.second;
			ret.first = i;
		}
		if( a < ret.second)
			a = ret.second;
		if( a >= b)
			break;
	}
	game_tree[board->hash()].eval[turn] = ret;
	return ret;
}

Board_p adventure(Board_p& board, int turn, int depth) {
	depth = 4;
	Dynamic_Evals r = negamax(board, turn, depth);
	std::cout << "reading flow" << std::endl;
	return r.first;
}

