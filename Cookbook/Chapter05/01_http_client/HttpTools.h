#pragma once

#include <boost/asio.hpp>

#include <iostream>
#include <mutex>
#include <string>


namespace http_errors {

	enum http_error_codes 
    {
		invalid_response = 1
	};


	class http_errors_category : public boost::system::error_category {

        public:

            const char* name() const BOOST_SYSTEM_NOEXCEPT ;
            std::string message(int e) const ;
	};

	const boost::system::error_category& get_http_errors_category() ;
	
    boost::system::error_code make_error_code(http_error_codes e) ;

} // namespace http_errors


namespace boost {

	namespace system {

		template<>
		struct is_error_code_enum<http_errors::http_error_codes>
		{
			BOOST_STATIC_CONSTANT(bool, value = true);
		};

	} // namespace system

} // namespace boost



class HTTPClient;
class HTTPRequest;
class HTTPResponse;


typedef void(*Callback) (const HTTPRequest& request, const HTTPResponse& response, const boost::system::error_code& ec);


class HTTPResponse {

	friend class HTTPRequest;
	HTTPResponse() ;

public:

	unsigned int get_status_code() const ;
	const std::string& get_status_message() const ;
	const std::map<std::string, std::string>& get_headers() ;
	const std::istream& get_response() const ;

private:

	boost::asio::streambuf& get_response_buf() ;
	void set_status_code(unsigned int status_code) ;
	void set_status_message(const std::string& status_message) ;
	void add_header(const std::string& name, const std::string& value);

private:

	unsigned int m_status_code; 
	std::string m_status_message; 
	std::map<std::string, std::string> m_headers;
	boost::asio::streambuf m_response_buf;
	std::istream m_response_stream;
};



class HTTPRequest {

	friend class HTTPClient;
	static const unsigned int DEFAULT_PORT = 80;

	HTTPRequest(boost::asio::io_service& ios, unsigned int id) ;

public:

	void set_host(const std::string& host) ;
	void set_port(unsigned int port) ;
	void set_uri(const std::string& uri) ;
	void set_callback(Callback callback) ;
	std::string get_host() const ;
	unsigned int get_port() const ;
	const std::string& get_uri() const ;
	unsigned int get_id() const ;
	void execute() ;
	void cancel() ;

private:

	void on_host_name_resolved( const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator);
	void on_connection_established( const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator iterator);
	void on_request_sent(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_status_line_received( const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_headers_received(const boost::system::error_code& ec, std::size_t bytes_transferred) ;
	void on_response_body_received( const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_finish(const boost::system::error_code& ec) ;

private:

	std::string m_host;
	unsigned int m_port;
	std::string m_uri;
	unsigned int m_id;
	Callback m_callback;
	std::string m_request_buf;
	boost::asio::ip::tcp::socket m_sock;
	boost::asio::ip::tcp::resolver m_resolver;
	HTTPResponse m_response;
	bool m_was_cancelled;
	std::mutex m_cancel_mux;
	boost::asio::io_context& m_io_ctx;
};



class HTTPClient {

public:

	HTTPClient() ;
	std::shared_ptr<HTTPRequest> create_request(unsigned int id);
	void close() ;

private:

	boost::asio::io_context m_io_ctx;
	std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_work_guard;
	std::unique_ptr<std::thread> m_thread;
};
