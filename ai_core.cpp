#include <iostream>
#include <unistd.h>
#include <unordered_map>
#include "Board.hpp"
#include "game_node.hpp"
#include "Command.hpp"

using namespace teyo_shogi;
typedef std::unordered_map<Board,Game_Node, Board::Hash> Game_Tree;
void adventure(int turn, int depth, Game_Tree& tree, Board& board);
int main(int argc, char* argv[])
{
	try{
		Board base;
		// treeに入っていないものはフロンティア
		Game_Tree tree;
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket(io_service);
		Command cmd(socket);
		char* ip = argc > 1? (argv[1]):(NULL);
		int port = argc > 2? std::stoi(argv[2]):(4444);
		int self = cmd.connect(ip, port);
		while(1){
			while( self != cmd.turn()){
				usleep(1e5);
			}
			std::string input_board = cmd.board();
			std::cout <<input_board<<std::endl;
			base.write(input_board);
			/* if( tree.find(base) != tree.end()){ */
			/* 	// 4 手先までとりあえず読む */
			/* 	adventure(4, base, tree , self); */
			/* } */
			base.print();
			std::cout << "先手の利き"<< std::endl;
			for( int i = 0; i < BOARD_HEIGHT; i ++){
				for( int j = 0; j < BOARD_WIDTH; j++){
					std::cout << base.is_check(j, i, 0) 
						<< std::flush;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			std::cout << "後手の利き"<< std::endl;
			for( int i = 0; i < BOARD_HEIGHT; i ++){
				for( int j = 0; j < BOARD_WIDTH; j++){
					std::cout << base.is_check(j, i, 1) 
						<< std::flush;
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
			/* if( !cmd.move("B4","A3")){ */
				/* cmd.move("A3","B4"); */
				/* std::cerr << "cant move" << std::endl; */
			/* } */
			
		}
	} catch(std::string e){
		std::cerr << e << std::endl;
	}

}
void adventure( int turn, int depth, Game_Tree& tree, Board& board){
	
}
