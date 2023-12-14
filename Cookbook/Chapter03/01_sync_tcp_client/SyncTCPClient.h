#pragma once

#include <string>
#include <boost/asio.hpp>

class SyncTCPClient {

public:

	SyncTCPClient(const std::string& raw_ip_address, unsigned short port_num) ;

	void connect() ;
	void close() ;

	std::string emulateLongComputationOp(unsigned int duration_sec) ;

private:

	void sendRequest(const std::string& request) ;
	std::string receiveResponse();

private:

	boost::asio::io_context m_io_ctx;
	boost::asio::ip::tcp::endpoint m_ep;
	boost::asio::ip::tcp::socket m_sock;
};

