#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost;

int main()
{
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {
		
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
		
        boost::asio::io_context io_ctx;
		
        asio::ip::tcp::socket sock(io_ctx, ep.protocol());

		sock.connect(ep);
	}
	catch (system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;
		return e.code().value();
	}

	return 0;
}
