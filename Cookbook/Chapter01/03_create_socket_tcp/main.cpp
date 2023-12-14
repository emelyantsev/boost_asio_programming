#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
	boost::asio::io_context io_context;

	asio::ip::tcp protocol = asio::ip::tcp::v4();

	asio::ip::tcp::socket sock(io_context);

	boost::system::error_code ec;

	sock.open(protocol, ec);

	if (ec.value() != 0) {
		
		std::cout << "Failed to open the socket! Error code = " << ec.value() << ". Message: " << ec.message();
		
		return ec.value();
	}
	else {

		std::cout << "Local port is " << sock.local_endpoint().port() << std::endl;
	}
	


	return 0;
}