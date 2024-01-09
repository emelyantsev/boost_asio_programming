#include "Server.h"

#include <iostream>
#include <thread>

void Service::handle_client( boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& ssl_stream)
{

    try {

        ssl_stream.handshake( boost::asio::ssl::stream_base::server);

        boost::asio::streambuf request;
        boost::asio::read_until(ssl_stream, request, '\n');

        int i = 0;

        while (i != 1000000)

            i++;

        std::this_thread::sleep_for( std::chrono::milliseconds(500));
        std::string response = "Response\n";
        boost::asio::write(ssl_stream, boost::asio::buffer(response));
    }

    catch (boost::system::system_error& e) {

        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what();
    }
}



Acceptor::Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) :

		m_io_ctx(io_ctx),
		m_acceptor(m_io_ctx, boost::asio::ip::tcp::endpoint( boost::asio::ip::address_v4::any(), port_num)),
		m_ssl_context(boost::asio::ssl::context::sslv23_server)
{

    m_ssl_context.set_options( boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::single_dh_use );

    m_ssl_context.set_password_callback(

        [this](std::size_t max_length, boost::asio::ssl::context::password_purpose purpose) -> std::string

            {return get_password(max_length, purpose); }
    );

    m_ssl_context.use_certificate_chain_file("example.crt");
    m_ssl_context.use_private_key_file("example.key", boost::asio::ssl::context::pem);
    m_ssl_context.use_tmp_dh_file("dhparams.pem");
    m_acceptor.listen();
}


void Acceptor::accept() {

    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_stream(m_io_ctx, m_ssl_context);
    m_acceptor.accept(ssl_stream.lowest_layer());

    Service svc;
    svc.handle_client(ssl_stream);
}

std::string Acceptor::get_password(std::size_t max_length, boost::asio::ssl::context::password_purpose purpose) const
{
    return "pass";
}


Server::Server() : m_stop(false) {}


void Server::start(unsigned short port_num) {

    m_thread.reset(new std::thread([this, port_num]() {

            run(port_num);
        }
    ) );
}

void Server::stop() {

    m_stop.store(true);
    m_thread->join();
}

void Server::run(unsigned short port_num) {

    Acceptor acc(m_io_ctx, port_num);

    while (!m_stop.load()) {

        acc.accept();
    }
}