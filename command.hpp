#include <iostream>
#include <boost/asio.hpp>
#include <unistd.h>

namespace teyo_shogi{

namespace asio = boost::asio;
using asio::ip::tcp;
class Command{
	public:
	Command();
	Command(char* ip, int port=4444);
	bool move(std::string src, std::string dst);
	std::string board();
	std::string initboard();
	int whoami();
	int turn();
	
	private:
	const int interval = 1e4;
	asio::io_service io_service;
	tcp::socket* socket;

	std::string receive();
	void send(std::string const& msg);
	
};
};
