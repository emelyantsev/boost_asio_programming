#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
	const int BACKLOG_SIZE = 30;

	unsigned short port_num = 3333;

	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

	boost::asio::io_context io_ctx;

	try {
		
		asio::ip::tcp::acceptor acceptor(io_ctx, ep.protocol());

		acceptor.bind(ep);

		acceptor.listen(BACKLOG_SIZE);

		asio::ip::tcp::socket sock(io_ctx);

		acceptor.accept(sock);

        std::cout << "Accept OK from " << sock.remote_endpoint().address().to_string() << " : " << sock.remote_endpoint().port() << std::endl;

	}
	catch (system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
}


