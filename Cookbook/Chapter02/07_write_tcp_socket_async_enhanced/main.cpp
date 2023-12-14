#include <boost/asio.hpp>

#include <iostream>
#include <string>

using namespace boost;

struct Session {

	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::string buf;
};

void callback(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s)
{
	if (ec.value() != 0) {
		
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();

		return;
	}

}

void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {

	std::shared_ptr<Session> s(new Session);

	s->buf = std::string("Hello!\n");
	s->sock = sock;


    asio::async_write( *s->sock.get(),  asio::buffer(s->buf), std::bind(callback,
		std::placeholders::_1,
		std::placeholders::_2,
		s) )  ;
}

int main()
{
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {

		asio::ip::tcp::endpoint  ep( asio::ip::address::from_string(raw_ip_address), port_num );

		boost::asio::io_context io_ctx;

		std::shared_ptr<asio::ip::tcp::socket> sock( new asio::ip::tcp::socket( io_ctx, ep.protocol() ) );

		sock->connect(ep);

		writeToSocket(sock);

		io_ctx.run();
	}
	catch (system::system_error& e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();

		return e.code().value();
	}

	return 0;
}