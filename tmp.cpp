#include <iostream>
#include <unistd.h>
#include "command.hpp"
int main(int argc, char* argv[])
{
	try{
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::socket socket(io_service);
		teyo_shogi::Command cmd(socket);
		int self = cmd.connect(NULL, 4444);
		while(1){
			while( self != cmd.turn()){
				std::cout << "not my turn" << std::endl;
				usleep(1e5);
			}
			if( !cmd.move("B4","A3"))
				cmd.move("A3","B4");
		}
	} catch(std::string e){
		std::cerr << e << std::endl;
	}

}

