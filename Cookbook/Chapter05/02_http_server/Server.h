#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <map>
#include <memory>
#include <string>
#include <iostream>

class Service {

	static const std::map<unsigned int, std::string> http_status_table;

public:

	Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) ;
	void start_handling() ;

private:

	void on_request_line_received( const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_headers_received(const boost::system::error_code& ec, std::size_t bytes_transferred) ;
	void process_request() ;
	void send_response()  ;
	void on_response_sent(const boost::system::error_code& ec, std::size_t bytes_transferred);
	void on_finish() ;

private:

	std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
	boost::asio::streambuf m_request;
	std::map<std::string, std::string> m_request_headers;
	std::string m_requested_resource;
	std::unique_ptr<char[]> m_resource_buffer;
	unsigned int m_response_status_code;
	std::size_t m_resource_size_bytes;
	std::string m_response_headers;
	std::string m_response_status_line;
};

// ---------------------- Acceptor ------------------ //

class Acceptor {

public:

	Acceptor(boost::asio::io_context& m_io_ctx, unsigned short port_num) ;

	void Start() ;
	void Stop() ;

private:

	void InitAccept() ;
	void onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock) ;

private:

	boost::asio::io_context& m_io_ctx;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_isStopped;
};


// -----------------------  Server ------------  //

class Server {

public:

	Server() ;

	void Start(unsigned short port_num, unsigned int thread_pool_size) ;
	void Stop() ;

private:

	boost::asio::io_context m_io_ctx;
	std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_work_guard;
	std::unique_ptr<Acceptor> acc;
	std::vector<std::unique_ptr<std::thread>> m_thread_pool;

};
