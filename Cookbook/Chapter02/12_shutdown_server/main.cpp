#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost;

void print_buf(asio::streambuf& buf) {

    std::istream input(&buf);
    std::string message1;
    std::getline(input, message1);

    std::cout << message1 << std::endl;
}

void processRequest(asio::ip::tcp::socket& sock) {

	asio::streambuf request_buf;

	system::error_code ec;

    asio::read(sock, request_buf, ec);

	if (ec != asio::error::eof)
		throw system::system_error(ec);

    print_buf(request_buf);

	const char response_buf[] = { 0x48, 0x69, 0x21 };

	asio::write(sock, asio::buffer(response_buf));

	sock.shutdown(asio::socket_base::shutdown_send);
}

int main()
{
	unsigned short port_num = 3333;
	
	try {

		asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

		boost::asio::io_context io_ctx;

		asio::ip::tcp::acceptor acceptor(io_ctx, ep);

		asio::ip::tcp::socket sock(io_ctx);

		acceptor.accept(sock);

		processRequest(sock);
	}
	catch (system::system_error& e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
}
