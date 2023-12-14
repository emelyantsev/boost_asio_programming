#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost;

int main( int argc, const char* argv[] )
{

	std::string host = argc == 1 ? "www.ya.ru" : argv[1] ;
	std::string port_num = "3333";

	boost::asio::io_context io_ctx;

	asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);

	asio::ip::tcp::resolver resolver(io_ctx);

	try {
		
		asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);

		asio::ip::tcp::socket sock(io_ctx);

		asio::connect(sock, it);
	}

	catch (system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
}

