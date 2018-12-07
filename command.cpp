#include "command.hpp"
namespace teyo_shogi{

//Constructor
Command::Command ()
{
	boost::system::error_code error;
	socket = new tcp::socket(io_service);
	socket->connect(tcp::endpoint(
			asio::ip::address::from_string("127.0.0.1"), 4444
	), error);
	if (error) {
		std::cerr << 
			"connect failed : " << 
			error.message() <<
		std::endl;
		throw error.message();
	}

}
Command::Command (char* ip, int port)
{
	boost::system::error_code error;
	socket = new tcp::socket(io_service);
	socket->connect(tcp::endpoint(
			asio::ip::address::from_string(""), port
	), error);
	if (error) {
		std::cerr << 
			"connect failed : " << 
			error.message() <<
		std::endl;
		throw error.message();
	}

}

// Private Method 
std::string Command::receive(){
	asio::streambuf receive_buffer;
	boost::system::error_code error;

	asio::read(*socket, receive_buffer, asio::transfer_at_least(1), error);

	const char* data = 
		asio::buffer_cast<const char*>(receive_buffer.data());

	if (error && error != asio::error::eof) {
		std::cerr << "receive failed: " << error.message() << std::endl;
		throw error.message();
	}
	return asio::buffer_cast<std::string>(receive_buffer.data());
}
void Command::send(std::string const& msg){
	boost::system::error_code error;
	asio::write(*socket, asio::buffer(msg+"\n"), error);
	if (error) {
		std::cerr << "send failed: " << error.message() << std::endl;
		throw error.message();
	}
}


// Public Method

bool Command::move(std::string src, std::string dst){
	send("mv "+src+" "+dst);
	usleep(1e4);
	std::string rec = receive();
	return true;
}

std::string Command::board(){
	send("board");
	usleep(1e4);
	return receive();
}
std::string Command::initboard(){
	send("initboard");
	usleep(1e4);
	return receive();

}
int Command::whoami(){
	send("whoami");
	usleep(1e4);
	std::string who = receive();
	return -1;
}
int Command::turn(){
	send("turn");
	usleep(1e4);
	std::string t = receive();
	return -1;
}

} // teyo_shogi

