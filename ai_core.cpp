// TODO
// - recraft evalate function
// - Tsume Shogi Solver
// - speed up
// - 


#include <unordered_map>
#include "Board.hpp"
#include "Command.hpp"
#include "ai_core.hpp"

using namespace teyo_shogi;

bool DobutsuAI::is_win(Board_p& x, int self){
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

int16_t DobutsuAI::evalate(Board_p p){
	//0, 175, 185, 115, 100, 10000
	// none, zou, Kirin, Niwatori hiyoko, Lion
	static int16_t Mochigoma_point[] = {
		0, 120, 140, 90, 90, 10000
	};
	static int16_t Koma_point[2][4][2][6] = {
		{
			{
				{0, 170, 150, 105,  60, 10000},
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
				{0, 130, 150, 105,  60, 10000},
			}
		},
		//中央
		{
			{
				{0, 130, 160, 105,  60, 10000},
				{0, 130, 160, 125, 100, 10000},
			},
			{
				{0, 155, 175, 130, 100, 10000},
				{0, 155, 175, 130, 100, 10000}
			},
			{
				{0, 155, 175, 130, 100, 10000},
				{0, 155, 175, 130, 100, 10000}
			},
			{
				{0, 130, 160, 125, 100, 10000},
				{0, 130, 160, 105,  60, 10000},
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

Dynamic_Evals DobutsuAI::negamax( Board_p& board, int turn, int depth,
		int a = INT16_MIN + 1, int b = INT16_MAX) {

	Dynamic_Evals ret;
	// mvs : move result 
	// from this vector, we choose the highest evalation game node;
	int sgn =  (turn==BLACK)? 1: -1;
	int ntrn = (turn==BLACK)? WHITE: BLACK;
	if( depth == 0){
		ret.second= sgn*evalate(board);
		ret.first = nullptr;
		return ret;
	}
	Move_t mvs;
	// 未展開なら展開を行う
	if(game_tree[turn][board->hash()].next.size() == 0){
		mvs = board->generate_move(turn);
		game_tree[turn][board->hash()].next = mvs;
	}else{
		mvs = game_tree[turn][board->hash()].next;
	}
	if( mvs.size() == 0){
		// lose 
		ret.second= -sgn*30000;
		ret.first = nullptr;
		game_tree[turn][board->hash()].eval = ret;
		return ret;
	}
	ret.second = INT16_MIN;

	
	for(auto&& i: mvs){
		// if generated board is ended board, return win
		if( is_win( i, turn) ){
			ret.second = 30000;
			ret.first = i;
			game_tree[turn][board->hash()].eval = ret;
			break;
		}
		Dynamic_Evals next_eval = negamax( i, ntrn, depth-1, -b, -a);
		if( -next_eval.second > ret.second){
			ret.second = -next_eval.second;
			ret.first = i;
			if( a < ret.second){
				a = ret.second;
			}
		}
		if( a >= b)
			break;

	}
	
	game_tree[turn][board->hash()].eval = ret;
	return ret;
}

Dynamic_Evals DobutsuAI::negamax_avoid( Board_p& board, int turn, int depth,
		int a = INT16_MIN + 1, int b = INT16_MAX) 
{
	// mvs : move result 
	// from this vector, we choose the highest evalation game node;
	int ntrn = (turn==BLACK)? WHITE: BLACK;
	// これは一回しか呼ばれない
	// 動きが作れないこともない
	
	// mvsもあるハズ
	Move_t mvs = game_tree[turn][board->hash()].next;

	Dynamic_Evals ret;
	ret.second = INT16_MIN;
	ret.first = nullptr;
	for(auto&& i: mvs){
		// 勝利でリターンできれば二度目なんて来るはずがない
		if( game_tree[ntrn].find(i->hash()) != game_tree[ntrn].end()){
			continue;
		}
		Dynamic_Evals next_eval = negamax( i, ntrn, depth-1, -b, -a);
		if( -next_eval.second > ret.second){
			ret.second = -next_eval.second;
			ret.first = i;

			if( a < ret.second){
				a = ret.second;
			}
		}
		if( a >= b)
			break;
	}

	return ret;

}

Board_p DobutsuAI::adventure(Board_p& board, int turn, int depth) {

	// 初出の盤面の時はemergeを1とし, 探索をする
	/* if( game_tree[turn].find(board->hash()) == game_tree[turn].end()){ */
	if( game_tree[turn][board->hash()].emerge == 0){
		/* std::cout << game_tree[turn][board->hash()].emerge << std::endl; */
		game_tree[turn][board->hash()].emerge = 1;
		Dynamic_Evals r = negamax(board, turn, depth+1);

		//読み筋のコンソール出力
		std::cout << "reading flow" << std::endl;
		Board_p tmp = r.first;
		std::cout << "evals" << r.second << std::endl;
		for(int i = 0; i <= depth; i++){
			std::cout << "depth " << depth -i << std::endl;
			tmp->print();
			std::cout << std::endl;
			tmp = game_tree[turn^(i%2)^1][tmp->hash()].eval.first;
			if(tmp == nullptr)
				break;
		}
		game_tree[turn^1][r.first->hash()].emerge = 1;
		//盤面を返す
		return r.first;

	}else{
		std::cout<< "twice or more"<< std::endl;
		std::cout<< game_tree[turn][board->hash()].eval.second << std::endl;
		// 局面カウント
		game_tree[turn][board->hash()].emerge++;
		//
		// 不利ならば打開しない
		// WORSE は ai_core.hppにて定義
		// 現在は150程度でひよこ得
		if( 
			/* turn==BLACK&& */
			game_tree[turn][board->hash()].eval.second < WORSE
		  ){
			std::cout<< "GO TO SEN NICHI TE"<< std::endl;
			game_tree[turn][board->hash()].emerge++;
			return game_tree[turn][board->hash()].eval.first;
		}

		//打開時
		//negamax_avoidを行って思しき局面が無ければ打開をやめる
		Dynamic_Evals r = negamax_avoid(board, turn, depth+1);
		if( r.first == nullptr){
			std::cout<< "GO TO SEN NICHI TE"<< std::endl;
			return game_tree[turn][board->hash()].eval.first;
		}else if( r.second < WORSE){
			std::cout<< "GO TO SEN NICHI TE"<< std::endl;
			return game_tree[turn][board->hash()].eval.first;
		}
		std::cout<< "AVOID NICHI TE"<< std::endl;
		game_tree[turn^1][r.first->hash()].emerge = 1;
		return r.first;
	}

}

