#include <stdexcept>
#include <iostream>
#include <unistd.h>
#define MAX_DEPS 8
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
		while(1){
	#if 1
			//wait while enemy turn
			do{
				usleep(3e5);
			} while(self != cmd.turn());
			
			//入力
			std::string input_board = cmd.board();
			base->write(input_board);

			std::cout 
				<< (winable_check(base, self)?"win":"not win") 
				<< std::endl; 
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
				<< evalate(base, self)<< std::endl; 
			//現在の状態の表示
			int ntrn = (self==BLACK)?WHITE:BLACK;
			//探索
			pre_board = adventure(base, self, MAX_DEPS);
			pre_board->print();
			
			//コマンド送信
			if( !base->to_command(cmd, self, pre_board)){
				throw "move command error";
			}
	#else
			while(cmd.turn() != self){
				usleep(3e3);
			}
			
			cmd.whoami();
			std::string input_board = cmd.board();
			base->write(input_board);
			pre_board = adventure(self, MAX_DEPS , base);
			cmd.whoami();
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
			std::cout << "Black turn" << std::endl;
			if( winable_check(base, BLACK)){
				std::cout << "end" <<std::endl;
				break;
			}

			base = adventure(base, BLACK, 3);
			/* e = evalate( WHITE, base); */
			base->print();
			std::cout << "White turn" << std::endl;
			//TODO:千日手カウント
			if( winable_check(base, WHITE)){
				std::cout << "end" <<std::endl;
				break;
			}
			base = adventure(base, WHITE, MAX_DEPS);
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

