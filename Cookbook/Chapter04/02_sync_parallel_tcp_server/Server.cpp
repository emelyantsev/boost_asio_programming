#include "Server.h"

#include <thread>
#include <string>
#include <iostream>



void print_buf(boost::asio::streambuf& buf) {

    std::istream input(&buf);
    std::string message1;
    std::getline(input, message1);

    std::cout << message1 << std::endl;
}


Service::Service() {}


void Service::StartHandlingClient( std::shared_ptr<boost::asio::ip::tcp::socket> sock) {

    std::thread th( ( [this, sock]() {

            HandleClient(sock);

        }));

    th.detach();
}

void Service::HandleClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock) {

    try {

        boost::asio::streambuf request;

        boost::asio::read_until(*sock.get(), request, '\n');

        // Emulate request processing.

        int i = 0;

        while (i != 1000000)

            i++;


        std::this_thread::sleep_for( std::chrono::milliseconds(500));

        print_buf(request);

        std::string response = "Response\n";

        boost::asio::write(*sock.get(), boost::asio::buffer(response));

    }

    catch (boost::system::system_error &e) {

        std::cout << "Error occured! Error code = " << e.code() << ". Message: " << e.what() << std::endl;
    }

    delete this;
}



Acceptor::Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) :

    m_io_ctx(io_ctx),
    m_acceptor( m_io_ctx, boost::asio::ip::tcp::endpoint( boost::asio::ip::address_v4::any(), port_num))
{

    m_acceptor.listen();

}


void Acceptor::Accept() {

    std::shared_ptr<boost::asio::ip::tcp::socket> sock( new boost::asio::ip::tcp::socket( m_io_ctx ) );

    m_acceptor.accept(*sock.get());

    (new Service)->StartHandlingClient(sock);
}



Server::Server() : m_stop(false) {}


void Server::Start(unsigned short port_num) {

    m_thread.reset(new std::thread([this, port_num]() {

        Run(port_num);
    }));
}


void Server::Stop() {

    m_stop.store(true);
    m_thread->join();
}


void Server::Run(unsigned short port_num) {

    Acceptor acc(m_io_ctx, port_num);

    while (!m_stop.load()) {

        acc.Accept();
    }
}