#pragma once

#include <string>
#include <boost/asio.hpp>


class SyncUDPClient {

public:

	SyncUDPClient() ;

	std::string emulateLongComputationOp( unsigned int duration_sec, const std::string& raw_ip_address, unsigned short port_num) ;

private:

	void sendRequest(const boost::asio::ip::udp::endpoint& ep, const std::string& request) ;

	std::string receiveResponse(boost::asio::ip::udp::endpoint& ep) ;

private:

	boost::asio::io_context m_io_ctx;
	boost::asio::ip::udp::socket m_sock;
};
