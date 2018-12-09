#include <iostream>
#include <boost/asio.hpp>

namespace teyo_shogi{
class Command{
	private:
		boost::asio::ip::tcp::socket& socket;
	public:
	Command(boost::asio::ip::tcp::socket & sock);
	int connect(char* ip, int port);
	private:
	std::string receive();
	void send(std::string const& msg);
	public:
	bool move(std::string const& src, std::string const& dst);
	std::string board();
	std::string board2();
	std::string initboard();
	int whoami();
	int turn();
};
};
