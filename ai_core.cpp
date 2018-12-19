#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include "Board.hpp"
#include "game_node.hpp"
#include "Command.hpp"
#define MAX_DEPS 4
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
using namespace teyo_shogi;
/* typedef std::unordered_map<Board_p, Game_Node> Game_Tree; */
typedef std::unordered_map<size_t, Game_Node> Game_Tree;

Board_p adventure(int turn, int depth, Board_p& board);
bool winable_check(int self, Board_p& x);

namespace{
	// treeに入っていないものはフロンティア
	// BLACK と WHITEで別々のツリーを持っているように見せる
	// ただし参照先は互いとなる.
	/* Game_Tree is map that connect Board Hash to Game_Node  */
	Game_Tree game_tree;
}

int main(int argc, char* argv[])
{
	try{
		Board_p base(new Board);

	if( argc < 4){
		Board_p pre_board = nullptr;
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket(io_service);
		Command cmd(socket);
		char* ip = argc > 1? (argv[1]):(NULL);
		int port = argc > 2? std::stoi(argv[2]):(4444);
		int self = cmd.connect(ip, port);
		while(1){
	#if 1
			while(self != cmd.turn()){
				usleep(3e5);
			}
			//入力
			std::string input_board = cmd.board();
			base->write(input_board);
			if(pre_board != nullptr){
				if(!pre_board->check_legitimacy(*base, self)){
					std::cerr 
					<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					<< "!!! board might be illigal !!!"
					<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					<<std::endl;
					usleep(2e6);
				}
			}
			//千日手チェック
			if( game_tree.find(base->hash()) == game_tree.end()){
				game_tree[base->hash()].emerge[self] = 0;
			}else{
				game_tree[base->hash()].emerge[self]++;
			}
			if(game_tree[base->hash()].emerge[self] > 4){
				std::cout << "!!!sen-nchi-te!!!" << std::endl;
			}
			std::cout << game_tree[base->hash()].evalation[self] 
				<< base->evalate(self)<< std::endl; 
			//現在の状態の表示
			int ntrn = (self==BLACK)?WHITE:BLACK;
			//探索
			pre_board = adventure(self, MAX_DEPS , base);
			std::cout << game_tree[pre_board->hash()].evalation[self^1] 
				<< pre_board->evalate(self^1)<< std::endl; 
			pre_board->print();
			//コマンド送信
			if( !base->to_command(cmd, self, pre_board)){
				throw "move command error";
			}
	#else
			while(cmd.turn() != self){
				usleep(3e3);
			}
			
			std::string input_board = cmd.board();
			base->write(input_board);
			Move_t ck = base->generate_check(self);
			Board_p ckm = base->is_checkmate(self);

			ck[0]->print();
			if( ckm != nullptr){
				ckm->print();
			}
			while(cmd.turn() == self){
				usleep(3e3);
			}
		
	#endif
		}
	}else{
		int e = 0;
		Board_p pre_board(new Board);
		std::string init_str = std::string(INITBOARD);
		base->write(init_str);
		while(1){
			//TODO:千日手カウント
			base->print();
			if( winable_check(BLACK, base)){
				std::cout << "end" <<std::endl;
				break;
			}
			
			base = adventure(BLACK, MAX_DEPS, base);
			/* e = evalate( WHITE, base); */
			base->print();
			//TODO:千日手カウント
			if( winable_check(WHITE, base)){
				std::cout << "end" <<std::endl;
				break;
			}
			base = adventure(WHITE, MAX_DEPS, base);
			/* e = evalate( BLACK, base); */
		}
	}
	} catch(std::string e){
		std::cerr << e << std::endl;
	} catch( char const* e){
		std::cerr << e << std::endl;
	}catch (std::logic_error e){
		std::cerr << e.what() << std::endl;
	}
}
bool winable_check(int self, Board_p& x){
	int trh = (self==BLACK)?0:BOARD_HEIGHT-1;
	if(x->caps[self][0].type == LION){
		return true;
	}else{
		Koma self_lion, enemy_lion;
		self_lion.player = self;
		self_lion.type = LION;
		enemy_lion.player = self^1;
		enemy_lion.type = LION;
		for(int i = 0; i < BOARD_WIDTH; i++){
			if( x->field[i][trh].data == self_lion.data){
				if( !x->is_check(i, trh, self^1))
					return true;
				else
					return false;
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
//
//	@retval next board


int16_t negamax(Board_p& st, int turn, int depth, int a, int b){
	Move_t mvs = st->generate_move(turn);
	int se = st->evalate(turn);
	int ntrn = (turn==BLACK)?WHITE:BLACK;
	size_t h = st->hash();
	game_tree[h].next[turn] = mvs;
	if(depth == 0 || mvs.size() == 0){
		game_tree[h].evalation[turn] = se;
		/* for( int t = 0; t<MAX_DEPS- depth; t++){ */
			/* std::cout << '\t' << std::flush; */
		/* } */
		/* std::cout << se<< std::endl; */
		return se;
	}
	int16_t dev = INT16_MIN;
	for(auto&&i : mvs){
		//相手の評価の中で一番低いもの -> -で最大のものを選ぶ
		int16_t ev = -negamax(i, ntrn, depth-1, -b, -a);
		game_tree[i->hash()].evalation[ntrn] = ev;
		if(dev < ev ){
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
	/* for( int i = 0; i<MAX_DEPS- depth; i++){ */
	/* 	std::cout << '\t' << std::flush; */
	/* } */
	/* if( depth == MAX_DEPS) */
	/* std::cout << dev<<' '<< st->evalate(turn) << std::endl; */
	game_tree[h].evalation[turn] = dev;
	return dev;
}
Board_p adventure(int turn, int depth, Board_p& board) {
	int a = INT16_MIN+1;
	int b = INT16_MAX;
	
	int r = negamax(board, turn, depth, a, b);
	game_tree[board->hash()].evalation[turn] = r;
	return game_tree[board->hash()].best[turn];
}

