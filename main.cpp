#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include "ai_core.hpp"

using namespace teyo_shogi;
extern Game_Tree game_tree;
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
		std::cout <<  "I am " 
			<< ((self==BLACK)?"first":"second") << std::endl;
		DobutsuAI ai;
		while(1){
	#if 1
			//wait while enemy turn
			std::cout << "waiting" << std::endl;
			do{
				usleep(3e5);
			} while(self != cmd.turn());
			
			//入力
			std::string input_board = cmd.board();
			base->write(input_board);
			if(ai.is_win(base, self^1)){
				std::cout << "LOSE (TAT)" << std::endl;
				break;
			}
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
			//探索
			pre_board = ai.adventure(base, self, MAX_DEPS);
			
			//コマンド送信
			if( !base->to_command(cmd, self, pre_board)){
				throw "move command error";
			}

			if(ai.is_win(pre_board, self)){
				std::cout << "WIN \\(^^)/" << std::endl;
				break;
			}
	#else
			while(cmd.turn() != self){
				usleep(3e3);
			}
			
			cmd.whoami();
			std::string input_board = cmd.board();
			base->write(input_board);
			/* if( is_win( base, self)) */
			/* 	std::cout << "checkmate ?" << std::endl; */
			/* else */
			/* 	std::cout << "not mate" << std::endl; */
			/* pre_board = adventure(self, MAX_DEPS , base); */
			cmd.whoami();
			while(cmd.turn() == self){
				usleep(3e3);
			}

#endif
		}
	}else if(argc == 4){
		DobutsuAI blackAI, whiteAI;
		Board_p pre_board(new Board);
		std::string init_str = std::string(INITBOARD);
		base->write(init_str);
		while(1){
			base->print();
			if( whiteAI.is_win(base, WHITE)){
				std::cout << "WHITE WIN" <<std::endl;
				break;
			}
			std::cout << "Black turn" << std::endl;
			base = blackAI.adventure(base, BLACK, MAX_DEPS);

			base->print();
			if( blackAI.is_win(base, BLACK)){
				std::cout << "BLACK WIN" <<std::endl;
				break;
			}
			std::cout << "White turn" << std::endl;
			base = whiteAI.adventure(base, WHITE, MAX_DEPS);
		}
	}else{
		
	}
	} catch(std::string e){
		std::cerr << e << std::endl;
	} catch( char const* e){
		std::cerr << e << std::endl;
	}catch (std::logic_error e){
		std::cerr << e.what() << std::endl;
	}
}

