#include <iostream>
#include <boost/asio.hpp>
#include <unistd.h>

namespace asio = boost::asio;
using asio::ip::tcp;

void receive(tcp::socket &socket){
	asio::streambuf receive_buffer;
	boost::system::error_code error;

	asio::read(socket, receive_buffer, asio::transfer_at_least(1), error);

	const char* data = asio::buffer_cast<const char*>(receive_buffer.data());
	if (error && error != asio::error::eof) {
		std::cout << "receive failed: " << error.message() << std::endl;
	}
	else {
		const char* data = asio::buffer_cast<const char*>(receive_buffer.data());
		std::cout << data << std::endl;
	}
}
void send(tcp::socket& socket, std::string const& msg){
	boost::system::error_code error;
	asio::write(socket, asio::buffer(msg+"\n"), error);
	if (error) {
		std::cout << "send failed: " << error.message() << std::endl;
	}
	else {
		std::cout << "send correct!" << std::endl;
	}
}
int main()
{
	asio::io_service io_service;
	tcp::socket socket(io_service);

	boost::system::error_code error;
	socket.connect(tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 4444), error);

	if (error) {
		std::cout << "connect failed : " << error.message() << std::endl;
	}
	else {
		std::cout << "connected" << std::endl;
	}
	receive(socket);
	while(1){
		std::string msg;
		std::cin >> msg;
		send(socket, msg);
		usleep(1e4);
		receive(socket);
	}

}

