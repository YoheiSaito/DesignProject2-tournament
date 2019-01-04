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


Dynamic_Evals DobutsuAI::negamax( Board_p& board, int turn, int depth,
		int a = INT16_MIN + 1, int b = INT16_MAX) {

	Dynamic_Evals ret;
	// mvs : move result 
	// from this vector, we choose the highest evalation game node;
	int sgn =  (turn==BLACK)? 1: -1;
	int ntrn = (turn==BLACK)? WHITE: BLACK;
	if( depth == 0){
		ret.second= sgn*eval.evalate(board);
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
		// 負けるときは長い手順を選ぶ(depthは小さいほうが良い)
		ret.second= -sgn*(30000+depth);
		ret.first = nullptr;
		game_tree[turn][board->hash()].eval = ret;
		return ret;
	}
	ret.second = INT16_MIN;

	
	for(auto&& i: mvs){
		// if generated board is ended board, return win
		if( is_win( i, turn) ){
			// 勝つときは短い手順を選ぶ(depthは大きいほうが良い)
			ret.second = 30000+depth;
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

