#pragma once

#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

class Service {

    public:

        Service() ;

        void HandleClient(boost::asio::ip::tcp::socket& sock) ;
};


class Acceptor {

    public:

        Acceptor(boost::asio::io_context& io_ctx, unsigned short port_num) ;

        void Accept() ;

    private:

        boost::asio::io_context& m_io_ctx;
        boost::asio::ip::tcp::acceptor m_acceptor;
};


class Server {

public:

	Server() ;

	void Start(unsigned short port_num) ;
	void Stop() ;

private:

	void Run(unsigned short port_num) ;

	std::unique_ptr<std::thread> m_thread;
	std::atomic<bool> m_stop;
	boost::asio::io_context m_io_ctx;
};
