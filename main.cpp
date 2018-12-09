#include <iostream>
#include <unistd.h>
#include "Board.hpp"
#include "Command.hpp"
int main(int argc, char* argv[])
{
	try{
		
		teyo_shogi::Board base;
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket(io_service);
		teyo_shogi::Command cmd(socket);
		int self = cmd.connect(NULL, 4444);
		while(1){
			while( self != cmd.turn()){
				usleep(1e5);
			}
			std::string input_board = cmd.board();
			base.write(input_board);
			if( !cmd.move("B4","A3"))
				cmd.move("A3","B4");
			base.print();
		}
	} catch(std::string e){
		std::cerr << e << std::endl;
	}

}

