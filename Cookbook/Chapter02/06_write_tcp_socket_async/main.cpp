#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>

using namespace boost;

struct Session {
	std::shared_ptr<asio::ip::tcp::socket> sock;
	std::string buf;
	std::size_t total_bytes_written;
};

void callback(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<Session> s)
{
	if (ec.value() != 0) {

		std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
		return;
	}

	s->total_bytes_written += bytes_transferred;

	if (s->total_bytes_written == s->buf.length()) {

        std::cout << "All bytes have been written OK" << std::endl;
		return;
	}

	s->sock->async_write_some(

		asio::buffer(
            s->buf.c_str() +
            s->total_bytes_written,
            s->buf.length() -
            s->total_bytes_written),
            std::bind(callback, std::placeholders::_1,
            std::placeholders::_2, s)
        );
}

void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {

	std::shared_ptr<Session> s = std::make_shared<Session>();

	s->buf = std::string("Hello\n");
	s->total_bytes_written = 0;
	s->sock = sock;

	s->sock->async_write_some(

		asio::buffer(s->buf),
		std::bind(callback,
		std::placeholders::_1,
		std::placeholders::_2,
		s));
}

void connect_handler(const boost::system::error_code& error, std::shared_ptr<asio::ip::tcp::socket> ptr_sock) {

    if (error) {
        return;
    }

    writeToSocket(ptr_sock);
}

int main()
{
	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	try {

		asio::ip::tcp::endpoint ep( asio::ip::address::from_string(raw_ip_address), port_num );

		boost::asio::io_context io_ctx;

		std::shared_ptr<asio::ip::tcp::socket> sock( new asio::ip::tcp::socket( io_ctx, ep.protocol() ) );

        sock->async_connect( ep, 

            [sock](const boost::system::error_code& error ) {

                if (!error) {

                    writeToSocket(sock);
                }
                else {

                    std::cout << "Got error! Error code = " << error.value() << ". Message: " << error.message() << std::endl;

                    return ;
                }
            } 

           // std::bind(connect_handler, std::placeholders::_1, sock) 

        );

         
        //sock->connect(ep);
		//writeToSocket(sock);

		io_ctx.run();
	}
	catch (system::system_error &e) {

		std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;

		return e.code().value();
	}

	return 0;
}
