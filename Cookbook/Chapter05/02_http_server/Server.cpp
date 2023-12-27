#include "Server.h"

Service::Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) :

		m_sock(sock),
		m_request(4096),
		m_response_status_code(200), // Assume success.
		m_resource_size_bytes(0)

	{};


void Service::start_handling() {

    boost::asio::async_read_until( *m_sock.get(), m_request, "\r\n",

            [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            on_request_line_received(ec, bytes_transferred);
        });
}



void Service::on_request_line_received( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) {

        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();

        if (ec == boost::asio::error::not_found) {

            m_response_status_code = 413;
            send_response();
            return;
        }

        else {

            on_finish();
            return;
        }
    }

    std::string request_line;
    std::istream request_stream(&m_request);
    std::getline(request_stream, request_line, '\r');

    request_stream.get(); // Remove symbol '\n' from the buffer.

    std::string request_method;
    std::istringstream request_line_stream(request_line);
    request_line_stream >> request_method;

    if (request_method.compare("GET") != 0) {

        m_response_status_code = 501;
        send_response();
        return;
    }

    request_line_stream >> m_requested_resource;
    std::string request_http_version;
    request_line_stream >> request_http_version;

    if (request_http_version.compare("HTTP/1.1") != 0) {

        m_response_status_code = 505;
        send_response();
        return;
    }

    boost::asio::async_read_until( *m_sock.get(), m_request, "\r\n\r\n",

        [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            on_headers_received(ec, bytes_transferred);
        });

    return;
}


void Service::on_headers_received(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) {

        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();

        if (ec == boost::asio::error::not_found) {

            m_response_status_code = 413;
            send_response();
            return;
        }
        else {

            on_finish();
            return;
        }
    }

    std::istream request_stream(&m_request);
    std::string header_name, header_value;

    while (!request_stream.eof()) {

        std::getline(request_stream, header_name, ':');

        if (!request_stream.eof()) {

            std::getline(request_stream, header_value, '\r');
            request_stream.get(); // Remove symbol \n from the stream.
            m_request_headers[header_name] = header_value;
        }
    }

    process_request();
    send_response();

    return;
}


void Service::process_request() {

    std::string resource_file_path = std::string("./www") + m_requested_resource;

    if (!boost::filesystem::exists(resource_file_path)) {

        m_response_status_code = 404;
        return;
    }

    std::ifstream resource_fstream( resource_file_path, std::ifstream::binary);

    if (!resource_fstream.is_open()) {

        m_response_status_code = 500;
        return;
    }

    resource_fstream.seekg(0, std::ifstream::end);
    m_resource_size_bytes = static_cast<std::size_t>(resource_fstream.tellg());
    m_resource_buffer.reset(new char[m_resource_size_bytes]);
    resource_fstream.seekg(std::ifstream::beg);
    resource_fstream.read(m_resource_buffer.get(), m_resource_size_bytes);
    m_response_headers += std::string("content-length") + ": " + std::to_string(m_resource_size_bytes) + "\r\n";
}


void Service::send_response()  {

    m_sock->shutdown( boost::asio::ip::tcp::socket::shutdown_receive);

    auto status_line = http_status_table.at(m_response_status_code);

    m_response_status_line = std::string("HTTP/1.1 ") + status_line + "\r\n";

    m_response_headers += "\r\n";

    std::vector<boost::asio::const_buffer> response_buffers;

    response_buffers.push_back( boost::asio::buffer(m_response_status_line));


    if (m_response_headers.length() > 0) {

        response_buffers.push_back( boost::asio::buffer(m_response_headers));
    }


    if (m_resource_size_bytes > 0) {

        response_buffers.push_back( boost::asio::buffer(m_resource_buffer.get(), m_resource_size_bytes));

    }

    boost::asio::async_write(*m_sock.get(), response_buffers,

        [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)

        {
            on_response_sent(ec, bytes_transferred);
        });
}


void Service::on_response_sent(const boost::system::error_code& ec, std::size_t bytes_transferred)
{

    if (ec.value() != 0) {

        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
    }

    m_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    on_finish();
}


void Service::on_finish() {

    delete this;
}



const std::map<unsigned int, std::string> Service::http_status_table = 

    {
        { 200, "200 OK" },
        { 404, "404 Not Found" },
        { 413, "413 Request Entity Too Large" },
        { 500, "500 Server Error" },
        { 501, "501 Not Implemented" },
        { 505, "505 HTTP Version Not Supported" }
    };


// ------------------- Acceptor --------------------- //

Acceptor::Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) :

		m_io_ctx(io_ctx),
		m_acceptor(m_io_ctx, boost::asio::ip::tcp::endpoint( boost::asio::ip::address_v4::any(), port_num)),
		m_isStopped( false )
	{}

void Acceptor::Start() {

    m_acceptor.listen();
    InitAccept();
}

void Acceptor::Stop() {

    m_isStopped.store(true);
}


void Acceptor::InitAccept() {

    std::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(m_io_ctx));

    m_acceptor.async_accept( *sock.get(),

        [this, sock]( const boost::system::error_code& error)
        {
            onAccept(error, sock);
        });
}


void Acceptor::onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock)
{
    if (ec.value() == 0) {

        ( new Service(sock) )->start_handling();
    }
    else {

        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message();
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


void Server::Start(unsigned short port_num,

    unsigned int thread_pool_size) {
    assert(thread_pool_size > 0);

    acc.reset(new Acceptor(m_io_ctx, port_num));
    acc->Start();

    for (unsigned int i = 0; i < thread_pool_size; i++) {

        std::unique_ptr<std::thread> th(

            new std::thread([this]()
            {
                m_io_ctx.run();
            }));

        m_thread_pool.push_back(std::move(th));
    }
}

void Server::Stop() {

    acc->Stop();
    m_io_ctx.stop();

    for (auto& th : m_thread_pool) {

        th->join();
    }
}
