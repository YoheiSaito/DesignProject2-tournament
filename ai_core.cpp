#include <unordered_map>
#include "Board.hpp"
#include "Command.hpp"
#include "ai_core.hpp"
using namespace teyo_shogi;
/* typedef std::unordered_map<Board_p, Game_Node> Game_Tree; */

Game_Tree game_tree;

bool winable_check(Board_p& x, int self){
	int trh = (self==BLACK)?0:BOARD_HEIGHT-1;
	int enm = (self==BLACK)?WHITE:BLACK;

	Koma self_lion, enemy_lion;
	self_lion.player = self;
	self_lion.type = LION;
	enemy_lion.player = enm;
	enemy_lion.type = LION;


	if(x->caps[self][0].type == LION){
		return true;
	}else{

		for(int i = 0; i < BOARD_WIDTH; i++){
			if( x->field[i][trh].data == self_lion.data){
				if( !x->is_check(i, trh, enm))
					return true;
			}
			for(int j = 0; j < BOARD_HEIGHT; j++){
				if( x->field[i][j].data == enemy_lion.data){
					if(x->is_check(i,j,self)){
						return true;
					}
				}
			}
		}
	}
	return false;
}

int16_t evalate(Board_p p, int turn){
	//0, 175, 185, 115, 100, 10000
	static int16_t Mochigoma_point[] = {
		0, 165, 175, -200, 90, 10000
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
	int s = turn==BLACK?1:-1;
	int h = turn==BLACK?0:BOARD_HEIGHT-1;
	int ntrn = turn==BLACK?WHITE:BLACK;
	Koma slion, nlion;
	slion.type = LION;
	slion.player = turn;
	nlion.type = LION;
	nlion.player = ntrn;
	//try check
	for(int i = 0; i < BOARD_WIDTH; i++){
		if ( p->field[i][h].type == LION &&
				p->field[i][h].player == turn){
			if(!p->is_check(i,h,ntrn))
				return 30000*s;
		}
	}
	//have captured LION
	if(p->caps[turn][0].type == LION){
		return 30000;
	}
	if(p->caps[ntrn][0].type == LION){
		return -30000;
	}

	for(int i = 0; i < BOARD_WIDTH; i++){
		for(int j = 0; j < BOARD_HEIGHT; j++){
			int kt = p->field[i][j].type;
			if( p->field[i][j].player == BLACK){
				r += Koma_point[i%2][j][BLACK][kt];
			}else{
				r -= Koma_point[i%2][j][WHITE][kt];
			}
			if( p->field[i][j].data == nlion.data){
				if( p->is_check(i,j, turn)){
					return 30000*s;
				}
			}
		}
	}
	for( int i = 0; i < MAX_CAPS; i++){
		r += Mochigoma_point[p->caps[BLACK][i].type];
		r -= Mochigoma_point[p->caps[WHITE][i].type];
	}
	return s*r;
}
int16_t negamax(Board_p& st, int turn, int depth, 
		int a  = INT16_MIN+1, int b = INT16_MAX)
{
	size_t h = st->hash();
	if(depth == 0){
		int se = evalate(st, turn);
		game_tree[h].evalation[turn] = se;
		return se;
	}
	if(winable_check(st, turn)){
		return (turn==BLACK)?30000:-30000;
	}
	Move_t mvs = st->generate_move(turn);
	if(mvs.size() == 0){ // no moves
		return (turn==BLACK)?-30000:30000;
	}

	int ntrn = (turn==BLACK)?WHITE:BLACK;
	game_tree[h].next[turn] = mvs;
	
	int16_t dev = INT16_MIN;
	for(auto&&i : mvs){
		//相手の評価の中で一番低いもの -> -で最大のものを選ぶ
		int16_t ev = -negamax(i, ntrn, depth-1, -b, -a);
		game_tree[i->hash()].evalation[ntrn] = ev;
		if( dev < ev ){
			dev = ev;
			game_tree[h].best[turn] = i;
		}
		if( a < dev){
			a = dev;
		}
		if( a >= b){
			break;
		}
	}
	game_tree[h].evalation[turn] = dev;
	return dev;
}
Board_p adventure(Board_p& board, int turn, int depth) {
	int r = negamax(board, turn, depth);
	game_tree[board->hash()].evalation[turn] = r;
	return game_tree[board->hash()].best[turn];
}

