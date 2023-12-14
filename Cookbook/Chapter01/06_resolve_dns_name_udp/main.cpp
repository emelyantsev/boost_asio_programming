#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main(int argc, const char* argv[] )
{
	std::string host = argc == 1 ? "samplehost.com" : argv[1] ;
	std::string port_num = "3333";

	boost::asio::io_context io_ctx;

	asio::ip::udp::resolver::query resolver_query(host, port_num, asio::ip::udp::resolver::query::numeric_service);

	asio::ip::udp::resolver resolver(io_ctx);

	boost::system::error_code ec;

	asio::ip::udp::resolver::iterator it = resolver.resolve(resolver_query, ec);


	if (ec.value() != 0) {

		std::cout << "Failed to resolve a DNS name." << "Error code = " << ec.value() << ". Message = " << ec.message() << std::endl;

		return ec.value();
	}

	asio::ip::udp::resolver::iterator it_end;

	for (; it != it_end; ++it) {

		asio::ip::udp::endpoint ep = it->endpoint();
        std::cout << host << " : " << ep.address().to_string() << std::endl;
	}

	return 0;
}
