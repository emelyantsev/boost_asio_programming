#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
	boost::asio::io_context io_context;

	asio::ip::tcp protocol = asio::ip::tcp::v4();

	try {

		asio::ip::tcp::socket sock(io_context, protocol);
	}
	catch (boost::system::system_error & e) {
		
		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
	}

	return 0;
}