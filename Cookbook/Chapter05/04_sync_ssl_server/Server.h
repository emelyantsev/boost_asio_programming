#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class Service {

public:

	Service() {}
	void handle_client( boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& ssl_stream);
};


class Acceptor {

public:

	Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) ;
	void accept() ;

private:

	std::string get_password(std::size_t max_length, boost::asio::ssl::context::password_purpose purpose) const ;

private:

	boost::asio::io_context& m_io_ctx;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::ssl::context m_ssl_context;
};


class Server {

public:

	Server() ;

	void start(unsigned short port_num) ;
	void stop() ;

private:

	void run(unsigned short port_num) ;

	std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_stop;
	boost::asio::io_context m_io_ctx;
};



