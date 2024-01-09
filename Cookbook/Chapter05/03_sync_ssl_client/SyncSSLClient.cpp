#include "SyncSSLClient.h"
#include <iostream>


SyncSSLClient::SyncSSLClient(const std::string& raw_ip_address, unsigned short port_num) :

    m_ep(boost::asio::ip::address::from_string(raw_ip_address),	port_num),
    m_ssl_context(boost::asio::ssl::context::sslv23_client),
    m_ssl_stream(m_io_ctx, m_ssl_context)
{

    m_ssl_stream.set_verify_mode(boost::asio::ssl::verify_peer);
    
    m_ssl_stream.set_verify_callback( [this] ( bool preverified, boost::asio::ssl::verify_context& context) -> bool {

            return on_peer_verify(preverified, context);
        }
    );
}


void SyncSSLClient::connect() {

    m_ssl_stream.lowest_layer().connect(m_ep);
    m_ssl_stream.handshake( boost::asio::ssl::stream_base::client );
}


void SyncSSLClient::close() {

    boost::system::error_code ec;
    m_ssl_stream.shutdown( ec ); 

    m_ssl_stream.lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ec );
    m_ssl_stream.lowest_layer().close( ec );
}


std::string SyncSSLClient::emulate_long_computation_op( unsigned int duration_sec) {

    std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
    send_request(request);
    return receive_response();
}


bool SyncSSLClient::on_peer_verify(bool preverified, boost::asio::ssl::verify_context& context)
{
    return true; 
}


void SyncSSLClient::send_request(const std::string& request) {

    boost::asio::write( m_ssl_stream, boost::asio::buffer(request) );
}


std::string SyncSSLClient::receive_response() {

    boost::asio::streambuf buf;
    boost::asio::read_until( m_ssl_stream, buf, '\n' );

    std::string response;
    std::istream input(&buf);
    std::getline(input, response);

    return response;
}