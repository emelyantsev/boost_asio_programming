#include "HttpTools.h"

namespace http_errors {

    const char* http_errors_category::name() const BOOST_SYSTEM_NOEXCEPT { return "http_errors"; }

    std::string http_errors_category::message(int e) const {

        switch (e) {

        case invalid_response:

            return "Server response cannot be parsed.";
            break;

        default:

            return "Unknown error.";
            break;
        }
    }

    const boost::system::error_category& get_http_errors_category()
    {
        static http_errors_category cat;
        return cat;
    }

    boost::system::error_code make_error_code(http_error_codes e)
    {
        return boost::system::error_code( static_cast<int>(e), get_http_errors_category() );
    }

} // namespace http_errors



// -------------------------- Http response -------------------------- //

HTTPResponse::HTTPResponse() : m_response_stream(&m_response_buf) {}


unsigned int HTTPResponse::get_status_code() const {

    return m_status_code;
}

const std::string& HTTPResponse::get_status_message() const {

    return m_status_message;
}

const std::map<std::string, std::string>& HTTPResponse::get_headers() {

    return m_headers;
}

const std::istream& HTTPResponse::get_response() const {

    return m_response_stream;
}

boost::asio::streambuf& HTTPResponse::get_response_buf() {

    return m_response_buf;
}

void HTTPResponse::set_status_code(unsigned int status_code) {

    m_status_code = status_code;
}

void HTTPResponse::set_status_message(const std::string& status_message) {

    m_status_message = status_message;
}


void HTTPResponse::add_header(const std::string& name, const std::string& value) {

    m_headers[name] = value;
}


// --------------- http request -------------------------  //

HTTPRequest::HTTPRequest(boost::asio::io_context& io_ctx, unsigned int id) :

		m_port(DEFAULT_PORT),
		m_id(id),
		m_callback(nullptr),
		m_sock(io_ctx),
		m_resolver(io_ctx),
		m_was_cancelled(false),
		m_io_ctx(io_ctx)
	{}


void HTTPRequest::set_host(const std::string& host) {

    m_host = host;
}

void HTTPRequest::set_port(unsigned int port) {

    m_port = port;
}

void HTTPRequest::set_uri(const std::string& uri) {

    m_uri = uri;
}

void HTTPRequest::set_callback(Callback callback) {

    m_callback = callback;
}

std::string HTTPRequest::get_host() const {

    return m_host;
}

unsigned int HTTPRequest::get_port() const {

    return m_port;
}

const std::string& HTTPRequest::get_uri() const {

    return m_uri;
}

unsigned int HTTPRequest::get_id() const {

    return m_id;
}


void HTTPRequest::execute() {

    assert(m_port > 0);
    assert(m_host.length() > 0);
    assert(m_uri.length() > 0);
    assert(m_callback != nullptr);

    boost::asio::ip::tcp::resolver::query resolver_query(m_host, std::to_string(m_port), boost::asio::ip::tcp::resolver::query::numeric_service);

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {

        cancel_lock.unlock();
        on_finish(boost::system::error_code( boost::asio::error::operation_aborted));
        return;
    }

    m_resolver.async_resolve(resolver_query, 
    
        [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
        {
            on_host_name_resolved(ec, iterator);

        }
    );
}


void HTTPRequest::cancel() {

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    m_was_cancelled = true;
    m_resolver.cancel();

    if (m_sock.is_open()) {
        m_sock.cancel();
    }
}

void HTTPRequest::on_host_name_resolved( const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
{
    if (ec.value() != 0) {

        on_finish(ec);
        return;
    }

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {

        cancel_lock.unlock();
        on_finish(boost::system::error_code( boost::asio::error::operation_aborted) );
        return;
    }

    boost::asio::async_connect(m_sock, iterator,
            [this](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
            {
                on_connection_established(ec, iterator);
            }
        );
}


void HTTPRequest::on_connection_established( const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator)
{
    if (ec.value() != 0) {

        on_finish(ec);
        return;
    }

    m_request_buf += "GET " + m_uri + " HTTP/1.1\r\n";
    m_request_buf += "Host: " + m_host + "\r\n";
    m_request_buf += "\r\n";

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {

        cancel_lock.unlock();
        on_finish(boost::system::error_code( boost::asio::error::operation_aborted) );
        return;
    }

    boost::asio::async_write(m_sock, boost::asio::buffer(m_request_buf),

        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            on_request_sent(ec, bytes_transferred);
        }
    );
}


void HTTPRequest::on_request_sent(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) {

        on_finish(ec);
        return;
    }

    m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send);

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {

        cancel_lock.unlock();
        on_finish(boost::system::error_code( boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_read_until(m_sock, m_response.get_response_buf(), "\r\n",

        [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            on_status_line_received(ec, bytes_transferred);
        }
    );
}

void HTTPRequest::on_status_line_received( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) {
        on_finish(ec);
        return;
    }

    std::string http_version;
    std::string str_status_code;
    std::string status_message;

    std::istream response_stream( &m_response.get_response_buf() );

    response_stream >> http_version;

    // std::err << "|" << http_version << "|" << std::endl;

    if (http_version != "HTTP/1.1") {

        on_finish( http_errors::invalid_response );
        return;
    }

    response_stream >> str_status_code;

    unsigned int status_code = 200;

    try {

        status_code = std::stoul(str_status_code);
    }
    catch (std::logic_error&) {

        on_finish(http_errors::invalid_response);
        return;
    }

    std::getline(response_stream, status_message, '\r');

    response_stream.get();

    m_response.set_status_code(status_code);
    m_response.set_status_message(status_message);

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {

        cancel_lock.unlock();
        on_finish(boost::system::error_code( boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_read_until(m_sock, m_response.get_response_buf(), "\r\n\r\n",

        [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            on_headers_received(ec, bytes_transferred);
        }
    );
}


void HTTPRequest::on_headers_received(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() != 0) {

        on_finish(ec);
        return;
    }

    std::string header, header_name, header_value;
    std::istream response_stream( &m_response.get_response_buf() );

    while (true) {

        std::getline(response_stream, header, '\r');

        response_stream.get();

        if (header == "")
            break;

        size_t separator_pos = header.find(':');

        if (separator_pos != std::string::npos) {

            header_name = header.substr(0, separator_pos);

            if (separator_pos < header.length() - 1) {

                header_value = header.substr(separator_pos + 1);
            }
            else {
                header_value = "";
            }
                
            m_response.add_header(header_name, header_value);
        }
    }

    std::unique_lock<std::mutex> cancel_lock(m_cancel_mux);

    if (m_was_cancelled) {

        cancel_lock.unlock();
        on_finish(boost::system::error_code( boost::asio::error::operation_aborted));
        return;
    }

    boost::asio::async_read( m_sock, m_response.get_response_buf(),

        [this]( const boost::system::error_code& ec, std::size_t bytes_transferred)
        {
            on_response_body_received(ec, bytes_transferred);
        });

    return;
}


void HTTPRequest::on_response_body_received( const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    if (ec.value() == boost::asio::error::eof) {

        on_finish(boost::system::error_code());
    }
    else {
        on_finish(ec);
    }
}


void HTTPRequest::on_finish(const boost::system::error_code& ec)
{
    if (ec.value() != 0) {
        std::cout << "Error occured! Error code = " << ec.value() << ". Message: " << ec.message() << std::endl;
    }

    m_callback(*this, m_response, ec);
    return;
}


HTTPClient::HTTPClient() {

    m_work_guard.reset( new boost::asio::executor_work_guard<boost::asio::io_context::executor_type>( m_io_ctx.get_executor() ) ) ;

    m_thread.reset(new std::thread([this](){

        m_io_ctx.run();
    }));
}

std::shared_ptr<HTTPRequest> HTTPClient::create_request(unsigned int id)
{
    return std::shared_ptr<HTTPRequest>( new HTTPRequest(m_io_ctx, id));
}

void HTTPClient::close() {

    m_work_guard.reset( nullptr );
    m_thread->join();
}