#include "Server.h"

#include <boost/current_function.hpp>


Service::Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) : m_sock( sock ) {}


void Service::StartHandling() {

    print_thread_id(BOOST_CURRENT_FUNCTION);

    boost::asio::async_read_until( *m_sock.get(), m_request, '\n',

        [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            onRequestReceived(ec, bytes_transferred);
        } 
    );
}


void Service::onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) {

    if (ec.value() != 0) {

        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
        onFinish();
        return;
    }

    m_response = ProcessRequest(m_request);

    boost::asio::async_write( *m_sock.get(), boost::asio::buffer(m_response),

        [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
        onResponseSent(ec, bytes_transferred);
    });

}


void Service::onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred) {

    if (ec.value() != 0) {

        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }

    print_thread_id(BOOST_CURRENT_FUNCTION);
    onFinish();
}


void Service::onFinish() {

    delete this;
}


std::string Service::ProcessRequest(boost::asio::streambuf& request) {

    print_thread_id(BOOST_CURRENT_FUNCTION);
    print_buf(request);

    int i = 0;

    while (i != 1000000)
        i++;

    std::this_thread::sleep_for( std::chrono::milliseconds(100) );

    std::string response = "Response\n";

    return response;
}


void Service::print_buf(boost::asio::streambuf& buf) {

    std::istream input(&buf);
    std::string message1;
    std::getline(input, message1);

    std::cout << message1 << std::endl;
}

void Service::print_thread_id(const std::string& message) {

    std::cout << message << " " << std::this_thread::get_id() << std::endl;
}


Acceptor::Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) :
    m_io_ctx(io_ctx),
    m_acceptor(io_ctx, boost::asio::ip::tcp::endpoint( boost::asio::ip::address_v4::any(), port_num) ),
    m_isStopped(false)
{}


void Acceptor::Start() {

    m_acceptor.listen();
    InitAccept();
}


void Acceptor::Stop() {

    m_isStopped.store(true);
}


void Acceptor::InitAccept() {

    std::shared_ptr<boost::asio::ip::tcp::socket> sock( new boost::asio::ip::tcp::socket(m_io_ctx) );

    m_acceptor.async_accept( *sock.get(),

        [this, sock]( const boost::system::error_code& error)
    {
        onAccept(error, sock);
    });

}


void Acceptor::onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
    if (ec.value() == 0) {

        ( new Service(sock) )->StartHandling();
    }
    else {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
    }

    if (!m_isStopped.load()) {

        InitAccept();
    }
    else {
        m_acceptor.close();
    }
}



Server::Server() {

    m_work_guard.reset( new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>( m_io_ctx.get_executor() ) ) ;
}


void Server::Start(unsigned short port_num, unsigned int thread_pool_size) {

    assert(thread_pool_size > 0);

    acc.reset(new Acceptor(m_io_ctx, port_num));
    acc->Start();

    for (unsigned int i = 0; i < thread_pool_size; i++) {

        m_thread_pool.create_thread( [this] () { m_io_ctx.run(); } ) ;
    }
}

void Server::Stop() {

    acc->Stop();
    m_io_ctx.stop();
    m_thread_pool.join_all();
}