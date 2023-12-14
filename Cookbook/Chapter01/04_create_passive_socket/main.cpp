#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
	
	boost::asio::io_context io_ctx;

	asio::ip::tcp protocol = asio::ip::tcp::v6();
	asio::ip::tcp::acceptor acceptor( io_ctx );

	boost::system::error_code ec;

	acceptor.open(protocol, ec);

	if (ec.value() != 0) {

		std::cout << "Failed to open the acceptor socket!" << "Error code = " << ec.value() << ". Message: " << ec.message();
		
        return ec.value();
	}

	return 0;
}
