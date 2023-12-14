#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{

	unsigned short port_num = 3333;

	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

	boost::asio::io_context io_ctx;

	asio::ip::tcp::acceptor acceptor( io_ctx, ep.protocol() );

	boost::system::error_code ec;

	acceptor.bind(ep, ec);

	if (ec.value() != 0) {

		std::cout << "Failed to bind the acceptor socket." << "Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
		return ec.value();
	}
    else {
        std::cout << "Bind OK : press <Enter> to continue" << std::endl;
        ::getchar();
    }


	return 0;
}
