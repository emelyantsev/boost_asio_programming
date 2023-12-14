#include <boost/asio.hpp>
#include <iostream>

using namespace boost;


void print_buf(asio::streambuf& buf) {

    std::istream input(&buf);
    std::string message1;
    std::getline(input, message1);

    std::cout << message1 << std::endl;
}

void communicate(asio::ip::tcp::socket& sock) {
	
	const char request_buf[] = { 0x48, 0x65, 0x0, 0x6c, 0x6c, 0x6f };

	asio::write(sock, asio::buffer(request_buf));

	sock.shutdown(asio::socket_base::shutdown_send);

	asio::streambuf response_buf;

	system::error_code ec;
	asio::read(sock, response_buf, ec);

	if (ec == asio::error::eof) {

        std::cout << "Got eof" << std::endl;
        std::cout << response_buf.size() << std::endl;
        print_buf(response_buf);
	}
	else {
		throw system::system_error(ec);
	}
}

int main()
{
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {
		asio::ip::tcp::endpoint ep( asio::ip::address::from_string( raw_ip_address ), port_num );

		boost::asio::io_context io_ctx;

		asio::ip::tcp::socket sock( io_ctx, ep.protocol() );

		sock.connect(ep);

		communicate(sock);
	}
	catch (system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
}
