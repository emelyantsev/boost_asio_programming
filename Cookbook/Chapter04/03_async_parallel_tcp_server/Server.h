#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>
#include <iostream>

class Service {

public:

	Service(std::shared_ptr<boost::asio::ip::tcp::socket> sock) ;

	void StartHandling() ;

private:

	void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) ;
	void onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred) ;
	void onFinish() ;

	std::string ProcessRequest(boost::asio::streambuf& request) ;

    void print_buf(boost::asio::streambuf& buf) ;
    void print_thread_id(const std::string& message) ;

private:

	std::shared_ptr<boost::asio::ip::tcp::socket> m_sock;
	std::string m_response;
	boost::asio::streambuf m_request;
};


class Acceptor {

public:

	Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) ;

	void Start() ;
	void Stop() ;

private:

	void InitAccept() ;
	void onAccept(const boost::system::error_code& ec, std::shared_ptr<boost::asio::ip::tcp::socket> sock);

private:

	boost::asio::io_context& m_io_ctx;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool> m_isStopped;
};


class Server {

public:

	Server() ;

	void Start(unsigned short port_num, unsigned int thread_pool_size) ;
	void Stop() ;

private:

	boost::asio::io_context m_io_ctx;
	std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_work_guard;
	std::unique_ptr<Acceptor> acc;
	boost::thread_group m_thread_pool;
};
