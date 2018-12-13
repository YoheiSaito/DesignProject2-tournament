#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include "Board.hpp"
#include "game_node.hpp"
#include "Command.hpp"
#define MAX_DIPS 6
using namespace teyo_shogi;
/* typedef std::unordered_map<Board_p, Game_Node> Game_Tree; */
typedef std::unordered_map<size_t, Game_Node> Game_Tree;

Board_p adventure(int turn, int depth, Board_p& board);
int16_t evalate(int turn, Board_p& board);
bool winable_check(Board_p&, int);

namespace{
	// treeに入っていないものはフロンティア
	// BLACK と WHITEで別々のツリーを持っているように見せる
	// ただし参照先は互いとなる.
	/* Game_Tree is map that connect Board Hash to Game_Node  */
	Game_Tree game_tree[2];
}

int main(int argc, char* argv[])
{
	try{
		Board_p base(new Board);
	if( argc < 3){
		Board_p pre_board = nullptr;
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket(io_service);
		Command cmd(socket);
		char* ip = argc > 1? (argv[1]):(NULL);
		int port = argc > 2? std::stoi(argv[2]):(4444);
		int self = cmd.connect(ip, port);
		while(1){
			while( self != cmd.turn()){
				usleep(3e5);
			}
			//入力
			std::string input_board = cmd.board();
			/* std::cout <<input_board<<std::endl; */
			base->write(input_board);
			//合法性チェック
			if(pre_board != nullptr){
				if(!pre_board->check_legitimacy(*base, self)){
					std::cerr 
					<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					<< "!!! board might be illigal !!!"
					<< "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					<<std::endl;
				}
			}
			//現在の状態の表示
			base->print();
			int e = evalate( self, base);
			Move_t bs = base->generate_move(self);
			int ntrn = (self==BLACK)?WHITE:BLACK;
			for(auto&& i : bs){
				i->print();
				std::cout << evalate( ntrn, i) << std::endl;
			}
			if( ((e<0)?-e:e) > 15000){
				std::cout << "end" <<std::endl;
				break;
			}
			//探索
			pre_board = adventure(self, MAX_DIPS , base);
			/* std::cout<< mv.size() << std::endl; */
			//コマンド送信
			if( !base->to_command(cmd, self, pre_board)){
				std::cerr << "couldn\'t move" << std::endl;
				std::cerr << "!!To-ryo (by move error)!!"
					<< std::endl;
				break;
			}
			
		}
	}else{
		int e;
		Board_p pre_board(new Board);
		std::string init_str = std::string(INITBOARD);
		base->write(init_str);
		while(1){
			base->print();
			e = evalate( BLACK, base);
			std::cout << e << std::endl;
			if( ((e<0)?-e:e) > 15000){
				std::cout << "end" <<std::endl;
				break;
			}
			base = adventure(BLACK, MAX_DIPS, base);
			base->print();
			e = evalate( WHITE, base);
			std::cout << e << std::endl;
			if( ((e<0)?-e:e) > 15000){
				std::cout << "end" <<std::endl;
				break;
			}
			base = adventure(WHITE, MAX_DIPS, base);
		}
	}
	} catch(std::string e){
		std::cerr << e << std::endl;
	} catch( char const* e){
		std::cerr << e << std::endl;
	}
}
bool winable_check(Board_p& x, int self){
	int trh = (self==BLACK)?0:BOARD_HEIGHT-1;
	if(x->caps[self][0].type == LION){
		return true;
	}else{
		for(int i = 0; i < BOARD_WIDTH; i++){
			Koma self_lion;
			self_lion.player = self;
			self_lion.type = LION;
			if( x->field[i][trh].data == self_lion.data){
				if( !x->is_check(i, trh, self^1))
					return true;
				else
					return false;
			}
		}
	}
	return false;
}
//
//	@retval next board

int16_t evalate(int turn, Board_p& board){
	int16_t r = 0;
	if(winable_check(board, turn)){
		return 30000;
	}
	static int16_t Koma_point[] = {
		0, 400, 250, 130, 100, 10000
	};
	for(int i = 0; i < BOARD_WIDTH; i++){
		for(int j = 0; j < BOARD_HEIGHT; j++){
			if( board->field[i][j].player == BLACK){
				r += Koma_point[board->field[i][j].type];
			}else{
				r -= Koma_point[board->field[i][j].type];
			}
		}
	}
	for( int i = 0; i < MAX_CAPS; i++){
		r += Koma_point[board->caps[BLACK][i].type];
		r -= Koma_point[board->caps[WHITE][i].type];
	}
	return (turn==BLACK)?r:-r;
}
Board_p adventure(int turn, int depth, Board_p& board) {
	int eva_max = -32767;
	int ntrn = (turn==BLACK)?WHITE:BLACK;
	if( depth == 0){
		Game_Node n;
		n.evalation = evalate(turn, board);
		game_tree[turn][board->hash()] = n;
		return board;
	}
	Move_t next = board->generate_move(turn);
	int n = next.size();
	if( n == 0){
		Game_Node n;
		n.evalation = evalate(turn, board);
		game_tree[turn][board->hash()] = n;
		return board;
	}

	for( auto&& i: next){
		adventure(ntrn, depth-1, i);
		/* int tmp_eval = game_tree[ntrn][i->hash()].evalation; */
	}
	int ind = 0;
	for( int i = 0; i < n; i++){
		size_t h = next[i]->hash();
		int e = -game_tree[ntrn][h].evalation;
		if( e > eva_max){
			eva_max = e;
			ind = i;
		}
	}
	game_tree[turn][board->hash()].evalation = eva_max;
	return next[ind];
}

