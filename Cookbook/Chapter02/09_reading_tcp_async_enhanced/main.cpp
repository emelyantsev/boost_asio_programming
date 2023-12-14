#include <boost/asio.hpp>
#include <iostream>
#include <string_view>

using namespace boost;

struct Session {

	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::unique_ptr<char[]> buf;
	unsigned int buf_size;
};

void callback(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s)
{
	if (ec.value() != 0) {

		std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
		return;
	}
    else {

        std::cout << "[" << bytes_transferred << "]" << "\"" <<  std::string_view( s->buf.get(), s->buf_size ) << "\"" << std::endl;
    }
}

void readFromSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {

	std::shared_ptr<Session> s(new Session);

	const unsigned int MESSAGE_SIZE = 7;

	s->buf.reset(new char[MESSAGE_SIZE]);

	s->sock = sock;
	s->buf_size = MESSAGE_SIZE;


    asio::async_read( *s->sock.get(),  asio::buffer(s->buf.get(), s->buf_size), 
                    std::bind(callback, std::placeholders::_1, std::placeholders::_2, s) );
    
}

int main()
{
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {

		asio::ip::tcp::endpoint  ep(asio::ip::address::from_string(raw_ip_address), port_num);

		boost::asio::io_context io_ctx;

		std::shared_ptr<asio::ip::tcp::socket> sock( new asio::ip::tcp::socket(io_ctx, ep.protocol() ) );

		sock->connect(ep);

		readFromSocket(sock);

		io_ctx.run();
	}
	catch (system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
}