#ifndef async_tcp_client_h
#define async_tcp_client_h

#include <boost/predef.h>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include <map>


typedef void(*Callback) (unsigned int request_id, const std::string& response, const boost::system::error_code& ec);


struct Session {

	Session(boost::asio::io_context& io_ctx, const std::string& raw_ip_address, unsigned short port_num, const std::string& request, unsigned int id, Callback callback) :

        m_sock(io_ctx),
        m_ep(boost::asio::ip::address::from_string(raw_ip_address), port_num ),
        m_request(request),
        m_id(id),
        m_callback(callback),
        m_was_cancelled(false) {}

	boost::asio::ip::tcp::socket m_sock; 
	boost::asio::ip::tcp::endpoint m_ep; 
	std::string m_request;
	boost::asio::streambuf m_response_buf;
	std::string m_response; 
	boost::system::error_code m_ec;
	unsigned int m_id; 
	Callback m_callback;
	bool m_was_cancelled;
	std::mutex m_cancel_guard;
};


class AsyncTCPClient : public boost::noncopyable {

public:

	AsyncTCPClient(unsigned char num_of_threads) ;

	void emulateLongComputationOp( unsigned int duration_sec, const std::string& raw_ip_address, unsigned short port_num, Callback callback, unsigned int request_id) ;	
    void cancelRequest(unsigned int request_id) ;	
    void close() ;

private:

	void onRequestComplete(std::shared_ptr<Session> session) ;

private:

	boost::asio::io_context m_io_ctx;
	std::map<int, std::shared_ptr<Session>> m_active_sessions;
	std::mutex m_active_sessions_guard;
	std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> m_work_guard;
	boost::thread_group m_threads;
};

#endif