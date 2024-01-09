#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

class SyncSSLClient {

public:

	SyncSSLClient(const std::string& raw_ip_address, unsigned short port_num) ;

	void connect() ;
	void close() ;

	std::string emulate_long_computation_op( unsigned int duration_sec) ;

private:

	bool on_peer_verify(bool preverified, boost::asio::ssl::verify_context& context);
	void send_request(const std::string& request) ;
	std::string receive_response() ;

private:

	boost::asio::io_context m_io_ctx;
	boost::asio::ip::tcp::endpoint m_ep;
	boost::asio::ssl::context m_ssl_context;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_ssl_stream;
};